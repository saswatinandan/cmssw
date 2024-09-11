import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-r", dest="input_raw", default='', help="input for raw")
parser.add_argument("-rp", dest="input_rawp", default='', help="input for rawp")
parser.add_argument("-n", dest="n", default='-1', help="number of events to be run")
parser.add_argument("-c", action='store_true', dest="cluster", default=False, help="make flatntuple for cluster")

options = parser.parse_args()

input_for_raw = options.input_raw
output_for_raw = f'flatntuple_{input_for_raw}'
os.system(f'cmsRun sep19_2_2_dump_raw.py inputFiles={input_for_raw} outputFile={output_for_raw} n={options.n} c={options.cluster}')

input_for_rawp = options.input_rawp
output_for_rawp = f'flatntuple_{input_for_rawp}'
os.system(f'cmsRun sep19_2_1_dump_rawprime.py inputFiles={input_for_rawp} outputFile={output_for_rawp} n={options.n} c={options.cluster}')

os.system('g++ `root-config --cflags --glibs` -O3 rootMacro/LHCC_raw_vs_rawprime.cc -o rootMacro/LHCC_raw_vs_rawprime.o')
os.system(f'./rootMacro/LHCC_raw_vs_rawprime.o flatntuple_{input_for_rawp} flatntuple_{input_for_raw}')
