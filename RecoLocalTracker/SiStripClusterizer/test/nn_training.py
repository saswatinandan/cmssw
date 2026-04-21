import pandas as pd
import numpy as np
import uproot
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler, MinMaxScaler, OneHotEncoder
from sklearn.utils.class_weight import compute_class_weight
from sklearn.metrics import roc_curve, auc
from sklearn.utils import resample, shuffle
import matplotlib.pyplot as plt
import awkward as ak
import tensorflow as tf
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Input, Dense, Lambda, Concatenate,Normalization, Dropout
import seaborn as sns
from plotter import plot_loss_accuracy, plot_var_dist, plot_loss_accuracy_true
from evaluate_model import evaluate_model,evaluate_model_onnx
import os
from tensorflow.keras.callbacks import TensorBoard
from create_model import save_model
from create_varlists import save_json
logdir = os.path.join("logs", "full_train")
tensorboard_cb = TensorBoard(log_dir=logdir)
from callbacks import MemoryClearCallback
from reduce_lr import reduce_lr
from early_stopping import early_stopping, RecallEarlyStopping
import os
from create_model import create_model
from dataloader import data_loader_all
from load_gpu import load_gpu
load_gpu()

varlists = ['size', 'charge', 'adcs_ten', 'mean_x', 'mean_y', 'mean_z', 'target']
#df, varlists = data_loader_all('/home/users/nandan/tmp/CMSSW_14_0_11/src/RecoLocalTracker/SiStripClusterizer/test/test.root', varlists, frac=1)#/home/users/nandan/tmp/CMSSW_14_1_5/src/RecoLocalTracker/SiStripClusterizer/test/test_pp.root', varlists, frac=1)
df, varlists = data_loader_all('/scratch/nandan/inputfile_for_prehlt/ephemeral/dnn_ntuple/hadd_ntuple.root', varlists, frac=1)
varlists = [v for v in varlists if v not in ['target', 'adcs_ten', 'pixeltrk_validsharedhits', 'recotrk_validpixelhits', 'recotrk_pt', 'pixeltrk_pt', 'dr_reco_vs_pixel']]
print(varlists)
X = df[varlists]
Y = df['target']

X, X_test, Y, y_test = train_test_split(X, Y, stratify=Y, test_size=0.2, random_state=42)
X_train, X_val, y_train, y_val = train_test_split(X, Y, stratify=Y, test_size=0.2, random_state=42)

df_train = pd.DataFrame(X_train, columns=X_train.columns)
df_train['target'] = y_train.values
'''plot_var_dist(df_train, varlists)
print(varlists)
save_json(varlists, 'final_variable')
classes = np.unique(y_train)
weights = compute_class_weight(class_weight="balanced", classes=classes, y=Y)
class_weight_dict = dict(zip(classes, weights))

print(class_weight_dict)
normalizer = Normalization()
with tf.device('/CPU:0'):
    #normalizer.adapt(X_train.iloc[:, cols_to_normalize].values)
    normalizer.adapt(X_train.values.astype('float32'))
model = create_model(64,5,0.000029, len(varlists), normalizer=normalizer, layernorm=1, batchnorm=0, dropout=0.0, L2=0.02604)#256,11,0.0000457)

#class_weight_dict[1] *= 0.60
#class_weight_dict[0] = 1.0
print('class: ', class_weight_dict)
val_weights = np.where(y_val == 1, class_weight_dict[1], class_weight_dict[0])
history = model.fit(X_train.astype('float32'),
        y_train,
        epochs=2, batch_size=1024,
        validation_data=(X_val.astype('float32'), y_val, val_weights),
        class_weight=class_weight_dict,
        callbacks=[tensorboard_cb, early_stopping, reduce_lr, MemoryClearCallback()],
        validation_batch_size = 8191
        )

del X_val, y_val, df
plot_loss_accuracy(history)
save_model(model, len(varlists))

evaluate_model(model, X_train, X_test, y_train, y_test)
'''
evaluate_model_onnx(X_train, X_test, y_train, y_test)
