# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --conditions auto:phase1_2025_realistic --datatier GEN-SIM-RECO --era Run3_2025 --eventcontent RECO --filein file:step2.root --fileout file:step3.root --geometry DB:Extended --nStreams 1 --nThreads 8 --no_exec --number 100 --pileup Run3_Flat55To75_PoissonOOTPU --pileup_input das:/RelValMinBias_14TeV/CMSSW_15_1_0-151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/GEN-SIM --python_filename step_3_wPU_cfg.py --step RAW2DIGI,L1Reco,RECO --customise_commands process.siStripClusters.Clusterizer.clusterChargeCut.value=1945.0
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_2025_cff import Run3_2025

process = cms.Process('RECO',Run3_2025)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mix_Run3_Flat55To75_PoissonOOTPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:step2.root'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    TryToContinue = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToCallForTryToContinue = cms.untracked.vstring(),
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step3 nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RECOoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RECO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:step3.root'),
    outputCommands = process.RECOEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.mix.input.fileNames = cms.untracked.vstring(['/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/02918cb1-5160-441b-a141-fbe31e108211.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/066567b7-5d2d-4037-baed-6be12a8a7413.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/06d2f2dc-e487-46d1-a075-67567d72a41c.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/088c4420-561d-472f-97b1-dc79586d5f56.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/106680d9-ed04-497d-b9c9-1ceba1637b81.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/1087521f-dfa4-494d-9aff-89e46872a67f.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/18830714-f662-4017-8925-1ceea86d9d88.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/18bc6ce2-dd61-456b-b921-d25d162a73c7.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/225b455a-eab2-47c8-80ad-07eea010593d.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/245bf399-f1bd-4756-a230-fe2e4f796536.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/24fd93b8-fdef-4566-a199-49f6d249e897.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/2b9dcd57-cdfe-4911-91f8-c9b34e1595b9.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/2d028a47-766a-42d8-8c4b-1d3c8732c94a.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/2eae9889-b3f0-4118-ba23-d279cfafed6f.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/3837c046-fec4-4525-b034-8e31b12b9500.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/3e7365f9-9d21-4df5-80be-a407392801de.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/45f76956-f22d-460a-b12b-08c53e296333.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/52f9f55d-d773-4eb7-aa92-8df1925bc9c5.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/53bdfb0a-7cc4-414f-abda-abc42ed3be4b.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/5f9c5ed2-cd61-4fa9-8a24-3c2988d858bf.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/6843136a-3d4d-456f-a62a-085a3a11f8ac.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/6e2a9b63-cada-432b-bf06-bde974f1d9cd.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/76318a9f-6c4a-4192-acd1-365e5c3f0fe3.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/7f51a660-b0d2-48bf-92a5-7e23359f389d.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/8405aefe-5207-4498-990b-7f9f9e555bff.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/874b586f-bcd5-4181-9c48-d1ceeef6395c.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/8859e1a5-1697-4c58-ae66-3a188b77b6a5.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/8e7b518b-9237-4593-a80d-e455f6ff7988.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/8eb9f77a-f982-499c-a96b-5d7164c60d90.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/9add7b59-74e7-4ce9-b339-e98a299ef8aa.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/a5895478-5e2f-4616-a9b1-3d517eb96719.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/b2f26ebb-6e5a-4823-a0f4-2efff2e5c231.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/b5d693ff-de28-4ec1-abeb-fcc933675954.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/b8c85e0d-4e10-4236-a60b-b1817a041888.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/b95a05eb-e596-41e1-96bd-87510424d3cf.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/bc0b24d8-588e-4551-861c-668cc8cfe5e1.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/bffd40a9-a8bd-475c-9e0e-c8c388f57420.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/c3f5855d-4e33-4937-bc0c-1b8aaa51b0c6.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/c6af463a-72d0-4955-8c00-f2ce0bac0d00.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/d2e60973-697d-41db-acf9-c04b4fea33af.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/d58ea08e-ff7a-4974-9444-93078c874d69.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/d6920bd1-c0a9-4f97-9ba1-3176facd5090.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/d74fc35c-ed4c-424a-aea6-c39190d15467.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/d7e50727-1318-4b90-9e9a-71c53f9ab133.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/dfd7ad38-c9bd-4fd8-9250-dda6b7cf3671.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/f5a093d9-ca4e-4962-8c22-d92a6a1b016c.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/f9a23bff-dc86-4e91-a182-5936ef4276f5.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/fa31e923-6f5a-4c81-a9b9-de77a810706b.root', '/store/relval/CMSSW_15_1_0/RelValMinBias_14TeV/GEN-SIM/151X_mcRun3_2025_realistic_v4_STD_MinBias_2025_GenSim-v1/2590000/fceb638e-e8a6-470b-9e61-fc1e4a65c301.root'])
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2025_realistic', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.endjob_step,process.RECOoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads = 1
process.options.numberOfStreams = 1



# Customisation from command line

process.siStripClusters.Clusterizer.clusterChargeCut.value=1945.0
#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
