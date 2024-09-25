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

for obj in ['size', 'cluster', 'tracks', 'jets']:

   y = []

   for bit in x:

      input = os.path.join('/gpfs/ddn/users/', os.getlogin(), f'{bit}bit', os.environ['CMSSW_BASE'].split('/')[-1], f'src/RecoLocalTracker/SiStripClusterizer/test/')

      if obj == 'size':
         input_file = os.path.join(input, 'size.log')
      elif obj in ['tracks', 'jets']:
         input_file = os.path.join(input, 'object.log')
      else:
         input_file = os.path.join(input, 'cluster.log')

      with open(input_file, 'r') as f:
         lines = f.readlines()

      for line in lines:
        if obj == 'size' and 'SiStripApproximateClusterCollection_hltSiStripClusters2ApproxClusters__ReHLT' in line:
          y.append(float(line.split(' ')[-1]))
        elif f'not matched {obj}' in line and 'raw ' in line:
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
   if obj == 'size':
      plt.ylabel('Average Compressed Size (Bytes/Event) for hltSiStripClusters2ApproxClusters')
   else:
      plt.ylabel(f'unmatched {obj} in raw data in %')
   plt.savefig(f'{obj}.png')
   plt.close('all')

