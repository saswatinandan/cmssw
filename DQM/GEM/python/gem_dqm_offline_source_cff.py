import FWCore.ParameterSet.Config as cms

from DQM.GEM.GEMDigiSource_cfi import *
from DQM.GEM.GEMRecHitSource_cfi import *
from DQM.GEM.gemEfficiencyAnalyzer_cff import *

GEMDigiSource.runType   = "offline"
GEMRecHitSource.runType = "offline"

gemSources = cms.Sequence(
    GEMDigiSource *
    GEMRecHitSource *
    gemEfficiencyAnalyzerTightGlbSeq *
    gemEfficiencyAnalyzerStaSeq
)
