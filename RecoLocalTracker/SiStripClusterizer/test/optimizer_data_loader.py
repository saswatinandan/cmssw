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
from evaluate_model import evaluate_model, evaluate_model_from_generator, calculate_l_score
from load_gpu import load_gpu
import json
from dataloader import data_loader_all, uproot_batch_generator 
from optuna.integration import TFKerasPruningCallback
from optuna.pruners import ThresholdPruner
from callbacks import MemoryClearCallback
load_gpu()
logdir = os.path.join("logs", "bayes_opt_log")
tf.debugging.set_log_device_placement(True)

file = "test_HI_all.root"
tree_sig = "sep19_2_2_dump_raw/signalTree"
tree_bkg = "sep19_2_2_dump_raw/bkgTree"
varlists = ['size', 'charge', 'max_adc', 'x','y','z', 'max_adc_x', 'max_adc_y', 'max_adc_z', 'diff_adc_pone', 'diff_adc_ptwo', 'diff_adc_pthree', 'diff_adc_mone', 'diff_adc_mtwo', 'diff_adc_mthree']
drop_vars = ["size", 'charge', 'x','y','z']

with uproot.open(file)[tree_sig] as tree_s:
        N_sig = tree_s.num_entries
with uproot.open(file)[tree_bkg] as tree_b:
        N_bkg = tree_b.num_entries

ratio_sig = N_sig / (N_sig + N_bkg)
totsig = 4000000
totbkg = totsig * N_bkg / N_sig
trainfrac = 0.70
valfrac = 0.30
evaluationfrac = 0.30

def mixed_sig_bkg_generator(file, tree_sig, tree_bkg, varlist, batch_size=1024, drop_vars=None, train=True, val=False, evaluation=False):

    n_sig_batch = int(batch_size * ratio_sig)
    n_bkg_batch = batch_size - n_sig_batch

    if train:
        start_idx_sig = 0
        end_idx_sig   = totsig * trainfrac if evaluation == False else 2000000
        start_idx_bkg = 0
        end_idx_bkg   = totbkg * trainfrac if evaluation == False else (2000000 * N_bkg / N_sig)
    elif val:
        start_idx_sig = totsig * trainfrac
        end_idx_sig   = totsig
        start_idx_bkg = totbkg * trainfrac
        end_idx_bkg   = totbkg

    print('train: ', train, '\t', evaluation, '\t', start_idx_sig, '\t', end_idx_sig, '\t', start_idx_bkg, '\t', end_idx_bkg)
    while True:
        sig_gen = uproot_batch_generator(file, tree_sig, varlist, start_idx_sig, end_idx_sig, n_sig_batch, drop_vars=drop_vars)
        bkg_gen = uproot_batch_generator(file, tree_bkg, varlist, start_idx_bkg, end_idx_bkg, n_bkg_batch, drop_vars=drop_vars)
        while True:
            X_sig, y_sig = next(sig_gen)
            X_bkg, y_bkg = next(bkg_gen)

            # Combine and shuffle
            X_batch = np.concatenate([X_sig, X_bkg], axis=0)
            y_batch = np.concatenate([y_sig, y_bkg], axis=0)
            w_sig = ( n_bkg_batch + n_sig_batch ) / ( 2. * n_sig_batch )
            w_sig *= 1. #0.60
            w_bkg = 1. #( n_bkg_batch + n_sig_batch ) / ( 2. * n_bkg_batch )
            ws    = np.where(y_batch == 1, w_sig, w_bkg).astype(np.float32)

            perm = np.random.permutation(len(y_batch))
            X_batch = X_batch[perm]
            y_batch = y_batch[perm]
            ws_batch= ws[perm]
            yield X_batch, y_batch, ws_batch

def tf_generator(train=True,val=False,evaluation=False,batch_size=1024):
    for X_batch, y_batch, w_batch in mixed_sig_bkg_generator(file, tree_sig, tree_bkg, varlists, batch_size, drop_vars=drop_vars, train=train, val=val,evaluation=evaluation):
        yield X_batch, y_batch, w_batch

#df, var = data_loader_all('test.root', varlists, frac=1)
#varlists = [v for v in varlists if v not in ['size', 'charge']]
#X_train = df[varlists].values
#y_train = df['target'].values
#X_fornorm = df[varlists]
# --- Normalization Layer ---
#normalizer = Normalization()
#with tf.device('/CPU:0'):
#    normalizer.adapt(df[varlists].values.astype('float32'))

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

# Create the normalizer layer
train_ds = tf.data.Dataset.from_generator(
    tf_generator,
    args=(True,False,False,1024),
    output_signature=(
        tf.TensorSpec(shape=(None, 14), dtype=tf.float32),
        tf.TensorSpec(shape=(None,), dtype=tf.float32),
        tf.TensorSpec(shape=(None,), dtype=tf.float32)
    )
   ).prefetch(tf.data.AUTOTUNE)

normalizer = tf.keras.layers.Normalization(axis=-1)
# Iterate over all batches in train_ds to adapt
for i, (X_batch, y_batch, _) in enumerate(train_ds):
    normalizer.adapt(X_batch)
    if i == 10:  # stop after one full epoch
        break

def objective(trial):
        n_batch_size = trial.suggest_int('units_batch_size', 9, 12)
        batch_size = 2 ** n_batch_size

        #sig_idx = np.random.randint(0, N_sig-totsig)
        #bkg_idx = np.random.randint(0, N_bkg-(totsig*N_bkg/N_sig))
        train_ds = tf.data.Dataset.from_generator(
          tf_generator,
          args=(True,False,False,batch_size),
          output_signature=(
            tf.TensorSpec(shape=(None, 14), dtype=tf.float32),
            tf.TensorSpec(shape=(None,), dtype=tf.float32),
            tf.TensorSpec(shape=(None,), dtype=tf.float32)
           )
          ).prefetch(tf.data.AUTOTUNE)
        val_ds = tf.data.Dataset.from_generator(
          tf_generator,
          args=(False,True,False,batch_size),
          output_signature=(
            tf.TensorSpec(shape=(None, 14), dtype=tf.float32),
            tf.TensorSpec(shape=(None,), dtype=tf.float32),
            tf.TensorSpec(shape=(None,), dtype=tf.float32)
          )
        ).prefetch(tf.data.AUTOTUNE)
        units_exp = trial.suggest_int('units_exp', 3, 8)  # units: 32 to 256
        n_units = 2 ** units_exp

        lr = trial.suggest_loguniform('lr', 1e-5, 1e-2)
        n_layers = trial.suggest_int('n_layers', 3, 6)
        layernorm = trial.suggest_int('layer_norm', 0, 1)
        dropout   = trial.suggest_float('dropout', 0.0,0.10, step=0.01)
        batchnorm = 0
        if layernorm == 0:
            batchnorm = trial.suggest_int('batch_norm', 0, 1)
        L2 = trial.suggest_float("l2", 1e-5, 1e-1, log=True)
        
        #d_scores = []
        #skf = StratifiedKFold(n_splits=2, shuffle=True)
        #for train_idx, val_idx in skf.split(X_train, y_train):
        
        #X_tr, X_val = X_train[train_idx], X_train[val_idx]
        #y_tr, y_val = y_train[train_idx], y_train[val_idx]

        #classes = np.unique(y_tr)
        #weights = compute_class_weight(class_weight="balanced", classes=classes, y=y_tr)
        #class_weight_dict = dict(zip(classes, weights))
        #sig_w = trial.suggest_float('sig_w', 0.30, 1.30, step=0.05)
        #class_weight_dict[1] *= sig_w
        #val_weights = np.where(y_val == 1, class_weight_dict[1], class_weight_dict[0])

        model = create_model(n_units, n_layers, lr, 14, normalizer=normalizer, layernorm=layernorm, batchnorm=batchnorm, dropout=dropout, L2=L2)
        '''history = model.fit(
            X_tr.astype('float32'), y_tr,
            validation_data=(X_val.astype('float32'), y_val, val_weights),
            epochs=30,
            batch_size=batch_size,
            callbacks=[early_stopping, reduce_lr, recall_earlystopping, MemoryClearCallback()],#, pruning_callback],
            class_weight=class_weight_dict,
            validation_batch_size = 4096,
            verbose=1
        )'''
        steps_per_epoch = ( ( totsig + totbkg ) * trainfrac ) // batch_size
        steps_per_epoch_val = ( ( totsig + totbkg ) * valfrac ) // batch_size
        history = model.fit(
            train_ds,
            epochs=15,
            steps_per_epoch=steps_per_epoch,
            validation_data=val_ds,
            validation_steps=steps_per_epoch_val,
            callbacks=[early_stopping, reduce_lr, recall_earlystopping, MemoryClearCallback()]
        )

        val_recall = history.history['val_recall'][early_stopping.best_epoch]
        val_auc    = history.history['val_auc'][early_stopping.best_epoch]
        if val_recall <= 0.10 or val_auc <= 0.66:
              print(f'val recall is {val_recall}, val auc {val_auc}')
              return 0
        
        steps_per_epoch = ( ( 2000000 + (2000000*N_bkg/N_sig) ) * trainfrac ) // batch_size
        steps_per_epoch_val = ( ( 1000000 + (1000000*N_bkg/N_sig) ) * valfrac ) // batch_size
        return evaluate_model_from_generator(model, tf_generator(True,False,True), int(steps_per_epoch), tf_generator(False,True), int(steps_per_epoch_val), True) 
        #d_scores.append(evaluate_model(model, X_tr, X_val, y_tr, y_val, True))
        #  if d_scores[-1] <= 0.7525:
        #      return d_scores[-1]

        #mean_s, std_s = np.mean(d_scores), np.std(d_scores)
        #return (mean_s - std_s)#, (mean_l - std_l)

study = optuna.create_study(study_name="example",
            storage="sqlite:///optuna_study.db",
            load_if_exists=True,
            direction='maximize')
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
