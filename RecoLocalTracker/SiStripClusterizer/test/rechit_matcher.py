import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

process = cms.Process("RECHITMATCHER")

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')  # pre-registers inputFiles, outputFile, maxEvents
options.register(
            'ptBins',
             [0,0.01,0.02,0.03,0.05,1,2,3,4,5,10],   # default as string
             VarParsing.multiplicity.list,
             VarParsing.varType.float,
             "Comma-separated pt bin edges"
        )

options.parseArguments()

# --- Message logger ---
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# --- Source ---
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(options.inputFiles
                            )
            )

process.TFileService = cms.Service("TFileService", fileName=cms.string(options.outputFile))
# --- Number of events ---
process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32(options.maxEvents)
            )

#pt_bin_edges = [float(x) for x in options.ptBins.split(',')]
# --- Load geometry and global tag ---
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2025_realistic', '')

# --- Your rechit_matcher analyzer ---
process.rechitMatcher = cms.EDAnalyzer(
            "rechit_matcher",
            trackSrc = cms.InputTag("generalTracks"),
            pt_bins  = cms.vdouble(*options.ptBins)
                        )

# --- Path ---
process.p = cms.Path(process.rechitMatcher)

