import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

import os

process = cms.Process('calo')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

options = VarParsing ('analysis')
options.register ('n',
                  -1, # default value
                  VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.varType.int,          # string, int, bool or float
                  "n")
options.parseArguments()
print('inputFiles:\t', options.inputFiles)
print('outputFile:\t', options.outputFile)

if options.n != -1:
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(options.n)
    )

# process.source = cms.Source("PoolSource",
process.source = cms.Source(
    "NewEventStreamFileReader" if ".dat" in options.inputFiles[0] else "PoolSource",
    fileNames = cms.untracked.vstring(['file:'+f for f in options.inputFiles]),
    #secondaryFileNames = cms.untracked.vstring(),
    # eventsToProcess = cms.untracked.VEventRange( list( open(options.eventsToProcessTxt).readlines() ) )
)
# Output definition
if (not os.path.exists('output/')): os.makedirs('output/')

process.TFileService = cms.Service("TFileService", fileName=cms.string(options.outputFile))

process.flatNtuple = cms.EDAnalyzer('flatNtuple_producer',
              jets   = cms.InputTag("hltAK4CaloJets","","HLTGRun"),
              jets_fromScouting   = cms.InputTag("hltAK4CaloJetsfromScouting","","HLTGRun")
)
process.flatNtuple_path = cms.Path(process.flatNtuple)
process.endjob_step = cms.EndPath(process.endOfProcess)

# Schedule definition
process.schedule = cms.Schedule(process.flatNtuple_path, process.endjob_step)

# Customisation from command line
process.options.numberOfThreads = 1
process.options.numberOfStreams = 0


