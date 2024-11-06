import os
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument("-n", dest="number", default='100', help="how many numbers of events")
parser.add_argument("-b", dest="barycenter_bits", nargs='+', default=[16, 10, 12, 14, 8, 6, 4], help="bit to be studied")
parser.add_argument("-w", dest="width_bits", nargs='+', default=[], help="bit to be studied for width")
parser.add_argument("-t", dest="threads", default=20, help="# of threads")
parser.add_argument("-r", type=int, dest="remove", default=0, help="want to delete directory")
parser.add_argument("-c", action='store_false', dest="cms_command", default=1, help="either want to run cmsDriver commands or only the .cc file")
parser.add_argument("-o", dest="output", default='output', help="output directory name")

options = parser.parse_args()
barycenter_bits = options.barycenter_bits
width_bits = options.width_bits
output = options.output

with open('makefile', 'w') as f:

    allbit = ' '.join([str(b)+'bit'+'_'+str(w)+'bit' for b in barycenter_bits for w in width_bits])
    allbit_plot = allbit + ' plot'
    f.write(f'all: {allbit_plot}\n')
    for bb in barycenter_bits:
      for wb in width_bits:
         f.write(f'{bb}bit_{wb}bit:\n')
         output += f'_barycenter_{bb}bit_width_{wb}bit'
         if options.cms_command:
            f.write(f'\tbash build_env.sh {bb}bit {wb}bit {options.number} {options.threads} {options.remove} {output}\n')
         else:
            f.write(f'\tcd {output} && ../rootMacro/LHCC_rawprime_clusters.o flatntuple_step3_RAW2DIGI_L1Reco_RECO_{bit}bit.root ~/backup/flatntuple_step5_RAW2DIGI_L1Reco_RECO.root > cluster.log && ../rootMacro/LHCC_raw_vs_rawprime.o flatntuple_step3_RAW2DIGI_L1Reco_RECO_{bit}bit.root ~/backup/flatntuple_step5_RAW2DIGI_L1Reco_RECO.root > object.log\n')
    f.write(f'\nplot: {allbit}\n')
    #bits = ' '.join([b for b in bits])
    #f.write(f'\t python3 plot_comparison.py -b {bits}')

os.system(f'make -f makefile -j {len(barycenter_bits)}')
 
