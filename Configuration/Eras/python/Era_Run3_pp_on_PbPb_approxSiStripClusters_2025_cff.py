
import FWCore.ParameterSet.Config as cms
from Configuration.ProcessModifiers.trackdnn_CKF_cff import trackdnn_CKF
from Configuration.ProcessModifiers.trackdnn_cff import trackdnn
from Configuration.Eras.Era_Run3_cff import Run3

Run3_pp_on_PbPb_2025 = cms.ModifierChain(Run3.copyAndExclude([trackdnn_CKF]))
