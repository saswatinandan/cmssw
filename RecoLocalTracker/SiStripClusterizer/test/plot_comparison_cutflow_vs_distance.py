import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import argparse
from ROOT import TFile, TH1, TH2, TCanvas

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
no_track_algo = 27

colors = ['r', 'b', 'g', 'c', 'm', 'y', 'k', 'w']

def readfile(input_file):

  with open(input_file, 'r') as f:
     lines = f.readlines()
  return lines

def draw(x_vals, y_vals, yerrs, axis, algo):

  fig = plt.figure()
  for key, x_val in x_vals.items():
    plt.errorbar(x_val, y_vals[key], yerr=yerrs[key], label=key)
  plt.xlabel(f'abs({axis}) -coordinate', fontsize=10)
  plt.ylabel('#rawp/raw tracks after selection', fontsize=10)
  plt.title(algo, fontsize=10)
  plt.legend()
  plt.savefig(f'singleplot_cutflow_vs_{axis}_barycenter_{key[0]}bit_{algo}.png')
  plt.close('all')

def build_array():

  for trk_algo in range(0,no_track_algo):
    for axis in ['xy', 'z']:
      for bit in x:
         xvals = {}
         yvals = {}
         yerrs = {}
         c = TCanvas('','',800,800)
         for avgCharge in avgCharges:
            xvals[(bit,avgCharge)] = []
            yvals[(bit,avgCharge)] = []
            yerrs[(bit,avgCharge)] = []
            input = f'/scratch/nandan/{output}_barycenter_{bit}bit_width_8bit_avgCharge_{avgCharge}bit'
            input_file = TFile(os.path.join(input, 'object_study.root'), 'r')
            cutflow_r = input_file.Get(f'raw_trk_cutflow_{axis}{trk_algo}')
            cutflow_rp = input_file.Get(f'rawp_trk_cutflow_{axis}{trk_algo}')
            ybin = cutflow_r.GetNbinsY()
            cutflow_rp.Divide(cutflow_r)
            for xbin in range(1,cutflow_r.GetNbinsX()+1):
                xvals[(bit,avgCharge)].append(cutflow_r.GetXaxis().GetBinCenter(xbin))
                yvals[(bit,avgCharge)].append(cutflow_rp.GetBinContent(xbin,ybin))
                yerrs[(bit,avgCharge)].append(cutflow_rp.GetBinError(xbin,ybin))
         draw(xvals, yvals, yerrs, axis, cutflow_rp.GetTitle())

build_array()
