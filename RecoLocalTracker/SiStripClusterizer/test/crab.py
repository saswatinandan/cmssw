from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'reco_raw'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'step_RAW2DIGI_L1Reco_RECO_PAT.py'  # Your cmsRun config
#config.JobType.allowUndistributedCMSSW = True  # <-- Use local code

config.Data.inputDataset = '/EphemeralHLTPhysics4/Run2025G-v1/RAW'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 100

config.Site.storageSite = 'T2_IT_Pisa'
