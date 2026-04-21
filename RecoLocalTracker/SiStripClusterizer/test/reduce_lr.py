import tensorflow as tf
reduce_lr = tf.keras.callbacks.ReduceLROnPlateau(
        monitor='val_loss',   # metric to monitor
        factor=0.5,           # multiply lr by this factor
        patience=3,           # number of epochs with no improvement before reducing lr
        verbose=1,
        min_lr=1e-6
)

cosinedecay = tf.keras.optimizers.schedules.CosineDecayRestarts(
    initial_learning_rate=0.000029,
    first_decay_steps=10000,
    t_mul=2,
    m_mul=1.0,
    alpha=1e-6
)

lr_schedule = tf.keras.optimizers.schedules.ExponentialDecay(
            initial_learning_rate=2.9e-5,
            decay_steps=500000,
            decay_rate=0.96
)

class LrMetricsLogger(tf.keras.callbacks.Callback):
    """Prints loss, metrics, and learning rate per batch or per epoch."""
    def __init__(self, per_batch=True):
        super().__init__()
        #self.per_batch = per_batch

    def on_train_batch_end(self, batch, logs=None):
            #if self.per_batch:
            logs = logs or {}
            # Get current LR from optimizer (works with schedules)
            lr = float(tf.keras.backend.get_value(self.model.optimizer.lr))
            logs['lr'] = lr
            # Print metrics and LR nicely
            metrics_str = " - ".join([f"{k}: {v:.4f}" for k, v in logs.items()])
            print(f" {metrics_str}")

    def on_epoch_end(self, epoch, logs=None):
            #if not self.per_batch:
            logs = logs or {}
            lr = float(tf.keras.backend.get_value(self.model.optimizer.lr))
            logs['lr'] = lr
            metrics_str = " - ".join([f"{k}: {v:.4f}" for k, v in logs.items()])
            print(f"Epoch {epoch+1}: {metrics_str}")
