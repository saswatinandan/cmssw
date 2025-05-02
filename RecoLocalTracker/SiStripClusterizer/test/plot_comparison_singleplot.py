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

colors = ['b', 'r', 'g', 'c', 'm', 'y', 'k', 'w']

def readfile(input_file):

  with open(input_file, 'r') as f:
     lines = f.readlines()
  return lines

def update_list(dirname, bary_bit, chrg_bit, rawtype, sizes, yvals, texts, ver, events=0, yerrs={}):

  input = f'/scratch/nandan/{dirname}_barycenter_{bary_bit}bit_width_8bit_avgCharge_{chrg_bit}bit/'
  input_file = os.path.join(input, 'size.log')
  lines = readfile(input_file)

  for idx, line in enumerate(lines):

     if 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__reHLT' in line:
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
    yvals[ver].append(f.Get(f'{rawtype}_trk_cutflow').GetBinContent(1,1))
    yerrs[ver].append(f.Get(f'{rawtype}_trk_cutflow').GetBinError(1,1))

  #print(sizes)
  #print(yvals)
  texts[ver].append((f'{bary_bit}', f'{chrg_bit}'))

def draw(x_vals, y_vals, texts, ytitle, obj, rawtype):

  fig = plt.figure(figsize=(8,7))
  ax = fig.add_subplot(111)
  plt.title(r'CMS Preliminary   2024 PbPb Data $\sqrt{s_{NN}} = 5.36$ TeV', loc='left', fontsize=15)
  for idx, key in enumerate(texts.keys()):
    plt.scatter(x_vals[key], y_vals[key], color=colors[idx], label=key)
    for i, text in enumerate(texts[key]):
      ax.text(x_vals[key][i], y_vals[key][i], text, fontsize=12)#, color=colors[idx])
  #plt.title(f'size vs {obj}', fontsize=20)
  #plt.margins(x=0.80)
  plt.xlabel('size of approx cluster in Byte', fontsize=20, labelpad=15)
  plt.ylabel(ytitle, fontsize=20, labelpad=15)
  plt.xticks(fontsize=14)
  plt.yticks(fontsize=14)
  plt.legend(fontsize=15)
  ax.grid(True)
  plt.subplots_adjust(bottom=0.2, top=0.90, left=0.15)
  plt.savefig(f'singleplot_{obj}_{rawtype}.png')
  plt.close('all')

def draw_trackno(x_vals, y_vals, y_errs, texts, ytitle, obj, rawtype):

      fig = plt.figure(figsize=(8,7))
      ax = fig.add_subplot(111)
      plt.title(r'CMS Preliminary   2024 PbPb Data $\sqrt{s_{NN}} = 5.36$ TeV', loc='left', fontsize=15)
      x_err = []
      for idx, key in enumerate(texts.keys()):
          plt.errorbar(x_vals[key], y_vals[key], yerr=y_errs[key], color=colors[idx], label=key)
          for i, text in enumerate(texts[key]):
                ax.text(x_vals[key][i], y_vals[key][i], text, fontsize=12)
      plt.xlabel('size of approx cluster in Byte', fontsize=15)
      plt.ylabel(ytitle, fontsize=15)
      #plt.xticks(fontsize=15)
      #plt.yticks(fontsize=15)
      plt.legend(fontsize=15, loc='upper left')
      ax.grid(True)
      plt.savefig('charge_cut.png')
      plt.close('all')
                                
def build_array(obj, rawtype):

  texts = {}
  sizes = {}
  yvals = {}

  texts["raw'"] = []
  yvals["raw'"] = []
  sizes["raw'"] = []

  for avgCharge in avgCharges:
    for bit in x:
      update_list(output, bit, avgCharge, rawtype, sizes, yvals, texts, "raw'", options.events)
  
  texts["HI_raw'"] = []
  yvals["HI_raw'"] = []
  sizes["HI_raw'"] = []
  
  update_list('HI_wchargecut', 16, 8, rawtype, sizes, yvals, texts, "HI_raw'", options.events)
  if options.version == 'v2':
    texts['v2'] = []
    yvals['v2'] = []
    sizes['v2'] = []
    update_list('HI_wchargecut_v2', 15, 8, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 15, 5, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 15, 4, rawtype, sizes, yvals, texts, 'v2')
    #update_list('HI_wchargecut_v2', 15, 7, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 15, 6, rawtype, sizes, yvals, texts, 'v2')
    #update_list('HI_wchargecut_v2', 14, 5, rawtype, sizes, yvals, texts, 'v2')
    #update_list('HI_wchargecut_v2', 14, 4, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 14, 8, rawtype, sizes, yvals, texts, 'v2')
    #update_list('HI_wchargecut_v2', 14, 7, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 14, 6, rawtype, sizes, yvals, texts, 'v2')
    #update_list('HI_wchargecut_v2', 13, 8, rawtype, sizes, yvals, texts, 'v2')'''
  elif options.version == 'v1':
    texts['v1'] = []
    yvals['v1'] = []
    sizes['v1'] = []
    update_list('remove_beginindices_v1_compression_LZMA', 14, 8, rawtype, sizes, yvals, texts, 'v1')
    update_list('remove_beginindices_v1_compression_LZMA', 14, 5, rawtype, sizes, yvals, texts, 'v1')
    update_list('remove_beginindices_v1_compression_LZMA', 14, 6, rawtype, sizes, yvals, texts, 'v1')
    update_list('remove_beginindices_v1_compression_LZMA', 14, 7, rawtype, sizes, yvals, texts, 'v1')
    update_list('remove_beginindices_v1_compression_LZMA', 14, 4, rawtype, sizes, yvals, texts, 'v1')
    update_list('remove_beginindices_v1_compression_LZMA', 14, 3, rawtype, sizes, yvals, texts, 'v1')
  elif options.version == 'v1.1':
    texts['v1.1'] = []
    yvals['v1.1'] = []
    sizes['v1.1'] = []
    update_list('HI_wchargecut_v1p1', 14, 8, rawtype, sizes, yvals, texts, 'v1.1')
    update_list('HI_wchargecut_v1p1', 14, 5, rawtype, sizes, yvals, texts, 'v1.1')
    update_list('HI_wchargecut_v1p1', 14, 6, rawtype, sizes, yvals, texts, 'v1.1')
    update_list('HI_wchargecut_v1p1', 14, 7, rawtype, sizes, yvals, texts, 'v1.1')
    update_list('HI_wchargecut_v1p1', 13, 7, rawtype, sizes, yvals, texts, 'v1.1')
    update_list('HI_wchargecut_v1p1', 13, 5, rawtype, sizes, yvals, texts, 'v1.1')
    update_list('HI_wchargecut_v1p1', 13, 6, rawtype, sizes, yvals, texts, 'v1.1')

  pt = obj.split('_')[1].split('pt')[0]
  ylabel = obj.split('_')[0]
  draw(sizes, yvals, texts, f'unmatched {pt} pt {ylabel} in %', obj, rawtype)

if not options.events:
  for raw in ['raw', "rawp"]:
     for obj in ['tracks_lowpt', 'tracks_highpt']:#, 'jet']:
        build_array(obj, raw)
else:

  texts = {}
  sizes = {}
  yvals = {}
  yerrs = {}

  texts["raw':chargecut"] = []
  yvals["raw':chargecut"] = []
  sizes["raw':chargecut"] = []
  yerrs["raw':chargecut"] = []

  update_list('HI_wchargecut_saswati', 15, 6, "rawp", sizes, yvals, texts, "raw':chargecut", options.events, yerrs)

  texts["raw':no chargecut"] = []
  yvals["raw':no chargecut"] = []
  yerrs["raw':no chargecut"] = []
  sizes["raw':no chargecut"] = []
  
  update_list('HI_wochargecut_saswati', 15, 6, "rawp", sizes, yvals, texts, "raw':no chargecut", options.events, yerrs)
  sizes["raw':chargecut"] = [(s1 -s2)*100/s1 for s1, s2 in zip(sizes["raw':no chargecut"], sizes["raw':chargecut"])]
  sizes["raw':no chargecut"] = [1]#[(s1 -s2)*100/s1 for s1, s2 in zip(sizes["raw':chargecut"], sizes["raw':chargecut"])]
  
  texts["HI_raw':chargecut"] = []
  yvals["HI_raw':chargecut"] = []
  yerrs["HI_raw':chargecut"] = []
  sizes["HI_raw':chargecut"] = []

  update_list('HI_wchargecut', 16, 8, "rawp", sizes, yvals, texts, "HI_raw':chargecut", options.events, yerrs)

  texts["HI_raw':no chargecut"] = []
  yvals["HI_raw':no chargecut"] = []
  yerrs["HI_raw':no chargecut"] = []
  sizes["HI_raw':no chargecut"] = []

  update_list('HI_wochargecut', 16, 8, 'rawp', sizes, yvals, texts, "HI_raw':no chargecut", options.events, yerrs)
  sizes["HI_raw':chargecut"] = [(s1 -s2)*100/s1 for s1, s2 in zip(sizes["HI_raw':no chargecut"], sizes["HI_raw':chargecut"])]
  sizes["HI_raw':no chargecut"] = [1]
  draw_trackno(sizes, yvals, yerrs, texts, '# of tracks', 'tracks', 'rawp')
