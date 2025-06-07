import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import argparse
from ROOT import TFile, TH1
from matplotlib.ticker import ScalarFormatter

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

def update_list(dirname, bary_bit, chrg_bit, rawtype, sizes, yvals, texts, ver, events=0, optimizer='Std'):

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
      if optimizer=='Std':
        if f'{obj}' in line and  f'Std:' in line and 'ratio' in line:
              val = float(line.split(f'Std:')[-1])#.split('%')[0])
              yvals[ver].append(val)
      else:
            if f'not matched {obj}' in line and  f'{rawtype} ' in line:
              val = float(line.split(f'in {rawtype} ')[-1].split('%')[0])
              yvals[ver].append(val)
  else:
    f = TFile(input_file, 'r')
    yvals[ver].append(f.Get(f'{rawtype}_trk_cutflow').GetBinContent(4))

  #print(sizes)
  #print(yvals)
  texts[ver].append((int(f'{bary_bit}'), int(f'{chrg_bit}')))

def draw(x_vals, y_vals, texts, ytitle, obj, rawtype):

  fig = plt.figure(figsize=(8,7))
  ax = fig.add_subplot(111)
  max_ = max(max(y_vals.values()))*(1.10)
  min_ = min(min(y_vals.values()))*(0.80)
  plt.ylim(top=max_,bottom=min_)
  plt.text(0.15, 0.98, r'CMS $\it{Preliminary}$',
     horizontalalignment='center',
     verticalalignment='top',
     transform=plt.gca().transAxes,
     fontsize=15
  )
  #plt.title('CMS Preliminary', fontsize=15, loc='left')
  plt.title('PbPb collisions, 2024 (5.36 TeV)', fontsize=15, loc='right')
  for idx, key in enumerate(texts.keys()):
    plt.scatter(x_vals[key], y_vals[key], color=colors[idx], label=key)
    for i, text in enumerate(texts[key]):
      if text[0] == 15 and text[1] == 8 and key=="raw'":
            x_vals[key][i] -= 1.8
      ax.text(x_vals[key][i], y_vals[key][i], text, fontsize=12)#, color=colors[idx])

  formatter = ScalarFormatter(useMathText=True)
  formatter.set_scientific(True)
  formatter.set_powerlimits((-1, 1))  # controls the range that triggers sci notation
  ax.yaxis.set_major_formatter(formatter)
  
  plt.xlabel("$\Delta(size)/size$ in %", fontsize=16, labelpad=15)
  plt.ylabel("$\sigma(\Delta p_{T}/p_T)$ "+ ytitle, fontsize=16, labelpad=15)
  plt.xticks(fontsize=14)
  plt.yticks(fontsize=14)
  plt.legend(fontsize=15)
  ax.grid(True)
  plt.subplots_adjust(bottom=0.2, top=0.90, left=0.15)
  plt.savefig(f'singleplot_{obj}_{rawtype}.png')
  plt.close('all')

def draw_trackno(x_vals, y_vals, texts, ytitle, obj, rawtype):

      fig = plt.figure(figsize=(8,7))
      ax = fig.add_subplot(111)
      plt.title(r'CMS $\it{Preliminary}$', fontsize=15, loc='left')
      plt.title('PbPb collisions, 2024 (5.36 TeV)', fontsize=15, loc='right')
      for idx, key in enumerate(texts.keys()):
          plt.scatter(x_vals[key], y_vals[key], color=colors[idx], label=key)
          for i, text in enumerate(texts[key]):
                if x_vals[key][i] > 292500:
                    x = x_vals[key][i] - 1000
                else:
                    x = x_vals[key][i] 
                ax.text(x, y_vals[key][i], text, fontsize=12)
      plt.annotate('', xy=(x_vals["raw':chargecut"][0]-40, y_vals["raw':chargecut"][0]), xytext=(x_vals["raw':no chargecut"][0], y_vals["raw':no chargecut"][0]),arrowprops=dict(arrowstyle='->', linestyle='dotted', color='black', lw=2))
      plt.annotate('', xy=(x_vals["HI_raw':chargecut"][0], y_vals["HI_raw':chargecut"][0]), xytext=(x_vals["HI_raw':no chargecut"][0], y_vals["HI_raw':no chargecut"][0]),arrowprops=dict(arrowstyle='->', linestyle='dotted', color='black', lw=2))
      plt.xlabel('size of approx cluster collection in Byte', fontsize=15)
      plt.ylabel(ytitle, fontsize=15)
      plt.legend(fontsize=15, loc='best')
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
      update_list(output, bit, avgCharge, rawtype, sizes, yvals, texts, "raw'", options.events, 'Std')
  
  texts["HI_raw'"] = []
  yvals["HI_raw'"] = []
  sizes["HI_raw'"] = []
  
  update_list('HI_wchargecut', 16, 8, rawtype, sizes, yvals, texts, "HI_raw'", options.events, 'Std')
  sizes["raw'"] = [(s1 -sizes["HI_raw'"][0])*100/sizes["HI_raw'"][0] for s1 in sizes["raw'"]]
  
  if options.version == 'v2':
    texts['v2'] = []
    yvals['v2'] = []
    sizes['v2'] = []
    update_list('HI_wchargecut_v2', 15, 8, rawtype, sizes, yvals, texts, 'v2', options.events, 'Std')
    update_list('HI_wchargecut_v2', 15, 5, rawtype, sizes, yvals, texts, 'v2', options.events, 'Std')
    update_list('HI_wchargecut_v2', 15, 4, rawtype, sizes, yvals, texts, 'v2', options.events, 'Std')
    #update_list('HI_wchargecut_v2', 15, 7, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 15, 6, rawtype, sizes, yvals, texts, 'v2', options.events, 'Std')
    #update_list('HI_wchargecut_v2', 14, 5, rawtype, sizes, yvals, texts, 'v2')
    #update_list('HI_wchargecut_v2', 14, 4, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 14, 8, rawtype, sizes, yvals, texts, 'v2', options.events, 'Std')
    #update_list('HI_wchargecut_v2', 14, 7, rawtype, sizes, yvals, texts, 'v2')
    update_list('HI_wchargecut_v2', 14, 6, rawtype, sizes, yvals, texts, 'v2', options.events, 'Std')
    #update_list('HI_wchargecut_v2', 13, 8, rawtype, sizes, yvals, texts, 'v2')'''
    sizes["v2"] = [(s1 -sizes["HI_raw'"][0])*100/sizes["HI_raw'"][0] for s1 in sizes["v2"]]
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

  sizes["HI_raw'"] = [(s1 - s1)*100/sizes["HI_raw'"][0] for s1 in sizes["HI_raw'"]]
  pt = obj.split('_')[1].split('pt')[0]
  ylabel = obj.split('_')[0]
  draw(sizes, yvals, texts, f'for {pt} pt {ylabel}', obj, rawtype)

if not options.events:
  for raw in ['raw', "rawp"]:
     for obj in ['tracks_lowpt', 'tracks_highpt']:#, 'jet']:
        build_array(obj, raw)
else:

  texts = {}
  sizes = {}
  yvals = {}

  texts["raw':chargecut"] = []
  yvals["raw':chargecut"] = []
  sizes["raw':chargecut"] = []

  update_list('HI_wchargecut_saswati', 15, 6, "rawp", sizes, yvals, texts, "raw':chargecut", options.events)

  texts["raw':no chargecut"] = []
  yvals["raw':no chargecut"] = []
  sizes["raw':no chargecut"] = []

  update_list('HI_wochargecut_saswati', 15, 6, "rawp", sizes, yvals, texts, "raw':no chargecut", options.events)

  texts["HI_raw':chargecut"] = []
  yvals["HI_raw':chargecut"] = []
  sizes["HI_raw':chargecut"] = []

  update_list('HI_wchargecut', 16, 8, "rawp", sizes, yvals, texts, "HI_raw':chargecut", options.events)

  texts["HI_raw':no chargecut"] = []
  yvals["HI_raw':no chargecut"] = []
  sizes["HI_raw':no chargecut"] = []

  update_list('HI_wochargecut', 16, 8, 'rawp', sizes, yvals, texts, "HI_raw':no chargecut", options.events)

  draw_trackno(sizes, yvals, texts, 'number of tracks', 'tracks', 'rawp')
