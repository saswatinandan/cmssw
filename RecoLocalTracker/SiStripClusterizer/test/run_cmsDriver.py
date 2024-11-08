import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-b", dest="barycenter_bit", default='8bit', help="bit to be studied for barycenter")
parser.add_argument("-w", dest="width_bit", default='8bit', help="bit to be studied for width")
parser.add_argument("-n", dest="number", default='100', help="how many numbers of events")
parser.add_argument("-t", dest="threads", default='20', help="how many threads")

options = parser.parse_args()
barycenter_bit = options.barycenter_bit
number = options.number
threads = options.threads
width_bit = options.width_bit

def replace_line(infile, replaces_to_vals):

    with open(infile, 'r') as f:
      lines = f.readlines()

    with open(infile, 'w') as f:
        for line in lines:
            for replace_to_val in replaces_to_vals:
                replace = replace_to_val[0]
                val = replace_to_val[1]
                if replace in line:
                   line = line.replace(replace, val)
                   break 
            f.write(line)

### SiStripApproxCluster.h ####

#barycenter_bit = int(barycenter_bit.strip('bit'))
maxRange_ = (1<<int(barycenter_bit.strip('bit'))) -1
replace_line('../../../DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h',
             [('maxRange_ = ', f'maxRange_ = {maxRange_}; //')])
    
#width_bit = int(width_bit.strip('bit'))
maxRange_ = (1<<int(width_bit.strip('bit'))) -1
replace_line('../../../DataFormats/SiStripCluster/src/SiStripApproximateCluster.cc',
               [('width_ = ', 'width_ = ' + f'std::min({maxRange_},(int)cluster.size());//')])

run_cmd = 'scram b -j 8'
print(run_cmd)
os.system(run_cmd)

### hlt ###

cmd_hlt = f'hltGetConfiguration /users/vmuralee/PREmenu/V9 --globaltag 410X_dataRun3_HLT_for2024TSGStudies_v1 --data --unprescale --max-events {number} --eras Run3 --input /store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root > prehlt.py'

#os.system(cmd_hlt)

#replace_line('prehlt.py', ["'keep FEDRawDataCollection_rawPrimeDataRepacker_*_*',", "outputPhysicsHIPhysicsRawPrime0.root"],
#              ["'keep *_*siStripClusters*_*_*',\n'keep *_*generalTracks*_*_*',\n'keep FEDRawDataCollection_raw*_*_HLTX',\n", f"outputPhysicsHIPhysicsRawPrime0_{bit}.root"])

run_cmd = 'cmsRun prehlt.py &> prehlt.log'
print(run_cmd)
os.system(run_cmd)
output_prehlt = f'outputPhysicsHIPhysicsRawPrime0_barycenter_{barycenter_bit}_width_{width_bit}.root'

os.system(f'mv outputPhysicsHIPhysicsRawPrime0.root {output_prehlt}')


cmd_reco_step = f'cmsDriver.py step_reco --conditions 140X_dataRun3_Prompt_v3 -s RAW2DIGI,L1Reco,RECO --datatier RECO --eventcontent RECO --data --process reRECO --scenario pp -n {number} --repacked --era Run3_pp_on_PbPb_approxSiStripClusters --filein file:{output_prehlt} --no_exec --nThreads {threads}'
print(cmd_reco_step)
os.system(cmd_reco_step)

output_step_reco = f'step_reco_RAW2DIGI_L1Reco_RECO_barycenter_{barycenter_bit}_width_{width_bit}.root'

replace_line('step_reco_RAW2DIGI_L1Reco_RECO.py',
             [
               ('step_reco_RAW2DIGI_L1Reco_RECO.root', output_step_reco),
               ('from Configuration.Eras.Era_Run3_pp_on_PbPb_approxSiStripClusters_cff import Run3_pp_on_PbPb_approxSiStripClusters', 'from Configuration.Eras.Era_Run2024_pp_on_PbPb_approxSiStripCluster import Run3_pp_on_PbPb_approxSiStripClusters_2024'),
               ("process = cms.Process('reRECO',Run3_pp_on_PbPb_approxSiStripClusters)", "process = cms.Process('reRECO',Run3_pp_on_PbPb_approxSiStripClusters_2024)"),
               ('outputCommands = process.RECOEventContent.outputCommands', "outputCommands = cms.untracked.vstring( 'drop *',\n'keep *_*siStripClusters*_*_*',\n'keep *_*generalTracks*_*_*',\n'keep *_hltSiStripClusters2ApproxClusters_*_*',\n'keep *_ak4PFJets_*_*',\n'keep *_*pfMet*_*_*')\n")
            ])
run_cmd = 'cmsRun step_reco_RAW2DIGI_L1Reco_RECO.py &> step_reco_RAW2DIGI_L1Reco_RECO.log'
print(run_cmd)
os.system(run_cmd)

#### object comparison ####

run_cmd = f'python3 run_flatNtuplizer.py -rp {output_step_reco} -c -n {number}'
print(run_cmd)
os.system(run_cmd)

run_cmd = f"edmEventSize -v {output_step_reco} > size.log"
print(run_cmd)
os.system(run_cmd)
