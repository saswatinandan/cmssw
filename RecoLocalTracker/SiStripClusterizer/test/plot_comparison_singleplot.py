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
parser.add_argument("-v", dest="version", default='', help="which version you want to compare")
parser.add_argument("-e", action='store_true', dest="events", default=False, help="want to see # of events")

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

def update_list(dirname, bary_bit, chrg_bit, rawtype, sizes, yvals, texts, ver, events):

  input = f'/scratch/nandan/{dirname}_barycenter_{bary_bit}bit_width_8bit_avgCharge_{chrg_bit}bit/'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__HLTX' in line:
         sizes[ver].append(float(line.split(' ')[-1]))

  input_file = os.path.join(input, 'object.log' if not events else 'object_study.root')
  if not events:
   lines = readfile(input_file)
   for idx, line in enumerate(lines):
      if f'not matched {obj}' in line and  f'{rawtype} ' in line:
            val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
            yvals[ver].append(val)
  else:
    f = TFile(input_file, 'r')
    yvals[ver].append(f.Get(f'{rawtype}_trk_cutflow_z4').GetBinContent(1,1))
    
  
  texts[ver].append((f'{bary_bit}', f'{chrg_bit}'))

def draw(x_vals, y_vals, texts, obj, rawtype):

  fig = plt.figure(figsize=(8,6))
  ax = fig.add_subplot(111)
  colors = ['r', 'b', 'g', 'c', 'm', 'y', 'k', 'w']
  for idx, key in enumerate(texts.keys()):
    plt.scatter(x_vals[key], y_vals[key], color=colors[idx], label=key)
    for i, text in enumerate(texts[key]):
      ax.text(x_vals[key][i], y_vals[key][i], text, color=colors[idx])
  plt.title(f'size vs {obj}', fontsize=20)
  plt.xlabel('size', fontsize=20)
  plt.ylabel(f'{obj}', fontsize=20)
  plt.legend()
  ax.grid(True)
  plt.savefig(f'singleplot_{obj}_{rawtype}.png')
  plt.close('all')

def build_array(obj, rawtype):

  texts = {}
  sizes = {}
  yvals = {}

  compare = 'cutflow'

  texts['def'] = []
  yvals['def'] = []
  sizes['def'] = []

  for avgCharge in avgCharges:
    for bit in x:
      update_list(output, bit, avgCharge, rawtype, sizes, yvals, texts, 'def', options.events)
  
  if options.events:
   texts['def_wocharge_cut'] = []
   yvals['def_wocharge_cut'] = []
   sizes['def_wocharge_cut'] = []

   for avgCharge in avgCharges:
    for bit in x:
      update_list('test_wochargecut_compression_LZMA', bit, avgCharge, rawtype, sizes, yvals, texts, 'def_wocharge_cut', options.events)

  texts['def_HI'] = []
  yvals['def_HI'] = []
  sizes['def_HI'] = []
  
  update_list('default_10_compression_LZMA', 16, 8, rawtype, sizes, yvals, texts, 'def_HI', options.events)
  if options.events:
    texts['def_HI_wocharge_cut'] = []
    yvals['def_HI_wocharge_cut'] = []
    sizes['def_HI_wocharge_cut'] = []
    update_list('default_10_wochargecut_compression_LZMA', 16, 8, rawtype, sizes, yvals, texts, 'def_HI_wocharge_cut', options.events)

  if options.version == 'v2':
    update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 15, 8, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 15, 5, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 15, 4, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 5, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 4, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 8, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 7, rawtype, sizes, unmatched, texts, 'v2')
    #update_list('flatdistribution_comparison_with_16bit_keep_detid_diff_compression_LZMA', 14, 6, rawtype, sizes, unmatched, texts, 'v2')
  elif options.version == 'v1':
    update_list('remove_beginindices_v1_compression_LZMA', 14, 8, rawtype, sizes, unmatched, texts, 'v1')
  elif options.version == 'v1.1':
    update_list('keep_diff_beginindices_compression_LZMA', 14, 8, rawtype, sizes, unmatched, texts, 'v1.1')
    update_list('keep_diff_beginindices_compression_LZMA', 14, 5, rawtype, sizes, unmatched, texts, 'v1.1')
    update_list('keep_diff_beginindices_compression_LZMA', 14, 7, rawtype, sizes, unmatched, texts, 'v1.1')
    update_list('keep_diff_beginindices_compression_LZMA', 14, 6, rawtype, sizes, unmatched, texts, 'v1.1')
    update_list('keep_diff_beginindices_compression_LZMA', 13, 5, rawtype, sizes, unmatched, texts, 'v1.1')
    update_list('keep_diff_beginindices_compression_LZMA', 13, 7, rawtype, sizes, unmatched, texts, 'v1.1')
    update_list('keep_diff_beginindices_compression_LZMA', 13, 6, rawtype, sizes, unmatched, texts, 'v1.1')
  
  draw(sizes, yvals, texts, f'unmatched {obj} in %, rawtype)

if not options.events:
  for raw in ['raw', 'rawp']:
     for obj in ['tracks_lowpt', 'tracks_highpt', 'jet']:
        build_array(obj, raw)
else:

  texts = {}
  sizes = {}
  yvals = {}

  texts['raw'] = []
  yvals['raw'] = []
  sizes['raw'] = []

  update_list('test_compression_LZMA', 16, 8, 'raw', sizes, yvals, texts, 'raw', 1)

  texts['wochargecut_raw'] = []
  yvals['wochargecut_raw'] = []
  sizes['wochargecut_raw'] = []

  update_list('test_wochargecut_compression_LZMA', 16, 8, 'raw', sizes, yvals, texts, 'wochargecut_raw', 1)

  texts['def_rawp'] = []
  yvals['def_rawp'] = []
  sizes['def_rawp'] = []

  update_list('test_compression_LZMA', 16, 8, 'rawp', sizes, yvals, texts, 'def_rawp', options.events)

  texts['def_wochargecut_rawp'] = []
  yvals['def_wochargecut_rawp'] = []
  sizes['def_wochargecut_rawp'] = []

  update_list('test_wochargecut_compression_LZMA', 16, 8, 'rawp', sizes, yvals, texts, 'def_wochargecut_rawp', options.events)

  texts['def_HI_rawp'] = []
  yvals['def_HI_rawp'] = []
  sizes['def_HI_rawp'] = []

  update_list('default_10_compression_LZMA', 16, 8, 'rawp', sizes, yvals, texts, 'def_HI_rawp', options.events)

  texts['def_HI_wochargecut_rawp'] = []
  yvals['def_HI_wochargecut_rawp'] = []
  sizes['def_HI_wochargecut_rawp'] = []

  update_list('default_10_wochargcut_compression_LZMA', 16, 8, 'rawp', sizes, yvals, texts, 'def_HI_wochargecut_rawp', options.events)

  draw(sizes, yvals, texts, '# of tracks', 'raw_rawp')
