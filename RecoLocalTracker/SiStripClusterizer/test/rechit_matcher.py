#ptBins=0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,2,3,4,5 pBins=0.0,0.2,0.4,0.6,0.8,1,1.2,1.4,1.6,1.8,2,2.2,2.4,2.6,2.8,3
#ptBins=0.0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10 pBins=0.0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,6,7,8,9,10,15,20
#ptBins=0.0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,10.5,11,11.5,12,12.5,13,15 pBins=3,4,5,6,7,8,9,10,11,12,13,14,15,20,25,30,40,50,70,100

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

options.register(
            'pBins',
             [0,0.01,0.02,0.03,0.05,1,2,3,4,5,10],   # default as string
             VarParsing.multiplicity.list,
             VarParsing.varType.float,
             "Comma-separated p bin edges"
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
            pt_bins  = cms.vdouble(*options.ptBins),
            p_bins  = cms.vdouble(*options.pBins)
                        )

# --- Path ---
process.p = cms.Path(process.rechitMatcher)

