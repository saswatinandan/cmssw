from tensorflow.keras import layers
from tensorflow.keras.callbacks import EarlyStopping
import optuna
from tensorflow import keras
from scikeras.wrappers import KerasClassifier
from sklearn.datasets import make_classification
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
import uproot
import pandas as pd
import numpy as np
import awkward as ak
from sklearn.utils.class_weight import compute_class_weight
from tensorflow.keras.layers import Input, Dense, Normalization, Dropout
from tensorflow.keras.models import Model
from tensorflow.keras.models import Sequential
import matplotlib.pyplot as plt
import os
from tensorflow.keras.callbacks import TensorBoard
import tensorflow as tf
from tensorboard.plugins.hparams import api as hp
from collections import OrderedDict
import gc
import tensorflow.keras.backend as K
import uuid
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import roc_auc_score
from evaluate_model import evaluate_model, calculate_l_score
from load_gpu import load_gpu
import json
from dataloader import data_loader_all
from optuna.integration import TFKerasPruningCallback
from optuna.pruners import ThresholdPruner
from callbacks import MemoryClearCallback
load_gpu()
logdir = os.path.join("logs", "bayes_opt_log")
tf.debugging.set_log_device_placement(True)

varlists = ['size', 'charge', 'max_adc', 'n_saturated', 'x','y','z', 'max_adc_x', 'max_adc_y', 'max_adc_z', 'diff_adc_pone', 'diff_adc_ptwo', 'diff_adc_pthree', 'diff_adc_mone', 'diff_adc_mtwo', 'diff_adc_mthree', 'layer']
df, var = data_loader_all('test.root', varlists, frac=1)
varlists = [v for v in varlists if v not in ['size', 'charge']]
X_train = df[varlists].values
y_train = df['target'].values
X_fornorm = df[varlists]
# --- Normalization Layer ---
normalizer = Normalization()
with tf.device('/CPU:0'):
    normalizer.adapt(df[varlists].values.astype('float32'))

from reduce_lr import reduce_lr
from early_stopping import early_stopping, RecallEarlyStopping
recall_earlystopping = RecallEarlyStopping()
from create_model import create_model

def save_best_callback(study, trial):
       best_result = {
               "trial_number": study.best_trial.number,
               "value": study.best_value,
               "params": study.best_params,
       }
       with open("best_result.json", "w") as f:
            json.dump(best_result, f, indent=4)

def objective(trial):
        n_batch_size = trial.suggest_int('units_batch_size', 9, 12)
        batch_size = 2 ** n_batch_size

        units_exp = trial.suggest_int('units_exp', 3, 8)  # units: 32 to 256
        n_units = 2 ** units_exp

        lr = trial.suggest_loguniform('lr', 1e-5, 1e-2)
        n_layers = trial.suggest_int('n_layers', 3, 12)
        layernorm = trial.suggest_int('layer_norm', 0, 1)
        dropout   = trial.suggest_float('dropout', 0.0,0.10, step=0.01)
        batchnorm = 0
        if layernorm == 0:
            batchnorm = trial.suggest_int('batch_norm', 0, 1)
        L2 = trial.suggest_float("l2", 1e-5, 1e-1, log=True)
        
        d_scores = []
        skf = StratifiedKFold(n_splits=2, shuffle=True)
        for train_idx, val_idx in skf.split(X_train, y_train):
        
          X_tr, X_val = X_train[train_idx], X_train[val_idx]
          y_tr, y_val = y_train[train_idx], y_train[val_idx]

          classes = np.unique(y_tr)
          weights = compute_class_weight(class_weight="balanced", classes=classes, y=y_tr)
          class_weight_dict = dict(zip(classes, weights))
          sig_w = trial.suggest_float('sig_w', 0.30, 1.30, step=0.05)
          class_weight_dict[1] *= sig_w
          val_weights = np.where(y_val == 1, class_weight_dict[1], class_weight_dict[0])

          model = create_model(n_units, n_layers, lr, len(varlists), normalizer=normalizer, layernorm=layernorm, batchnorm=batchnorm, dropout=dropout, L2=L2)
          history = model.fit(
            X_tr.astype('float32'), y_tr,
            validation_data=(X_val.astype('float32'), y_val, val_weights),
            epochs=30,
            batch_size=batch_size,
            callbacks=[early_stopping, reduce_lr, recall_earlystopping, MemoryClearCallback()],#, pruning_callback],
            class_weight=class_weight_dict,
            validation_batch_size = 4096,
            verbose=1
          )

          val_recall = history.history['val_recall'][early_stopping.best_epoch]
          val_auc    = history.history['val_auc'][early_stopping.best_epoch]
          if val_recall <= 0.10 or val_auc <= 0.75:
              print(f'val recall is {val_recall}, val auc {val_auc}')
              return val_auc

          d_scores.append(evaluate_model(model, X_tr, X_val, y_tr, y_val, True))
          if d_scores[-1] <= 0.7525:
              return d_scores[-1]

        mean_s, std_s = np.mean(d_scores), np.std(d_scores)
        return (mean_s - std_s)#, (mean_l - std_l)

#threshold_pruner = ThresholdPruner(lower=0.74, n_warmup_steps=20)
study = optuna.create_study(study_name="example",
            storage="sqlite:///optuna_study.db",
            load_if_exists=True,
            #pruner=threshold_pruner,
            direction='maximize')#directions=['maximize','minimize'])
study.optimize(objective, n_trials=100, catch=(tf.errors.InternalError,), callbacks=[save_best_callback])

best_params = study.best_trial.params
print("Best hyperparameters:",best_params)
with open("best_params.json", "w") as f:
       json.dump(best_params, f, indent=4)

# Convert all trials to DataFrame
df = study.trials_dataframe()
df.to_csv("all_trials.csv", index=False)

# Or save to JSON
all_trials_serializable = []

for t in study.trials:
    trial_dict = {
        "number": t.number,
        "value": t.value,
        "params": t.params,
        "state": str(t.state),
        "datetime_start": t.datetime_start.isoformat() if t.datetime_start else None,
        "datetime_complete": t.datetime_complete.isoformat() if t.datetime_complete else None
    }
    all_trials_serializable.append(trial_dict)

# Save to JSON
with open("all_trials.json", "w") as f:
    json.dump(all_trials_serializable, f, indent=4)
