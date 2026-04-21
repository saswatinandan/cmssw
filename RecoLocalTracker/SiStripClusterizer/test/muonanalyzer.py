import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from FWCore.PythonUtilities.LumiList import LumiList

process = cms.Process("muonanalyzer")

# Load standard services
process.load("FWCore.MessageService.MessageLogger_cfi")

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
print('outputFile:\t', options.outputFile)

if options.n != -1:
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(options.n)
    )
# Input file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(['file:'+f for f in options.inputFiles] # replace with your EDM file
    #fileNames  = cms.untracked.vstring([f'root://cms-xrd-global.cern.ch/{f}' for f in options.inputFiles])
    #fileNames  = cms.untracked.vstring('root://cms-xrd-global.cern.ch//store/data/Run2025G/EphemeralHLTPhysics4/MINIAOD/PromptReco-v1/000/398/124/00000/28540e27-9ca0-4700-a631-80a10ed29c01.root')
    ),
    # Optional: only run on N events
    # firstRun = cms.untracked.uint32(1),
    # eventsToProcess = cms.untracked.VEventRange('1:1')
)
processedLumis = LumiList(filename='processedLumis.json')
process.source.lumisToProcess = processedLumis.getVLuminosityBlockRange()
# Analyzer module
process.MuonAnalyzer = cms.EDAnalyzer("MuonAnalyzer",
    muons      = cms.InputTag("slimmedMuons","", ""),
    vertex = cms.InputTag("offlineSlimmedPrimaryVertices","",""),
    partMass = cms.double(3.09),
    minMass  = cms.double(2.6),
    maxMass  = cms.double(3.5)
)

process.options.numberOfThreads = 1
process.options.numberOfStreams = 0

# Path
process.p = cms.Path(process.MuonAnalyzer)

# TFileService for output ROOT file
process.TFileService = cms.Service("TFileService",
    fileName = cms.string(options.outputFile)
)

