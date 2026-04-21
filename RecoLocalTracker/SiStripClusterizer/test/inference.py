import numpy as np
import tensorflow as tf
from sklearn.metrics import roc_curve, auc
import onnxruntime as ort
from dataloader import *

def do_inference(onnx_model_path='nn_model_with_drmin.onnx', threshold=True):
  # Load model
  session = ort.InferenceSession(onnx_model_path)

  # Check input/output names
  input_name = session.get_inputs()[0].name
  output_name = session.get_outputs()[0].name
  print("Input name:", input_name)
  print("Output name:", output_name)

  my_values = [180, 254, 0.00306964, 0.0108296, 0.00113729, 24.2294, 0.72224, -4.13304, 0, 0, 221, 148, 0, 0, 99 ]
  X_test = np.array([my_values], dtype=np.float32)

  # Run inference
  y_pred = session.run([output_name], {input_name: X_test})

  # y_pred is a list with one element (the model output)
  print("Predictions:", y_pred[0])
  #if threshold:
  #    find_threshold()

def main():

    '''varlists_all = load_varlists()
    non_normalize_var = varlists_all['non_normalize']
    normalize_var     = varlists_all['normalize']
    varlists          = non_normalize_var + normalize_var
    df, varlists = data_loader_all('test.root', varlists)'''
    do_inference()

if __name__ == "__main__":
    main()

fpr, tpr, thresholds = roc_curve(data['target'], y_scores)
#print(tpr[:200])
#print(thresholds[:200])
#print(data['target'][:200])
#sys.exit()

