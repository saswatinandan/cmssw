#~/tmp/CMSSW_14_0_11/src/RecoLocalTracker/SiStripClusterizer/test/step5_RAW2DIGI_L1Reco_RECO.root outputFile=test.root
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from Configuration.StandardSequences.Eras import eras

import os
import glob as glob

process = cms.Process('Raw',eras.Run3_2023)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("RecoLocalTracker.SiStripRecHitConverter.StripCPEESProducer_cfi")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
# Standard offline propagators
process.load("TrackingTools.MaterialEffects.MaterialPropagatorParabolicMf_cff")

options = VarParsing ('analysis')
options.register ('eventsToProcessTxt',
              '',
              VarParsing.multiplicity.singleton,
              VarParsing.varType.string,
              "Events to process text")
options.register ('n',
                  -1, # default value
                  VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.varType.int,          # string, int, bool or float
                  "n")

options.parseArguments()
print('inputFiles:\t', options.inputFiles)
print('eventsToProcessTxt:\t', options.eventsToProcessTxt)
print('outputFile:\t', options.outputFile)

if options.n != -1:
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(options.n)
    )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(['file:'+f for f in options.inputFiles]),
    secondaryFileNames = cms.untracked.vstring(),   
    # eventsToProcess = cms.untracked.VEventRange( list( open(options.eventsToProcessTxt).readlines() ) )
)

process.options = cms.untracked.PSet(

)

#process.source.eventsToProcess = cms.untracked.VEventRange(
#                   *open("eventsToProcess.txt").read().split()
#                                )

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step2 nevts:30'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
if (not os.path.exists('output/')): os.makedirs('output/')

process.outputClusters = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('ZLIB'),
    compressionLevel = cms.untracked.int32(7),
    eventAutoFlushCompressedSize = cms.untracked.int32(31457280),
    fileName = cms.untracked.string('file:{:s}'.format( options.outputFile.replace('.root', '_trimmed.root') ) ),
    outputCommands = cms.untracked.vstring(
    'drop *',   
    'keep *_*siStripClusters*_*_*',
    )
)

process.TFileService = cms.Service("TFileService", fileName=cms.string(options.outputFile))

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '150X_dataRun3_Prompt_v1', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2025_realistic_hi', '')

process.sep19_2_2_dump_raw = cms.EDAnalyzer("nn_tupleProducer_raw",
    siStripClustersTag = cms.InputTag("siStripClusters"),
    tracks = cms.InputTag('generalTracks',"", 'reRECO'),
    hlttracks = cms.InputTag('hltMergedTracks',"", 'reHLT'),
    hltPixeltracks = cms.InputTag('hltPixelTracks', "", 'reHLT'),
    #hltPixelclusters = cms.InputTag('hltSiPixelClusters', "", 'HLTX'),
    #hltPixeltracks = cms.InputTag('pixelTracks', "", 'reRECO'),
    vertex = cms.InputTag("hltPixelVertices","","reHLT")
)

# Path and EndPath definitions
process.analyzer_step = cms.Path(process.sep19_2_2_dump_raw) 
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.outputClusters)

# Schedule definition
process.schedule = cms.Schedule(process.analyzer_step,process.endjob_step, process.output_step)

from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
MassReplaceInputTag(process, new="rawDataMapperByLabel", old="rawDataCollector")

#do not add changes to your config after this point (unless you know what you are doing)
from FWCore.ParameterSet.Utilities import convertToUnscheduled
process=convertToUnscheduled(process)


# Customisation from command line
process.options.numberOfThreads = 50
process.options.numberOfStreams = 0

#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
