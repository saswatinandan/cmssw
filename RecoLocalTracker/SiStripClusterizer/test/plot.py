import uproot
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# -----------------------------
# 1. Load ROOT file and TTree
# -----------------------------
file = uproot.open("/home/users/nandan/tmp/CMSSW_14_1_5/src/RecoLocalTracker/SiStripClusterizer/test/test_study.root")  # replace with your ROOT file path
tree = file["sep19_2_2_dump_raw/tree"] 
# replace with your TTree name
#tree_bkg = file["sep19_2_2_dump_raw/bkgTree"]


# -----------------------------
# 2. Load specific branches into a DataFrame
# -----------------------------
#branches_to_load = ["noise_diff_adc_pone", "noise_diff_adc_ptwo", "noise_diff_adc_pthree", "noise_diff_adc_mone", "noise_diff_adc_mtwo", "noise_diff_adc_mthree", "noise_diff_max_adc",]  # replace with the branches you need
branches_to_load = ['pixeltrk_pt', 'dr_min_pixelTrk', 'dr_reco', 'dr_hlt', 'pixeltrk_eta', 'pixeltrk_qoverp', 'pixeltrk_dz', 'target']
df = tree.arrays(branches_to_load, library="pd")
#df_bkg = tree_bkg.arrays(branches_to_load, library="pd")


# -----------------------------
# 3. Create target variable
# target = 1 if a == 1 else 0
# -----------------------------
#df["target"] = ((df["low_pt_trk_cluster"] == 1) | (df["high_pt_trk_cluster"] == 1)).astype(int)

# -----------------------------
# 5. Plot histograms using Matplotlib
# -----------------------------
plt.figure(figsize=(8,6))

df_test = df[(df['target']==1) & (df['dr_hlt'] != 99) & (df['dr_reco'] != 99)]
plt.hist2d(df_test['dr_hlt'], df_test['dr_reco'], bins=400, range=[(0,50),(0,50)])
plt.colorbar()
plt.savefig(f'dr_hlt_vs_dr_reco.png', bbox_inches='tight')
plt.close('all')

for var in branches_to_load:
  if var == 'target': continue
  for label in [0,1]:
      #if label == 1:
      data = df[df['target']==label][var]
      #else:
      #    data = df_bkg[var]
      weights = np.ones_like(data) / len(data)
      plt.hist(data, weights=weights,bins=100, range=(0,10), histtype='step',label='sig' if label==1 else 'bkg')

  plt.xlabel(var)
  plt.ylabel("Normalized Count")
  plt.legend()
  plt.savefig(f'{var}.png', bbox_inches='tight')
  plt.close('all')
