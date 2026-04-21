import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def plot_var_dist(df, varlists):
   for var in varlists:
       minimum_value = min(df[var])
       maximum_value = max(df[var])
       bins = np.linspace(minimum_value, maximum_value, 100)
       for label in [0,1]:
           data = df[df['target'] == label][var]
           weights = np.ones_like(data) / len(data)
           plt.hist(data, weights=weights, histtype='step', \
            label='sig' if label==1 else 'bkg', bins=bins)
       plt.legend(loc='best', title=var)
       plt.ylim([0.0001, 10])
       plt.yscale('log')
       out_file = var + '_distribution.png'
       plt.savefig(out_file, bbox_inches='tight')
       plt.close('all')

def plot_loss_accuracy_true(fitted_model):
    fig1, ax = plt.subplots()
    epochs = range(1, len(fitted_model.history["loss"])+1)
    plt.figure(figsize=(9, 4))
    #plt.subplot(1, 2, 1)
    plt.plot(epochs, fitted_model.history["true_train_loss"], "o-", label="Training")
    plt.plot(epochs, fitted_model.history["true_val_loss"], "o-", label="Validation")
    plt.xlabel("Epochs"), plt.ylabel("Loss")
    plt.ylim(0.50*min(min(fitted_model.history["true_train_loss"]), min(fitted_model.history["true_val_loss"])), 1.05*max(max(fitted_model.history["true_train_loss"]), max(fitted_model.history["true_val_loss"])))
    plt.grid()
    plt.legend()

    '''plt.subplot(1, 2, 2)
    plt.plot(epochs, fitted_model.history["accuracy"], "o-", label="Training")
    plt.plot(epochs, fitted_model.history["val_accuracy"], "o-", label="Validation")
    plt.xlabel("Epochs"), plt.ylabel("Accuracy")
    plt.ylim(.95*min(min(fitted_model.history["true_train_accuracy"]), min(fitted_model.history["true_val_accuracy"])), 1.05*max(max(fitted_model.history["true_train_accuracy"]), max(fitted_model.history["true_val_accuracy"])))'''
    plt.legend(loc="best");
    loss_vs_epoch = ("loss_vs_epoch_true.png")
    plt.savefig(loss_vs_epoch)
    plt.close('all')

def plot_loss_accuracy(fitted_model):
    fig1, ax = plt.subplots()
    epochs = range(1, len(fitted_model.history["loss"])+1)
    plt.figure(figsize=(9, 4))
    plt.subplot(1, 2, 1)
    plt.plot(epochs, fitted_model.history["loss"], "o-", label="Training")
    plt.plot(epochs, fitted_model.history["val_loss"], "o-", label="Validation")
    plt.xlabel("Epochs"), plt.ylabel("Loss")
    plt.ylim(0.99*min(min(fitted_model.history["loss"]), min(fitted_model.history["val_loss"])), 1.01*max(max(fitted_model.history["loss"]), max(fitted_model.history["val_loss"])))
    plt.grid()
    plt.legend()
    
    plt.subplot(1, 2, 2)
    plt.plot(epochs, fitted_model.history["accuracy"], "o-", label="Training")
    plt.plot(epochs, fitted_model.history["val_accuracy"], "o-", label="Validation")
    plt.xlabel("Epochs"), plt.ylabel("Accuracy")
    plt.ylim(.99*min(min(fitted_model.history["accuracy"]), min(fitted_model.history["val_accuracy"])), 1.01*max(max(fitted_model.history["accuracy"]), max(fitted_model.history["val_accuracy"])))
    plt.grid()
    plt.legend(loc="best");

    loss_vs_epoch = ("loss_vs_epoch.png")
    plt.savefig(loss_vs_epoch)
    plt.close('all')

def plot_loss_mse(fitted_model):
    fig1, ax = plt.subplots()
    epochs = range(1, len(fitted_model.history["mean_squared_error"])+1)
    plt.figure(figsize=(9, 4))
    plt.subplot(1, 2, 1)
    plt.plot(epochs, fitted_model.history["mean_squared_error"], "o-", label="Training")
    plt.plot(epochs, fitted_model.history["val_mean_squared_error"], "o-", label="Validation")
    plt.xlabel("Epochs"), plt.ylabel("mean_squared_error")
    plt.ylim(0.99*min(min(fitted_model.history["mean_squared_error"]), min(fitted_model.history["val_mean_squared_error"])), 1.01*max(max(fitted_model.history["mean_squared_error"]), max(fitted_model.history["val_mean_squared_error"])))
    plt.grid()
    plt.legend()

    plt.grid()
    plt.legend(loc="best");

    loss_vs_epoch = ("loss_vs_epoch.png")
    plt.savefig(loss_vs_epoch)
    plt.close('all')

def plot_correlation(df, varlists):
    
    pearson_corr = df[varlists].corr(method='pearson')
    plt.figure(figsize=(8, 6))
    sns.heatmap(pearson_corr, annot=True, cmap="coolwarm", vmin=-1, vmax=1)
    plt.title("Pearson Correlation Matrix of Training Variables")
    plt.savefig('corelation.png')
    plt.close('all')
