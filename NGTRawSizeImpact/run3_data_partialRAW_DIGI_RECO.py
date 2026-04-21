# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --conditions auto:run3_data_prompt_relval -s RAW2DIGI --datatier DIGI --eventcontent RECO --data --process reRECO --scenario pp --era Run3 --customise Configuration/DataProcessing/RecoTLR.customisePostEra_Run3 -n 100 --filein /store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root
import sys
import argparse
from importFED import FEDexclude,FEDinclude 
import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_cff import Run3

# Argument parsing setup
parser = argparse.ArgumentParser(description="Configuration script for CMSSW Run 3 event size analysis.")
group = parser.add_mutually_exclusive_group()  # Create a group for mutually exclusive flags
group.add_argument(
    '--pu', 
    action='store_true', 
    help="Run with pileup (PU). This is the default."
)
group.add_argument(
    '--nopu', 
    action='store_true', 
    help="Run without pileup (no PU)."
)

args = parser.parse_args()

pileup = True
if args.nopu:
    pileup=False

# Process setup

process = cms.Process('reRECO',Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

outputFileName = 'run3_data_partialRAW_DIGI_RECO.root'

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root'),
    secondaryFileNames = cms.untracked.vstring('/store/data/Run2024F/Muon0/RAW/v1/000/382/216/00000/6371d4f3-434e-4222-bc48-5edb252e5590.root',
                                            '/store/data/Run2024F/Muon0/RAW/v1/000/382/216/00000/806325bc-6f02-4157-a8fd-b11f7728700d.root',
                                            '/store/data/Run2024F/Muon0/RAW/v1/000/382/216/00000/c0fa023c-10f2-4259-8d79-e8c8b0f67caa.root',
                                            '/store/data/Run2024F/Muon0/RAW/v1/000/382/216/00000/f25d968b-0b83-4d46-89ef-fd96e2dd8a70.root'),
    lumisToProcess = cms.untracked.VLuminosityBlockRange('382216:167-382216:388')
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
    numberOfThreads = cms.untracked.uint32(16),
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

# Pixel, ECAL, ES, HCAL, Strips, Muons, Other
pixel_fed_list = (cms.vuint32(tuple(FEDinclude("Pixel"))))
ecal_fed_list = (cms.vuint32(tuple(FEDinclude("ECAL"))))
es_fed_list = (cms.vuint32(tuple(FEDinclude("ES"))))
hcal_fed_list = (cms.vuint32(tuple(FEDinclude("HCAL"))))
strips_fed_list = (cms.vuint32(tuple(FEDinclude("Strips"))))
muons_fed_list = (cms.vuint32(tuple(FEDinclude("Muons"))))
other_fed_list = (cms.vuint32(tuple(FEDinclude("Other"))))

process.partialRawDataRepackerPixel = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = pixel_fed_list
)
process.partialRawDataRepackerECAL = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = ecal_fed_list
)
process.partialRawDataRepackerES = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = es_fed_list
)
process.partialRawDataRepackerHCAL = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = hcal_fed_list
)
process.partialRawDataRepackerStrips = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = strips_fed_list
)
process.partialRawDataRepackerMuons = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = muons_fed_list
)
process.partialRawDataRepackerOther = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = other_fed_list
)

# Output definition
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('DIGI'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string(outputFileName),
    SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( 'partialrawrepackers_step' ) ),
    outputCommands = cms.untracked.vstring(
            'drop *',
            'keep FEDRawDataCollection_*_*_*',
            'keep *_siStripDigis_*_*',
            'keep *_siPixelDigis_*_*',
            'keep *_ecalDigis_*_*',
            'keep *_hcalDigis_*_*',
            'keep *_simHcalDigis_*_*',
            'keep *_simEcalDigis_*_*',
            'keep *_simSiStripDigis_*_*',
            'keep *_simSiPixelDigis_*_*',
            'keep *_simEcalPreshowerDigis_*_*',
            'keep *_ecalPreshowerDigis_*_*',
            'drop *_*_*_RECO'
            # 'keep *_siPixelClusters_*_RECO',
            # 'keep *_siStripClusters_*_RECO',
      )
    # splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, globaltag = '140X_dataRun3_Prompt_v4')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.partialrawrepackers_step = cms.Path(process.partialRawDataRepackerPixel 
                                       + process.partialRawDataRepackerECAL
                                       + process.partialRawDataRepackerES 
                                    #    + process.partialRawDataRepackerHCAL # Crashing 
                                       + process.partialRawDataRepackerStrips 
                                       + process.partialRawDataRepackerMuons 
                                       + process.partialRawDataRepackerOther)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.RECOoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.partialrawrepackers_step,process.endjob_step,process.output_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from Configuration.DataProcessing.RecoTLR
from Configuration.DataProcessing.RecoTLR import customisePostEra_Run3 

#call to customisation function customisePostEra_Run3 imported from Configuration.DataProcessing.RecoTLR
process = customisePostEra_Run3(process)

# End of customisation functions


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

# Load reconstruction
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')


# Add paths for reconstruction
process.reconstruction_step = cms.Path(process.reconstruction)


# Extend the schedule to include the new steps
process.schedule.extend([process.reconstruction_step])

# Update the output commands to keep clusters, rechits, and generalTracks
process.RECOoutput.outputCommands.extend([
    'keep *_siPixelClusters_*_*',
    'keep *_siStripClusters_*_*',
    'keep *_siPixelRecHits_*_*',
    'keep *_siStripMatchedRecHits_*_*',
    'keep *_generalTracks_*_*',
    'keep *_*recHit*_*_*',
    'keep *_*RecHit*_*_*',
    'keep *_*Cluster*_*_*'
    ])

