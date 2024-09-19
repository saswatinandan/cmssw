import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-b", dest="bit", default='8bit', help="bit to be studied")
parser.add_argument("-n", dest="number", default='100', help="how many numbers of events")
parser.add_argument("-t", dest="threads", default='20', help="how many threads")

options = parser.parse_args()
bit = options.bit
number = options.number
threads = options.threads

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

bit_no = int(bit.strip('bit'))
maxRange_ = (1<<bit_no) -1
replace_line('../../../DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h',
             [('maxRange_ = ', f'maxRange_ = {maxRange_}; //')])

run_cmd = 'scram b -j 8'
print(run_cmd)
os.system(run_cmd)

### hlt ###

cmd_hlt = f'hltGetConfiguration /users/vmuralee/PREmenu/V9 --globaltag 140X_dataRun3_HLT_for2024TSGStudies_v1 --data --unprescale --max-events {number} --eras Run3 --input /store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root > prehlt.py'

#os.system(cmd_hlt)

#replace_line('prehlt.py', ["'keep FEDRawDataCollection_rawPrimeDataRepacker_*_*',", "outputPhysicsHIPhysicsRawPrime0.root"],
#              ["'keep *_*siStripClusters*_*_*',\n'keep *_*generalTracks*_*_*',\n'keep FEDRawDataCollection_raw*_*_HLTX',\n", f"outputPhysicsHIPhysicsRawPrime0_{bit}.root"])

run_cmd = 'cmsRun prehlt.py'
print(run_cmd)
os.system(run_cmd)
os.system(f'mv outputPhysicsHIPhysicsRawPrime0.root outputPhysicsHIPhysicsRawPrime0_{bit}.root')


#### step2 ####
#Run3_pp_on_PbPb_approxSiStripClusters
cmd_step2 = f"cmsDriver.py step2 --scenario pp --conditions auto:run3_data_prompt -s REPACK:DigiToApproxClusterRaw --datatier GEN-SIM-DIGI-RAW-HLTDEBUG --era Run3_pp_on_PbPb_approxSiStripClusters --eventcontent REPACKRAW -n {number} --customise_commands"
cmd_step2 += ' "process.rawPrimeDataRepacker.src='
cmd_step2 += "'rawDataRepacker'"
cmd_step2 += f'" --repacked --process ReHLT --filein file:outputPhysicsHIPhysicsRawPrime0_{bit}.root --no_exec'

print(cmd_step2)
os.system(cmd_step2)
replace_line('step2_REPACK.py', 
               [( 'step2_REPACK.root', f'step2_REPACK_{bit}.root'),
                ('from Configuration.Eras.Era_Run3_pp_on_PbPb_approxSiStripClusters_cff import Run3_pp_on_PbPb_approxSiStripClusters', 'from Configuration.Eras.Era_Run2024_pp_on_PbPb_approxSiStripCluster import Run3_pp_on_PbPb_approxSiStripClusters_2024'),
               ("process = cms.Process('ReHLT',Run3_pp_on_PbPb_approxSiStripClusters)", "process = cms.Process('ReHLT',Run3_pp_on_PbPb_approxSiStripClusters_2024)")
])
run_cmd = ('cmsRun step2_REPACK.py')
print(run_cmd)
os.system(run_cmd)

###step3 ####

cmd_step3 = f'cmsDriver.py step3 --conditions auto:run3_data_prompt -s RAW2DIGI,L1Reco,RECO --datatier RECO --eventcontent RECO --data --process reRECO --scenario pp -n {number} --repacked --era Run3_pp_on_PbPb_approxSiStripClusters --filein file:step2_REPACK_{bit}.root --no_exec --nThreads {threads}'

print(cmd_step3)
os.system(cmd_step3)

replace_line('step3_RAW2DIGI_L1Reco_RECO.py',
             [
               ('step3_RAW2DIGI_L1Reco_RECO.root', f'step3_RAW2DIGI_L1Reco_RECO_{bit}.root'),
               ('from Configuration.Eras.Era_Run3_pp_on_PbPb_approxSiStripClusters_cff import Run3_pp_on_PbPb_approxSiStripClusters', 'from Configuration.Eras.Era_Run2024_pp_on_PbPb_approxSiStripCluster import Run3_pp_on_PbPb_approxSiStripClusters_2024'),
               ("process = cms.Process('reRECO',Run3_pp_on_PbPb_approxSiStripClusters)", "process = cms.Process('reRECO',Run3_pp_on_PbPb_approxSiStripClusters_2024)"),
               ('outputCommands = process.RECOEventContent.outputCommands', "outputCommands = cms.untracked.vstring( 'drop *',\n'keep *_*siStripClusters*_*_*',\n'keep *_*generalTracks*_*_*',\n'keep *_hltSiStripClusters2ApproxClusters_*_*',\n'keep *_ak4PFJets_*_*',\n'keep *_*pfMet*_*_*',\n'keep DetIds_hltSiStripRawToDigi_*_ReHLT',\n'keep FEDRawDataCollection_raw*_*_ReHLT',\n'keep FEDRawDataCollection_hltSiStripDigiToZSRaw_*_ReHLT',\n'keep GlobalObjectMapRecord_hltGtStage2ObjectMap_*_ReHLT',\n'keep edmTriggerResults_*_*_ReHLT',\n'keep triggerTriggerEvent_*_*_ReHLT')\n")
            ])
run_cmd = 'cmsRun step3_RAW2DIGI_L1Reco_RECO.py'
print(run_cmd)
os.system(run_cmd)

#### object comparison ####

run_cmd = f'python3 run_flatNtuplizer.py -r step5_RAW2DIGI_L1Reco_RECO.root -rp step3_RAW2DIGI_L1Reco_RECO_{bit}.root -c -n {number}'
print(run_cmd)
os.system(run_cmd)
