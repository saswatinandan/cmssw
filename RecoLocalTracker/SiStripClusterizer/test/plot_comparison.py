import numpy as np
import matplotlib.pyplot as plt
import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-b", type=int, dest="bits", nargs='+', default=[], help="bit to be studied")
options = parser.parse_args()
bits = options.bits

x = np.array(bits)
x = np.sort(x)

for obj in ['cluster', 'tracks', 'jets']:

   y = []

   for bit in x:

      if obj in ['tracks', 'jets']:
         input = os.path.join('/gpfs/ddn/users/', os.getlogin(), f'{bit}bit', os.environ['CMSSW_BASE'].split('/')[-1], f'src/RecoLocalTracker/SiStripClusterizer/test/object.log')
      else:
         input = os.path.join('/gpfs/ddn/users/', os.getlogin(), f'{bit}bit', os.environ['CMSSW_BASE'].split('/')[-1], f'src/RecoLocalTracker/SiStripClusterizer/test/cluster.log')

      with open(input, 'r') as f:
         lines = f.readlines()

      for line in lines:
        if f'not matched {obj}' in line and 'raw ' in line:
             y.append(float(line.split('in raw')[-1].split('%')[0]))
   y = np.array(y)
   print(obj)
   print(x)
   print(y)
   fig = plt.figure(figsize=(8,6))
   ax = fig.add_subplot(111)
   plt.scatter(x,y)
   plt.title(obj)
   plt.xlabel('bit')
   plt.ylabel(f'unmatched {obj} in raw data in %')
   plt.savefig(f'{obj}.png')
   plt.close('all')

