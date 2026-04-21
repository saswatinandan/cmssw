import tensorflow as tf
import numpy as np
import uproot  # if reading ROOT files
import psutil, os
from collections import defaultdict

from load_gpu import load_gpu
load_gpu()

from create_model import create_model, save_model
from callbacks import MemoryClearCallback, MemoryLogger
from early_stopping import early_stopping, RecallEarlyStopping
from reduce_lr import reduce_lr, cosinedecay, LrMetricsLogger
from plotter import plot_loss_accuracy, plot_var_dist, plot_loss_accuracy_true
from evaluate_model import evaluate_model_from_generator

process = psutil.Process(os.getpid())
def print_mem(msg=""):
    ram_gb = process.memory_info().rss / 1e9
    gpu_mem = tf.config.experimental.get_memory_info('GPU:0')['current'] / 1e9
    print(f"{msg} | RAM: {ram_gb:.2f} GB | GPU: {gpu_mem:.2f} GB")

# -------------------------
# 1. Generator yielding batches
# -------------------------
'''
def training_generator(files, feature_to_use, class_weight_dict=None, batch_size=1024):
    """
    Yields batches of (features, labels, sample_weights).
    Replace ROOT reading with your actual code.
    """
    for fname in files:
        # -----------------
        # Example data: replace with uproot reading
        # -----------------
        with uproot.open(fname) as f:
            tree = f[tree_name]
            arrays = tree.arrays(feature_branches + [label_branch], library="np")
            adc_first4 = np.array(
                    [x[:4] for x in arrays["adcs_ten"]]
            )
            arrays["adc_at_idx_0"] = adc_first4[:, 0]
            arrays["adc_at_idx_1"] = adc_first4[:, 1]
            arrays["adc_at_idx_2"] = adc_first4[:, 2]
            arrays["adc_at_idx_3"] = adc_first4[:, 3]
            del adc_first4
            x = np.column_stack([arrays[b] for b in feature_to_use]).astype(np.float32)
            y = arrays[label_branch].astype(np.int32)
            if class_weight_dict:
               w = np.array([class_weight_dict[int(yi)] for yi in y], dtype=np.float32)
            else:
               w = np.ones_like(y, dtype=np.float32)
            print_mem("After opening tree")
            for i in range(0, w.shape[0], batch_size):
               batch_x = x[i:i+batch_size]
               batch_y = y[i:i+batch_size]
               batch_w = w[i:i+batch_size]
               yield batch_x, batch_y, batch_w
            del x,y,w
'''
def training_generator(files, tree_name, feature_branches, label_branch, feature_to_use, class_weight_dict=None, batch_size=1024):
    for fname in files:
        print_mem(f"Opening file {fname}")
        # Use iterate to stream events in batch_size chunks
        for arrays in uproot.iterate(
            fname,
            treepath=tree_name,
            expressions=feature_branches + [label_branch],
            step_size=batch_size,  # only read step_size events at a time
            library="np"
        ):
            # Process adcs_ten first 4 elements
            if "adcs_ten" in arrays:
                adc_first4 = np.array([x[:4] for x in arrays["adcs_ten"]])
                for i in range(4):
                    arrays[f"adc_at_idx_{i}"] = adc_first4[:, i]
                del adc_first4
            # Stack features
            x = np.column_stack([arrays[b] for b in feature_to_use])
            y = arrays[label_branch]

            # Sample weights
            if class_weight_dict:
                w = np.array([class_weight_dict[int(yi)] for yi in y], dtype=np.float32)
            else:
                w = np.ones_like(y, dtype=np.float32)

            yield x, y, w
        del x,y,w
        print_mem(f"Finished file {fname}")

# -------------------------
# 2. Create tf.data.Dataset
# -------------------------
def dataset_from_generator(files, tree_name, feature_branches, label_branch, feature_to_use, batch_size, class_weight_dict=None, shuffle_buffer=0):
    """
    Returns tf.data.Dataset yielding ((features, labels), sample_weights)
    """
    output_signature = (
        tf.TensorSpec(shape=(None, len(feature_to_use)), dtype=tf.float32),  # batch features
        tf.TensorSpec(shape=(None,), dtype=tf.int32),                        # batch labels
        tf.TensorSpec(shape=(None,), dtype=tf.float32)                       # batch weights
    )

    ds = tf.data.Dataset.from_generator(
        lambda: training_generator(files, tree_name, feature_branches, label_branch, feature_to_use, class_weight_dict, batch_size),
        output_signature=output_signature
    )

    # Shuffle batches if training
    if shuffle_buffer > 0:
        ds = ds.shuffle(shuffle_buffer)

    # Wrap for model.fit ((features, labels), sample_weights)
    ds = ds.map(lambda x, y, w: (tf.cast(x, tf.float32), y, w))
    ds = ds.prefetch(tf.data.AUTOTUNE)
    print('ds: ', ds)
    print_mem("within data generator")
    return ds

def main():

  logdir = os.path.join("logs", "full_train")
  tensorboard_cb = tf.keras.callbacks.TensorBoard(log_dir=logdir)

  # Directory path
  dir_path = "/scratch/nandan/inputfile_for_prehlt/ephemeral_zerobias/dnn_ntuple/ntuple/" #"/scratch/nandan/inputfile_for_prehlt/ephemeral/dnn_ntuple/ntuple/"
  # List all files (not directories)
  file_list = [os.path.join(dir_path, f) for f in os.listdir(dir_path) if os.path.isfile(os.path.join(dir_path, f))]
  print(file_list)
  train_idx = round(len(file_list)*0.6)
  val_idx = round(len(file_list)*0.2)
  train_files = file_list[:train_idx]          # list of ROOT files for training
  val_files   = file_list[train_idx:train_idx+val_idx]# list of ROOT files for validation
  test_files  = file_list[train_idx+val_idx:]

  tree_name = "sep19_2_2_dump_raw/tree"
  feature_branches = ['size', 'charge', 'adcs_ten', 'mean_x', 'mean_y', 'mean_z']
  feature_to_use = feature_branches.copy()
  feature_to_use.remove('adcs_ten')
  feature_to_use.extend(["adc_at_idx_0", "adc_at_idx_1", "adc_at_idx_2", "adc_at_idx_3"])

  label_branch = "target"
  batch_size_train = 1024
  epochs = 100

  class_counts = defaultdict(int)
  total_count = 0

  for fname in train_files:
    with uproot.open(fname) as f:
        tree = f[tree_name]
        y = tree.arrays(label_branch, library="np")[label_branch]
        # vectorized counting per file
        classes, counts = np.unique(y, return_counts=True)
        for c, cnt in zip(classes, counts):
            class_counts[int(c)] += int(cnt)

        total_count += len(y)

  # Compute weights
  classes = np.array(list(class_counts.keys()))
  counts = np.array([class_counts[c] for c in classes])

  weights = total_count / (len(classes) * counts)
  print(classes, '\t', counts, '\t', weights)
  class_weight_dict = dict(zip(classes, weights))
  #class_weight_dict[1] *= 0.90
  del total_count
  print("Training class weights:", class_weight_dict)
  
  train_dataset = dataset_from_generator(
          train_files, tree_name, feature_branches, label_branch, feature_to_use, batch_size=batch_size_train, class_weight_dict=class_weight_dict, shuffle_buffer=200
  ).prefetch(tf.data.AUTOTUNE)

  val_dataset = dataset_from_generator(
          val_files, tree_name, feature_branches, label_branch, feature_to_use, batch_size=8000, class_weight_dict=class_weight_dict, shuffle_buffer=200
  ).prefetch(tf.data.AUTOTUNE)

  test_dataset = dataset_from_generator(
          test_files, tree_name, feature_branches, label_branch, feature_to_use, batch_size=8000
  ).prefetch(tf.data.AUTOTUNE)


  # -------------------------
  # 4. Define model
  # -------------------------
  normalizer = tf.keras.layers.Normalization()
  features_ds = train_dataset.map(lambda x, y, w: tf.cast(x, tf.float32))
  normalizer.adapt(features_ds)

  model = create_model(64,5,0.000029, len(feature_to_use), normalizer=normalizer, layernorm=1, batchnorm=0, dropout=0.0, L2=0.02604)

  # -------------------------
  # 6. Train model
  # -------------------------
  history = model.fit(
    train_dataset,
    validation_data=val_dataset,
    epochs=10,
    callbacks=[tensorboard_cb, early_stopping, MemoryClearCallback()]
  )
  plot_loss_accuracy(history)
  save_model(model, len(feature_to_use))
  train_dataset = dataset_from_generator(
    train_files, feature_to_use, batch_size=batch_size_train, class_weight_dict=class_weight_dict
  ).prefetch(tf.data.AUTOTUNE)


  evaluate_model_from_generator(model, train_dataset, test_dataset)

if __name__ == "__main__":
    main()
