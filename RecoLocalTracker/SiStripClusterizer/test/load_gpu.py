import tensorflow as tf

def load_gpu():
  gpus = tf.config.list_physical_devices('GPU')
  print("GPUs Available:", gpus)

  if gpus:
    # Optional: set memory growth to avoid allocating all GPU memory at once
    for gpu in gpus:
        tf.config.experimental.set_memory_growth(gpu, True)

  print("Is TensorFlow built with CUDA:", tf.test.is_built_with_cuda())
  print("Is GPU available:", tf.test.is_gpu_available())  # Deprecated but still works in some versions
  print("Logical GPUs:", tf.config.list_logical_devices('GPU'))
