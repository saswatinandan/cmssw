from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.callbacks import Callback

class WarmupEarlyStopping(EarlyStopping):
        def __init__(self, start_epoch=5, **kwargs):
            super().__init__(**kwargs)
            self.start_epoch = start_epoch

        def on_epoch_end(self, epoch, logs=None):
            if epoch < self.start_epoch:
                   return  # skip checking until after warmup
            super().on_epoch_end(epoch, logs)

class RecallEarlyStopping(Callback):
    def on_epoch_end(self, epoch, logs=None):
        val_recall = logs.get("val_recall")
        val_auc    = logs.get("val_auc")
        if ((val_recall <= 0.02 or val_auc <= 0.50) and epoch >= 4):# or (val_auc <= 0.74 and epoch >= 20):   # stop immediately if val_recall is zero
                print(f"\nEpoch {epoch+1}: {val_recall}, {val_auc}, stopping training.")
                self.model.stop_training = True

early_stopping = WarmupEarlyStopping(
            monitor='val_auc', start_epoch=0, patience=10, min_delta=0.001, mode='max',
            restore_best_weights=True
        )
