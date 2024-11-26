import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import argparse
from ROOT import TFile, TH1

parser = argparse.ArgumentParser()
parser.add_argument("-b", type=int, dest="bits", nargs='+', default=[], help="bit to be studied for barycenter")
parser.add_argument("-w", type=int, dest="widths", nargs='+', default=[], help="bit to be studied for width")
parser.add_argument("-a", type=int, dest="avgCharges", nargs='+', default=[], help="bit to be studied for avgCharge")
options = parser.parse_args()
bits = options.bits
widths = options.widths
avgCharges = options.avgCharges

x = np.array(bits)
x = np.sort(x)
widths = np.sort(np.array(widths))
avgCharge = np.sort(np.array(avgCharges))

colors = ['r', 'b', 'g', 'c', 'm', 'y', 'k', 'w']

def draw_plot(x, x_title, y_title,
              title, filename,
              dict_, obj_type
             ):

   fig = plt.figure(figsize=(8,6))
   ax = fig.add_subplot(111)

   for idx, key in enumerate(dict_.keys()):
       if np.any(dict_[key][f'raw_{obj_type}']):
         if idx == 0:
             plt.scatter(x,dict_[key][f'raw_{obj_type}'],color=colors[idx], alpha=0.40, label=f'raw:default 8bit')
         elif 'cut' not in obj_type:
             plt.scatter(x,dict_[key][f'raw_{obj_type}'],color=colors[idx], alpha=0.40, label=f'raw:default 8bit')
       plt.scatter(x,dict_[key][f'rawp_{obj_type}'],color=colors[idx], marker=(5,1), label=f'rawp:{key}')

   plt.title(title, fontsize=20)
   plt.xlabel(x_title, fontsize=20)
   plt.ylabel(y_title, fontsize=20)
   plt.legend()
   plt.savefig(filename)
   plt.close('all')

def build_array(dirname, width, obj):

    ret = {}

    for avgCharge in avgCharges:
       avgCharge_key = f'avgCharge_{str(avgCharge)}'
       ret[avgCharge_key] = {}
       for bit in x:
          input = f'/scratch/nandan/{dirname}_barycenter_{bit}bit_width_{width}bit_avgCharge_{avgCharge}bit'
          cutflow_file = TFile(os.path.join(input, 'object_study.root'), 'r')

          if obj in ['tracks_lowpt', 'tracks_highpt', 'jet']:

             for raw_type in ['raw', 'rawp']:

                cutflow = cutflow_file.Get(f'{raw_type}_{obj}_cutflow' if 'track' not in obj
                                           else f'{raw_type}_track_cutflow'
                                          )
                if f'{raw_type}_before_cut' not in ret[avgCharge_key].keys():
                   ret[avgCharge_key][f'{raw_type}_before_cut'] = []
                   ret[avgCharge_key][f'{raw_type}_after_cut'] = []
                ret[avgCharge_key][f'{raw_type}_before_cut'].append(cutflow.GetBinContent(1))
                ret[avgCharge_key][f'{raw_type}_after_cut'].append(cutflow.GetBinContent(cutflow.GetNbinsX()))

          if obj == 'size':
             input_file = os.path.join(input, 'size.log')
          elif obj in ['tracks_lowpt', 'tracks_highpt', 'jet']:
             input_file = os.path.join(input, 'object.log')
          else:
             input_file = os.path.join(input, 'cluster.log')

          with open(input_file, 'r') as f:
             lines = f.readlines()

          for idx, line in enumerate(lines):

             if 'rawp_y' not in ret[avgCharge_key].keys():
                ret[avgCharge_key]['rawp_y'] = []
                ret[avgCharge_key]['raw_y'] = []
             if obj == 'size' and 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
                ret[avgCharge_key]['rawp_y'].append(float(line.split(' ')[-1]))
             else:
                for raw_type in ['raw', 'rawp']:
                   if f'not matched {obj}' in line and  f'{raw_type} ' in line:
                      val = float(line.split(f'in {raw_type}')[-1].split('%')[0])
                      ret[avgCharge_key][f'{raw_type}_y'].append(val)
                   elif obj == 'cluster' and f'total {obj} in {raw_type}:' in line:
                      if f'{raw_type}_after_cut' not in ret[avgCharge_key].keys():
                          ret[avgCharge_key][f'{raw_type}_after_cut'] = []
                      ret[avgCharge_key][f'{raw_type}_after_cut'].append(int(line.split(f'{raw_type}:')[-1]))
    return ret  

for width in widths:   
  for obj in ['size', 'cluster', 'tracks_lowpt', 'tracks_highpt', 'jet']:

    ret1 = build_array('output', width, obj)

    if obj == 'size':
      y_title = 'Average Compressed Size (Bytes/Event) for hltSiStripClusters2ApproxClusters'
    else:
      y_title = f'unmatched {obj} in %'

    draw_plot(x,'barycenter bit', y_title,
             obj, f'width_{width}bit_{obj}.png',
             ret1, 'y')
    if obj != 'size':
        if obj != 'cluster':
            draw_plot(x,'barycenter bit', f'total # of {obj}',
              f'total # of {obj} before selection', f'width_{width}bit_size_before_cut_{obj}.png',
              ret1, 'before_cut')
        if obj == 'cluster':
           title = f'total # of {obj}'
        else:
            title = f'total # of {obj} after selection'
        draw_plot(x,'barycenter bit', f'total # of {obj}',
              f'total # of {obj} after selection', f'width_{width}bit_size_after_cut_{obj}.png',
              ret1, 'after_cut')
