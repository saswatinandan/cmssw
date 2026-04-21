import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import roc_auc_score
print('calling')
from dnn import training_generator, dataset_from_generator
print('ending')
import psutil, os
import onnxruntime as ort

dir_path = "/scratch/nandan/inputfile_for_prehlt/ephemeral_zerobias/dnn_ntuple/ntuple/" #"/scratch/nandan/inputfile_for_prehlt/ephemeral/dnn_ntuple/ntuple/"
# List all files (not directories)
file_list = [os.path.join(dir_path, f) for f in os.listdir(dir_path) if os.path.isfile(os.path.join(dir_path, f))]
train_idx = round(len(file_list)*0.6)
val_idx = round(len(file_list)*0.2)
test_files  = file_list[train_idx+val_idx:][:5]
tree_name = "sep19_2_2_dump_raw/tree"

feature_branches = ['size', 'charge', 'adcs_ten', 'mean_x', 'mean_y', 'mean_z']
feature_to_use = feature_branches.copy()
feature_to_use.remove('adcs_ten')
feature_to_use.extend(["adc_at_idx_0", "adc_at_idx_1", "adc_at_idx_2", "adc_at_idx_3"])

label_branch = "target"
batch_size_train = 1024

session = ort.InferenceSession("nn_model.onnx")
input_name = session.get_inputs()[0].name
# -------------------------------
# 1. Permutation Importance Function
# -------------------------------
def permutation_importance_dataset(dataset, feature_count, metric=roc_auc_score, n_repeats=5):
    """
    Compute permutation importance directly on a tf.data.Dataset.
    
    Parameters
    ----------
    model : tf.keras.Model
        Trained DNN model
    dataset : tf.data.Dataset
        Dataset yielding (X_batch, y_batch)
    feature_count : int
        Number of features in X
    metric : callable
        Function to compute performance, e.g., roc_auc_score
    n_repeats : int
        Number of permutation repeats
    
    Returns
    -------
    importances : np.ndarray
        Array of feature importances
    """
    # Compute baseline metric
    y_true_all = []
    y_pred_all = []
    for X_batch, y_batch, _ in dataset:
        y_true_all.append(y_batch.numpy())
        y_pred_all.append(session.run(None, {input_name: X_batch.numpy().astype(np.float32)})[0].ravel())
    y_true_all = np.concatenate(y_true_all)
    y_pred_all = np.concatenate(y_pred_all)
    baseline = metric(y_true_all, y_pred_all)
    
    importances = np.zeros(feature_count)

    for col in range(feature_count):
        scores = []
        for _ in range(n_repeats):
            print('repeating col: ', col, '\tfor: ', _)
            y_pred_permuted_all = []
            for X_batch, y_batch, _ in dataset:
                X_batch_np = X_batch.numpy().astype(np.float32)
                X_batch_permuted = X_batch_np.copy()
                np.random.shuffle(X_batch_permuted[:, col])
                y_pred_permuted_all.append(session.run(None, {input_name: X_batch_permuted})[0].ravel())
            y_pred_permuted_all = np.concatenate(y_pred_permuted_all)
            scores.append(baseline - metric(y_true_all, y_pred_permuted_all))
        importances[col] = np.mean(scores)

    return importances

# -------------------------------
# 2. Plotting Function
# -------------------------------
def plot_importance(importance, feature_names, title="Permutation Feature Importance"):
    """
    Plot a bar chart of feature importance.

    Parameters
    ----------
    importance : np.ndarray
        Feature importance values
    feature_names : list of str
        Names of features corresponding to importance
    """
    idx = np.argsort(importance)[::-1]
    plt.figure(figsize=(10,6))
    plt.bar(range(len(importance)), importance[idx], color='skyblue')
    plt.xticks(range(len(importance)), np.array(feature_names)[idx], rotation=90)
    plt.ylabel("Permutation Importance")
    plt.title(title)
    plt.tight_layout()
    plt.savefig('importance.png')

# -------------------------------
# 3. Example Usage
# -------------------------------

# Suppose you have these from your code
# from dnn import training_generator, dataset_from_generator
print("calling generator")

test_dataset = dataset_from_generator(
    test_files, tree_name, feature_branches, label_branch, feature_to_use, batch_size=8000
).prefetch(tf.data.AUTOTUNE)

feature_count = len(feature_to_use)  # number of features
feature_names = feature_to_use       # list of feature names

# Compute permutation importance
importance = permutation_importance_dataset(test_dataset, feature_count)

# Plot feature importance
plot_importance(importance, feature_names)
