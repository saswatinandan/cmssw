from sklearn.utils.class_weight import compute_class_weight
import numpy as np

def class_weight(data):
  classes = np.unique(data)
  weights = compute_class_weight(class_weight="balanced", classes=classes, y=data)
  return dict(zip(classes, weights))
