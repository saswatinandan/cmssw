import os
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument("-n", dest="number", default='100', help="how many numbers of events")
parser.add_argument("-b", dest="bits", nargs='+', default=[16, 10, 12, 14, 8, 6, 4], help="bit to be studied")
parser.add_argument("-t", dest="threads", default=20, help="# of threads")
parser.add_argument("-r", type=int, dest="remove", default=0, help="want to delete directory")

options = parser.parse_args()
bits = options.bits

with open('makefile', 'w') as f:

    allbit = ' '.join([str(b)+'bit' for b in bits])
    allbit_plot = allbit + ' plot'
    f.write(f'all: {allbit_plot}\n')
    for bit in bits:
      f.write(f'{bit}bit:\n')
      f.write(f'\tbash build_env.sh {bit}bit {options.number} {options.threads} {options.remove}\n')
    f.write(f'\nplot: {allbit}\n')
    bits = ' '.join([b for b in bits])
    f.write(f'\t python3 plot_comparison.py -b {bits}')

#os.system(f'make -f makefile -j {len(bits)}')
 
