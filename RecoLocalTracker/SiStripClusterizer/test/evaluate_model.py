import tensorflow as tf
from sklearn.metrics import roc_curve, auc
from create_varlists import save_json_withdict
import matplotlib.pyplot as plt
import numpy as np
import onnxruntime as ort
from matplotlib.colors import LogNorm

def onnx_predict(X, session,output_name,input_name):
    """Keras-like predict().ravel()"""
    X = X.to_numpy(dtype=np.float32)
    y_pred = session.run([output_name], {input_name: X})[0]
    return y_pred.ravel()

def evaluate_model_onnx(X_train, X_test, y_train, y_test, optimizer=False):

    session = ort.InferenceSession("nn_model.onnx")
    input_name  = session.get_inputs()[0].name
    output_name = session.get_outputs()[0].name
    # Predictions (ONNX)
    y_pred_proba = onnx_predict(X_test, session, output_name, input_name)
    train_preds  = onnx_predict(X_train, session, output_name, input_name)

    # ROC / AUC
    fpr, tpr, threshold = roc_curve(y_test, y_pred_proba)
    roc_auc = auc(fpr, tpr)

    fpr_t, tpr_t, threshold_t = roc_curve(y_train, train_preds)
    roc_auc_t = auc(fpr_t, tpr_t)

    if optimizer:
        return calculate_d_score(roc_auc_t, roc_auc)
    else:
        find_threshold(tpr, fpr, threshold)

    # Plot ROC
    plt.figure(figsize=(8, 6))
    plt.plot(fpr, tpr, label=f"ROC test (AUC = {roc_auc:.4f})")
    plt.plot(fpr_t, tpr_t, "k--", label=f"ROC train (AUC = {roc_auc_t:.4f})")
    plt.plot([0, 1], [0, 1], "k--")
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.title("ROC Curve")
    plt.legend()
    plt.grid(True)
    plt.savefig("roc.png")
    plt.close()

def evaluate_model_from_generator(model, train_ds, test_ds, optimizer=False):
    
    def features_only(ds):
        return ds.map(lambda x, y, w: tf.cast(x, tf.float32))

    # --- Test predictions ---
    y_test_pred = model.predict(features_only(test_ds), verbose=0).ravel()

    # Collect true labels from the dataset
    y_test_true = np.concatenate([y.numpy().ravel() for _, y, _ in test_ds])

    # Compute ROC AUC
    fpr, tpr, thresholds = roc_curve(y_test_true, y_test_pred)
    if not optimizer:
        find_threshold(tpr, fpr, thresholds)
    roc_auc = auc(fpr, tpr)
    print(f"Test ROC AUC: {roc_auc:.4f}")

    # --- Train predictions ---
    y_train_pred = model.predict(features_only(train_ds), verbose=0).ravel()
    y_train_true = np.concatenate([y.numpy().ravel() for _, y, _ in train_ds])
    fpr_t, tpr_t, thresholds_t = roc_curve(y_train_true, y_train_pred)
    roc_auc_t = auc(fpr_t, tpr_t)
    print(f"Train ROC AUC: {roc_auc_t:.4f}")

    if optimizer:
        return calculate_d_score(roc_auc_t, roc_auc)
    else:
        #find_threshold(tpr, fpr, thresholds)

        plt.figure(figsize=(8,6))
        plt.plot(fpr, tpr, label=f'ROC test (AUC = {roc_auc:.4f})')
        plt.plot(fpr_t, tpr_t, 'k--', label=f'ROC train (AUC = {roc_auc_t:.4f})')
        plt.plot([0, 1], [0, 1], 'k--')
        plt.xlabel("False Positive Rate")
        plt.ylabel("True Positive Rate")
        plt.title("ROC Curve")
        plt.legend()
        plt.grid(True)
        plt.savefig('roc.png')

        del fpr, tpr, thresholds, fpr_t, tpr_t, thresholds_t
        # 1. Run inference
        # 2. Create masks for signal/background
        train_sig = y_train_true == 1
        train_bkg = y_train_true == 0
        test_sig = y_test_true == 1
        test_bkg = y_test_true == 0

        # 3. Plot predictions
        plt.figure(figsize=(10, 6))

        # Training predictions
        weights_sig = np.ones_like(y_train_pred[train_sig])/len(y_train_pred[train_sig])
        weights_bkg = np.ones_like(y_train_pred[train_bkg])/len(y_train_pred[train_bkg])
        #plt.hist(y_train_pred[train_sig], bins=50, alpha=0.5, weights=weights_sig, label='Train Signal', color='red')
        plt.hist(y_train_pred[train_bkg], bins=50, alpha=0.5, weights=weights_bkg, label='Train Background', color='blue')

        # Testing predictions
        weights_sig = np.ones_like(y_test_pred[test_sig])/len(y_test_pred[test_sig])
        weights_bkg = np.ones_like(y_test_pred[test_bkg])/len(y_test_pred[test_bkg])
        #plt.hist(y_test_pred[test_sig], bins=50, alpha=0.5, weights=weights_sig, label='Test Signal', color='orange', histtype='step')
        plt.hist(y_test_pred[test_bkg], bins=50, alpha=0.5, weights=weights_bkg, label='Test Background', color='cyan', histtype='step')

        # Labels and legend
        plt.xlabel('Model Output')
        plt.ylabel('Normalized Count')
        plt.title('Model Predictions for Train and Test Data')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig('inference.png')

def evaluate_model_regression(model, X_train, X_test, y_train, y_test, optimizer=False):
        y_pred = model.predict(X_test.astype('float32')).ravel()
        train_pred = model.predict(X_train.astype('float32')).flatten()

        fig, ax = plt.subplots(figsize=(6,5))

        # 2D histogram
        h = ax.hist2d(
           y_test,
           y_pred,
           bins=400,
           range=[[0, 20], [0, 20]],
           cmap='viridis',
           norm=LogNorm()
        )

        ax.set_xlabel('dr_reco')
        ax.set_ylabel('dr_hlt')
        # Add colorbar linked to the same axes
        cbar = fig.colorbar(h[3], ax=ax)
        cbar.set_label('Counts (log scale)')

        plt.savefig('evaluate_regression_test.png')

        fig, ax = plt.subplots(figsize=(6,5))

        # 2D histogram
        h = ax.hist2d(
           y_train,
           train_pred,
           bins=400,
           range=[[0, 20], [0, 20]],
           cmap='viridis',
           norm=LogNorm()
        )

        ax.set_xlabel('dr_reco')
        ax.set_ylabel('dr_hlt')
        # Add colorbar linked to the same axes
        cbar = fig.colorbar(h[3], ax=ax)
        cbar.set_label('Counts (log scale)')

        plt.savefig('evaluate_regression_train.png')

        # Create figure and axis explicitly
        fig, ax = plt.subplots(figsize=(6,5))

        # 2D histogram
        h = ax.hist2d(
           y_train,
           X_train['hlttrk_dr_min'],
           bins=400,
           range=[[0, 20], [0, 20]],
           cmap='viridis',
           norm=LogNorm()
        )

        ax.set_xlabel('dr_reco')
        ax.set_ylabel('dr_hlt')
        # Add colorbar linked to the same axes
        cbar = fig.colorbar(h[3], ax=ax)
        cbar.set_label('Counts (log scale)')

        plt.savefig('evaluate_train.png')

        fig, ax = plt.subplots(figsize=(6,5))

        # 2D histogram
        h = ax.hist2d(
           y_test,
           X_test['hlttrk_dr_min'],
           bins=400,
           range=[[0, 20], [0, 20]],
           cmap='viridis',
           norm=LogNorm()
        )

        ax.set_xlabel('dr_reco')
        ax.set_ylabel('dr_hlt')
        # Add colorbar linked to the same axes
        cbar = fig.colorbar(h[3], ax=ax)
        cbar.set_label('Counts (log scale)')

        plt.savefig('evaluate_test.png')


def evaluate_model(model, X_train, X_test, y_train, y_test, optimizer=False):
        y_pred_proba = model.predict(X_test.astype('float32')).ravel()
        train_preds = model.predict(X_train.astype('float32')).flatten()
        # --- ROC ---
        fpr, tpr, threshold = roc_curve(y_test, y_pred_proba)
        roc_auc = auc(fpr, tpr)

        fpr_t, tpr_t, threshold_t = roc_curve(y_train, train_preds)
        roc_auc_t = auc(fpr_t, tpr_t)

        if optimizer:
            return calculate_d_score(roc_auc_t, roc_auc)
        else:
            find_threshold(tpr, fpr, threshold)

        plt.figure(figsize=(8,6))
        plt.plot(fpr, tpr, label=f'ROC test (AUC = {roc_auc:.4f})')
        plt.plot(fpr_t, tpr_t, 'k--', label=f'ROC train (AUC = {roc_auc_t:.4f})')
        plt.plot([0, 1], [0, 1], 'k--')
        plt.xlabel("False Positive Rate")
        plt.ylabel("True Positive Rate")
        plt.title("ROC Curve")
        plt.legend()
        plt.grid(True)
        plt.savefig('roc.png')

        del fpr, tpr, threshold, fpr_t, tpr_t, threshold_t
        # 1. Run inference
        # 2. Create masks for signal/background
        train_sig = y_train == 1
        train_bkg = y_train == 0
        test_sig = y_test == 1
        test_bkg = y_test == 0

        # 3. Plot predictions
        plt.figure(figsize=(10, 6))

        # Training predictions
        weights_sig = np.ones_like(train_preds[train_sig])/len(train_preds[train_sig])
        weights_bkg = np.ones_like(train_preds[train_bkg])/len(train_preds[train_bkg])
        plt.hist(train_preds[train_sig], bins=50, alpha=0.5, weights=weights_sig, label='Train Signal', color='red')
        plt.hist(train_preds[train_bkg], bins=50, alpha=0.5, weights=weights_bkg, label='Train Background', color='blue')

        # Testing predictions
        weights_sig = np.ones_like(y_pred_proba[test_sig])/len(y_pred_proba[test_sig])
        weights_bkg = np.ones_like(y_pred_proba[test_bkg])/len(y_pred_proba[test_bkg])
        plt.hist(y_pred_proba[test_sig], bins=50, alpha=0.5, weights=weights_sig, label='Test Signal', color='orange', histtype='step')
        plt.hist(y_pred_proba[test_bkg], bins=50, alpha=0.5, weights=weights_bkg, label='Test Background', color='cyan', histtype='step')

        # Labels and legend
        plt.xlabel('Model Output')
        plt.ylabel('Normalized Count')
        plt.title('Model Predictions for Train and Test Data')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig('inference.png')


def find_threshold(tpr, fpr, thresholds):

    target_tprs = [0.99, 0.98, 0.97, 0.94, 0.95, 0.90, 0.85, 0.80]
    threshold = {}

    for target_tpr in target_tprs:
      idx = np.argmin(np.abs(tpr - target_tpr))

      # Threshold corresponding to that TPR
      chosen_threshold = thresholds[idx]
      print(f'for {target_tpr}')
      print("Closest TPR:", tpr[idx])
      print("Corresponding FPR:", fpr[idx])
      print("Score threshold:", chosen_threshold)
      threshold[target_tpr] = {'TPR': float(tpr[idx]), 'FPR': float(fpr[idx]), 'threshold': float(chosen_threshold)}

    save_json_withdict(threshold, 'threshold.json')

def calculate_d_score(train_score, test_score):
    print('calculating dscores')
    difference = max(0, train_score - test_score)
    fr_difference = difference / (1 - test_score)
    d_score = test_score - (0.30 * fr_difference)
    print('train_score ', train_score, '\t', test_score, '\t', d_score)
    return d_score

def calculate_l_score(train_loss, test_loss):
    print('train_loss ', train_loss, '\t', test_loss)
    difference = max(0, test_loss - train_loss)
    fr_difference = difference / (0.0000001 + test_loss)
    print('test_loss ', test_loss + (0.30 * fr_difference), '\t', difference, '\t', fr_difference)
    return test_loss + (0.30 * fr_difference) 
