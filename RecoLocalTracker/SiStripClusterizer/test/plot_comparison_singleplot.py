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
parser.add_argument("-o", dest="output", default='output', help="directory name where inputs are")
options = parser.parse_args()
bits = options.bits
widths = options.widths
avgCharges = options.avgCharges
output = options.output

x = np.array(bits)
x = np.sort(x)
widths = np.array(widths)
avgCharge = np.array(avgCharges)

colors = ['r', 'b', 'g', 'c', 'm', 'y', 'k', 'w']

def readfile(input_file):

  with open(input_file, 'r') as f:
     lines = f.readlines()
  return lines

def update_list(dirname, bary_bit, chrg_bit, rawtype, sizes, unmatched, texts, ver):

  input = f'/scratch/nandan/{dirname}_barycenter_{bary_bit}bit_width_8bit_avgCharge_{chrg_bit}bit/'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
         sizes.append(float(line.split(' ')[-1]))

  input_file = os.path.join(input, 'object.log')
  lines = readfile(input_file)
  for idx, line in enumerate(lines):
      if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            unmatched.append(val)
  texts.append((f'{ver}_{bary_bit}', f'{ver}_{chrg_bit}'))
  print(texts) 
def draw(x_vals, y_vals, texts, obj, rawtype):

  fig = plt.figure(figsize=(8,6))
  ax = fig.add_subplot(111)
  plt.scatter(x_vals, y_vals)
  sorted_xvals = sorted(x_vals)
  plt.text(sorted_xvals[len(x_vals)-3], max(y_vals), '(barycenter,avgCharge)')
  plt.title(f'size vs unmatched {obj}', fontsize=20)
  plt.xlabel('size', fontsize=20)
  plt.ylabel('unmatched in %', fontsize=20)
  for i, text in enumerate(texts):
    if isinstance(text[0], str) and 'def' in text[0]:
      ax.text(x_vals[i], y_vals[i], text, color='red')
    elif isinstance(text[0], str) and 'v2' in text[0]:
      ax.text(x_vals[i], y_vals[i], text, color='green')
    elif isinstance(text[0], str) and 'v1' in text[0]:
      ax.text(x_vals[i], y_vals[i], text, color='blue')
    else:
      ax.text(x_vals[i], y_vals[i], text)
  ax.grid(True)
  plt.savefig(f'singleplot_{obj}_{rawtype}.png')
  plt.close('all')

def build_array(obj, rawtype):

  texts = []
  sizes = []
  unmatched = []

  for avgCharge in avgCharges:
    for bit in x:
      texts.append((bit, avgCharge))
      input = f'/scratch/nandan/{output}_barycenter_{bit}bit_width_8bit_avgCharge_{avgCharge}bit'
      input_file = os.path.join(input, 'size.log')
      lines = readfile(input_file)

      for idx, line in enumerate(lines):

         if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
             sizes.append(float(line.split(' ')[-1]))
      
      input_file = os.path.join(input, 'object.log')
      lines = readfile(input_file)
      for idx, line in enumerate(lines):
         if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            unmatched.append(val)

  '''input = f'/scratch/nandan/default_10_compression_LZMA_barycenter_16bit_width_8bit_avgCharge_8bit'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
         sizes.append(float(line.split(' ')[-1]))

  input_file = os.path.join(input, 'object.log')
  lines = readfile(input_file)
  for idx, line in enumerate(lines):
      if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            unmatched.append(val)
  texts.append(('def_16', 'def_8'))
  '''
  update_list('default_10_compression_LZMA', 16, 8, rawtype, sizes, unmatched, texts, 'def')
  update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 15, 8, rawtype, sizes, unmatched, texts, 'v2')
  #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 15, 5, rawtype, sizes, unmatched, texts, 'v2')
  #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 15, 4, rawtype, sizes, unmatched, texts, 'v2')
  #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 5, rawtype, sizes, unmatched, texts, 'v2')
  #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 4, rawtype, sizes, unmatched, texts, 'v2')
  update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 8, rawtype, sizes, unmatched, texts, 'v2')
  update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 7, rawtype, sizes, unmatched, texts, 'v2')
  update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 6, rawtype, sizes, unmatched, texts, 'v2')
  update_list('remove_beginindices_v1_compression_LZMA', 14, 8, rawtype, sizes, unmatched, texts, 'v1')
  update_list('keep_diff_beginindices_compression_LZMA', 14, 8, rawtype, sizes, unmatched, texts, 'v1.1')
  update_list('keep_diff_beginindices_compression_LZMA', 14, 5, rawtype, sizes, unmatched, texts, 'v1.1')
  update_list('keep_diff_beginindices_compression_LZMA', 14, 7, rawtype, sizes, unmatched, texts, 'v1.1')
  update_list('keep_diff_beginindices_compression_LZMA', 14, 6, rawtype, sizes, unmatched, texts, 'v1.1')
  update_list('keep_diff_beginindices_compression_LZMA', 13, 5, rawtype, sizes, unmatched, texts, 'v1.1')
  update_list('keep_diff_beginindices_compression_LZMA', 13, 7, rawtype, sizes, unmatched, texts, 'v1.1')
  update_list('keep_diff_beginindices_compression_LZMA', 13, 6, rawtype, sizes, unmatched, texts, 'v1.1')
  print(texts)
  
  '''input = f'/scratch/nandan/flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA_barycenter_15bit_width_8bit_avgCharge_8bit/'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
         sizes.append(float(line.split(' ')[-1]))

  input_file = os.path.join(input, 'object.log')
  lines = readfile(input_file)
  for idx, line in enumerate(lines):
      if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            unmatched.append(val)
  texts.append(('v2_15', 'v2_8'))

  input = f'/scratch/nandan/remove_beginindices_v1_compression_LZMA_barycenter_14bit_width_8bit_avgCharge_8bit/'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
         sizes.append(float(line.split(' ')[-1]))

  input_file = os.path.join(input, 'object.log')
  lines = readfile(input_file)
  for idx, line in enumerate(lines):
      if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            unmatched.append(val)
  texts.append(('v1_14', 'v1_8'))

  input = f'/scratch/nandan/flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA_barycenter_15bit_width_8bit_avgCharge_5bit/'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
         sizes.append(float(line.split(' ')[-1]))

  input_file = os.path.join(input, 'object.log')
  lines = readfile(input_file)
  for idx, line in enumerate(lines):
      if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            unmatched.append(val)
  texts.append(('v2_15', 'v2_5'))
  '''
  draw(sizes, unmatched, texts, obj, rawtype)

for raw in ['raw', 'rawp']:
  for obj in ['tracks_lowpt', 'tracks_highpt', 'jet']:
     build_array(obj, raw)
