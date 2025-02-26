import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import argparse
from ROOT import TFile, TH1

parser = argparse.ArgumentParser()
parser.add_argument("-b", type=int, dest="bits", nargs='+', default=[], help="bit to be studied")
options = parser.parse_args()
bits = options.bits

x = np.array(bits)
x = np.sort(x)

def draw_plot(x, y_rawp_1,
              x_title, y_title,
              title, filename,
              y_raw_1=np.array([]),
              y_raw_2=np.array([]),
              y_rawp_2=np.array([]),
              title_rawp_1 = 'rawp',
              title_raw_2  = '',
              title_rawp_2 = ''
             ):

   fig = plt.figure(figsize=(8,6))
   ax = fig.add_subplot(111)

   plt.scatter(x,y_rawp_1,color='r',label=title_rawp_1)

   if np.any(y_raw_1):
       plt.scatter(x,y_raw_1,label='raw: without charge cut')
   if np.any(y_raw_2):
       plt.scatter(x,y_raw_2,label=title_raw_2)
   if np.any(y_rawp_2):
       plt.scatter(x,y_rawp_2,label=title_rawp_2)

   plt.title(title, fontsize=20)
   plt.xlabel(x_title, fontsize=20)
   plt.ylabel(y_title, fontsize=20)
   plt.legend()
   plt.savefig(filename)
   plt.close('all')

def build_array(dirname, obj):

    y_raw  = []
    y_rawp = []
    y_total_before_cut_raw =  []
    y_total_before_cut_rawp = []
    y_total_after_cut_raw =  []
    y_total_after_cut_rawp = []

    for bit in x:

      input = os.path.join(os.getcwd(), f'{dirname}_{bit}bit')
      cutflow_file = TFile(os.path.join(input, 'object_study.root'), 'r')

      if obj in ['track', 'jet']:

         for raw_type in ['raw', 'rawp']:

            cutflow = cutflow_file.Get(f'{raw_type}_{obj}_cutflow')
            before_cut = cutflow.GetBinContent(1)
            after_cut  = cutflow.GetBinContent(cutflow.GetNbinsX())

            if raw_type == 'raw':
               y_total_before_cut_raw.append(before_cut)
               y_total_after_cut_raw.append(after_cut)
            else:
               y_total_before_cut_rawp.append(before_cut)
               y_total_after_cut_rawp.append(after_cut)

      if obj == 'size':
         input_file = os.path.join(input, 'size.log')
      elif obj in ['track', 'jet']:
         input_file = os.path.join(input, 'object.log')
      else:
         input_file = os.path.join(input, 'cluster.log')

      with open(input_file, 'r') as f:
         lines = f.readlines()

      for idx, line in enumerate(lines):
        if obj == 'size' and 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
          y_rawp.append(float(line.split(' ')[-1]))
        else:
             for raw_type in ['raw', 'rawp']:

                if f'not matched {obj}' in line and  f'{raw_type} ' in line:
                   if raw_type == 'raw':
                      y_raw.append(float(line.split('in raw')[-1].split('%')[0]))
                   else:
                       y_rawp.append(float(line.split('in rawp')[-1].split('%')[0]))
                elif obj == 'cluster' and f'total {obj} in {raw_type}:' in line:
                     if raw_type == 'raw':
                           y_total_after_cut_raw.append(int(line.split(f'{raw_type}:')[-1]))
                     else:
                           y_total_after_cut_rawp.append(int(line.split(f'{raw_type}:')[-1]))
    y_raw   = np.array(y_raw)
    y_rawp  = np.array(y_rawp)
    y_total_before_cut_raw = np.array(y_total_before_cut_raw)
    y_total_after_cut_raw = np.array(y_total_after_cut_raw)

    return y_raw, y_rawp, y_total_before_cut_raw, y_total_after_cut_raw, y_total_before_cut_rawp, y_total_after_cut_rawp 
            
for obj in ['size', 'cluster', 'track', 'jet']:

    y_raw_1, y_rawp_1, y_total_before_cut_raw_1, y_total_after_cut_raw_1, y_total_before_cut_rawp_1, y_total_after_cut_rawp_1 = build_array('output_test', obj)
    y_raw_2, y_rawp_2, y_total_before_cut_raw_2, y_total_after_cut_raw_2, y_total_before_cut_rawp_2, y_total_after_cut_rawp_2 = build_array('output_charge', obj)

    if obj == 'size':
      y_title = 'Strip detector size (Bytes/Event)'
    else:
      y_title = f'unmatched {obj} in %'

    draw_plot(x, y_rawp_1,
             '#bits for barycenter', y_title,
             obj, f'{obj}.pdf',
             y_raw_1,
             y_raw_2,
             y_rawp_2,
             'rawp: without charge cut',
             'raw: with charge cut',
             'rawp: with charge cut'
             )

    if obj != 'size':
        if obj != 'cluster': 
            draw_plot(x, y_total_before_cut_rawp_1,
              'bit', f'total # of {obj}',
              f'total # of {obj} before selection', f'size_before_cut_{obj}.pdf',
              y_total_before_cut_raw_1,
              y_total_before_cut_raw_2,
              y_total_before_cut_rawp_2,
              'rawp: without charge cut',
              'raw: with charge cut',
              'rawp: with charge cut'
             ) 
        if obj == 'cluster':
           title = f'total # of {obj}'
        else:
            title = f'total # of {obj} after selection'
        draw_plot(x, y_total_after_cut_rawp_1,
             'bit', f'total # of {obj}',
             title, f'size_after_cut_{obj}.pdf',
             y_total_after_cut_raw_1,
             y_total_after_cut_raw_2,
             y_total_after_cut_rawp_2,
             'rawp: without charge cut',
             'raw: with charge cut',
             'rawp: with charge cut'
             )
