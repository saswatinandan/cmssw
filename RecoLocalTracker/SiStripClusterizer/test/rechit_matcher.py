import FWCore.ParameterSet.Config as cms

process = cms.Process("RECHITMATCHER")

# --- Message logger ---
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

# --- Source ---
process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring(
                        'file:singlepion/step3_woPU_0.01to1.root'
                            )
            )

process.TFileService = cms.Service("TFileService", fileName=cms.string("rechit_matcher.root"))
# --- Number of events ---
process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32(10)
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
                        )

# --- Path ---
process.p = cms.Path(process.rechitMatcher)

