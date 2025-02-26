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

def draw(x_vals, y_vals, texts, obj, rawtype):

  fig = plt.figure(figsize=(8,6))
  ax = fig.add_subplot(111)
  plt.scatter(x_vals, y_vals)
  print(obj)
  print(x_vals)
  print(y_vals)
  print(texts)
  sorted_xvals = sorted(x_vals)
  plt.text(sorted_xvals[len(x_vals)-3], max(y_vals), '(barycenter,avgCharge)')
  plt.title(f'size vs unmatched {obj} in % for {rawtype}', fontsize=20)
  plt.xlabel('size', fontsize=20)
  plt.ylabel('unmatched in %', fontsize=20)
  for i, text in enumerate(texts):
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

  draw(sizes, unmatched, texts, obj, rawtype)

for raw in ['raw', 'rawp']:
  for obj in ['tracks_lowpt', 'tracks_highpt', 'jet']:
     build_array(obj, raw)
