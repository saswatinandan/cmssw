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

general_trk_algos = {'pixel_strip_seeded':[8,9,10,26], 'pixel_seeded_1': [4,11,13,14], 'pixel_seeded_2':[23,22,5,24,7,6]}

def draw(x_vals, y_vals, yerrs, axis, title):

  fig = plt.figure()
  for key, x_val in x_vals.items():
    plt.errorbar(x_val, y_vals[key], yerr=yerrs[key], label=key)
  plt.xlabel(f'abs({axis}) -coordinate', fontsize=10)
  plt.ylabel('#rawp/raw tracks after selection', fontsize=10)
  plt.title((title[0],title[1],title[2]), fontsize=10)
  plt.legend()
  plt.savefig(f'singleplot_cutflow_vs_{axis}_barycenter_{title[0]}bit_avgcharge_{title[1]}bit_{title[2]}.png')
  plt.close('all')

def build_array():

  for avgCharge in avgCharges:
    for bit in x:
       for axis in ['xy', 'z']:
         for algo_name,algo_indices  in general_trk_algos.items():
            xvals = {}
            yvals = {}
            yerrs = {}
            for algo_index in algo_indices:
               input = f'/scratch/nandan/{output}_barycenter_{bit}bit_width_8bit_avgCharge_{avgCharge}bit'
               input_file = TFile(os.path.join(input, 'object_study.root'), 'r')
               cutflow_r = input_file.Get(f'raw_trk_cutflow_{axis}{algo_index}')
               cutflow_rp = input_file.Get(f'rawp_trk_cutflow_{axis}{algo_index}')
               c = TCanvas('','',800,800)
               xvals[cutflow_rp.GetTitle()] = []
               yvals[cutflow_rp.GetTitle()] = []
               yerrs[cutflow_rp.GetTitle()] = []
               ybin = cutflow_r.GetNbinsY()
               cutflow_rp.Divide(cutflow_r)
               for xbin in range(1,cutflow_r.GetNbinsX()+1):
                  xvals[cutflow_rp.GetTitle()].append(cutflow_r.GetXaxis().GetBinCenter(xbin))
                  yvals[cutflow_rp.GetTitle()].append(cutflow_rp.GetBinContent(xbin,ybin))
                  yerrs[cutflow_rp.GetTitle()].append(cutflow_rp.GetBinError(xbin,ybin))
            draw(xvals, yvals, yerrs, axis, (bit, avgCharge,algo_name))

build_array()
