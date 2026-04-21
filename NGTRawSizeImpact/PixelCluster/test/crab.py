from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'muon_analyzer'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'muonanalyzer.py'  # Your cmsRun config
config.JobType.allowUndistributedCMSSW = True  # <-- Use local code
config.JobType.numCores=4
config.Data.inputDataset = '/TT_TuneCP5_14TeV-powheg-pythia8/Phase2Spring24DIGIRECOMiniAOD-PU200_AllTP_140X_mcRun4_realistic_v4-v1/GEN-SIM-DIGI-RAW-MINIAOD'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.inputDBS = 'global'
config.JobType.maxMemoryMB = 4000
config.Data.publication = True
config.Data.outputDatasetTag = 'muon_analyzer'

config.Site.storageSite = 'T2_IT_Pisa'
