# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step2 --process reHLT -s L1REPACK:Full,HLT --conditions auto:run3_hlt --data --eventcontent FEVTDEBUGHLT --datatier FEVTDEBUGHLT --era Run3_pp_on_PbPb_approxSiStripClusters_2025 -n 1 --no_exec
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

#from Configuration.Eras.Era_Run3_pp_on_PbPb_approxSiStripClusters_2025_cff import Run3_pp_on_PbPb_approxSiStripClusters_2025
from Configuration.Eras.Era_Run3_pp_on_PbPb_approxSiStripClusters_2025_cff import Run3_2025
process = cms.Process('reHLT',Run3_2025)
#process = cms.Process('reHLT',Run3_pp_on_PbPb_approxSiStripClusters_2025)
# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.SimL1EmulatorRepack_Full_cff')
process.load('HLTrigger.Configuration.HLT_GRun_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

options = VarParsing('python')
options.parseArguments()

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(options.inputFiles),#/scratch/nandan/inputfile_for_prehlt/ephemeral/6707e891-fda9-4462-9ffb-0b7902e07031_ephemeral_raw.root"),
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
    numberOfThreads = cms.untracked.uint32(4),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step2 nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

process.hltSiStripRawToDigi = cms.EDProducer( "SiStripRawToDigiModule",
    ProductLabel = cms.InputTag( "rawDataCollector" ),
    LegacyUnpacker = cms.bool( False ),
    AppendedBytes = cms.int32( 0 ),
    UseDaqRegister = cms.bool( False ),
    UseFedKey = cms.bool( False ),
    UnpackBadChannels = cms.bool( False ),
    MarkModulesOnMissingFeds = cms.bool( True ),
    TriggerFedId = cms.int32( 0 ),
    UnpackCommonModeValues = cms.bool( False ),
    DoAllCorruptBufferChecks = cms.bool( False ),
    DoAPVEmulatorCheck = cms.bool( False ),
    ErrorThreshold = cms.uint32( 7174 )
)

process.hltSiStripZeroSuppression = cms.EDProducer( "SiStripZeroSuppression",
    Algorithms = cms.PSet(
      doAPVRestore = cms.bool( True ),
      useCMMeanMap = cms.bool( False ),
      PedestalSubtractionFedMode = cms.bool( False ),
      CommonModeNoiseSubtractionMode = cms.string( "IteratedMedian" ),
      CutToAvoidSignal = cms.double( 2.0 ),
      Iterations = cms.int32( 3 ),
      APVInspectMode = cms.string( "BaselineFollower" ),
      ForceNoRestore = cms.bool( False ),
      useRealMeanCM = cms.bool( False ),
      DeltaCMThreshold = cms.uint32( 20 ),
      distortionThreshold = cms.uint32( 20 ),
      Fraction = cms.double( 0.2 ),
      Deviation = cms.uint32( 25 ),
      restoreThreshold = cms.double( 0.5 ),
      nSaturatedStrip = cms.uint32( 2 ),
      APVRestoreMode = cms.string( "BaselineFollower" ),
      nSigmaNoiseDerTh = cms.uint32( 4 ),
      consecThreshold = cms.uint32( 5 ),
      hitStripThreshold = cms.uint32( 40 ),
      nSmooth = cms.uint32( 9 ),
      minStripsToFit = cms.uint32( 4 ),
      ApplyBaselineCleaner = cms.bool( True ),
      CleaningSequence = cms.uint32( 1 ),
      slopeX = cms.int32( 3 ),
      slopeY = cms.int32( 4 ),
      ApplyBaselineRejection = cms.bool( True ),
      MeanCM = cms.int32( 0 ),
      discontinuityThreshold = cms.int32( 12 ),
      lastGradient = cms.int32( 10 ),
      sizeWindow = cms.int32( 1 ),
      widthCluster = cms.int32( 64 ),
      filteredBaselineMax = cms.double( 6.0 ),
      filteredBaselineDerivativeSumSquare = cms.double( 30.0 ),
      SiStripFedZeroSuppressionMode = cms.uint32( 4 ),
      TruncateInSuppressor = cms.bool( True ),
      Use10bitsTruncation = cms.bool( False )
    ),
    RawDigiProducersList = cms.VInputTag( 'hltSiStripRawToDigi:VirginRaw','hltSiStripRawToDigi:ProcessedRaw','hltSiStripRawToDigi:ScopeMode'),#,'hltSiStripRawToDigi:ZeroSuppressed' ),
    storeCM = cms.bool( False ),
    fixCM = cms.bool( False ),
    produceRawDigis = cms.bool( False ),
    produceCalculatedBaseline = cms.bool( False ),
    produceBaselinePoints = cms.bool( False ),
    storeInZScollBadAPV = cms.bool( True ),
    produceHybridFormat = cms.bool( False )
)

process.hltSiStripClusterizerForRawPrime = cms.EDProducer( "SiStripClusterizer",
    Clusterizer = cms.PSet(
      Algorithm = cms.string( "ThreeThresholdAlgorithm" ),
      ChannelThreshold = cms.double( 2.0 ),
      SeedThreshold = cms.double( 3.0 ),
      ClusterThreshold = cms.double( 5.0 ),
      MaxSequentialHoles = cms.uint32( 0 ),
      MaxSequentialBad = cms.uint32( 1 ),
      MaxAdjacentBad = cms.uint32( 0 ),
      MaxClusterSize = cms.uint32( 768 ),
      RemoveApvShots = cms.bool( True ),
      clusterChargeCut = cms.PSet(  refToPSet_ = cms.string( "HLTSiStripClusterChargeCutTight" ) ),
      ConditionsLabel = cms.string( "" )
    ),
    DigiProducersList = cms.VInputTag( 'hltSiStripRawToDigi:ZeroSuppressed','hltSiStripZeroSuppression:VirginRaw','hltSiStripZeroSuppression:ProcessedRaw','hltSiStripZeroSuppression:ScopeMode' )
)   

process.rawDataRepacker = cms.EDProducer( "RawDataCollectorByLabel",
    verbose = cms.untracked.int32( 0 ),
    RawCollectionList = cms.VInputTag( 'rawDataCollector' )  #'hltSiStripDigiToZSRaw','source','rawDataCollector' )
)
process.rawPrimeDataRepacker = cms.EDProducer("EvFFEDExcluder",
    fedsToExclude = cms.vuint32( (
        50, 51, 52, 53, 54,
        55, 56, 57, 58, 59,
        60, 61, 62, 63, 64,
        65, 66, 67, 68, 69,
        70, 71, 72, 73, 74,
        75, 76, 77, 78, 79,
        80, 81, 82, 83, 84,
        85, 86, 87, 88, 89,
        90, 91, 92, 93, 94,
        95, 96, 97, 98, 99,
        100, 101, 102, 103, 104,
        105, 106, 107, 108, 109,
        110, 111, 112, 113, 114,
        115, 116, 117, 118, 119,
        120, 121, 122, 123, 124,
        125, 126, 127, 128, 129,
        130, 131, 132, 133, 134,
        135, 136, 137, 138, 139,
        140, 141, 142, 143, 144,
        145, 146, 147, 148, 149,
        150, 151, 152, 153, 154,
        155, 156, 157, 158, 159,
        160, 161, 162, 163, 164,
        165, 166, 167, 168, 169,
        170, 171, 172, 173, 174,
        175, 176, 177, 178, 179,
        180, 181, 182, 183, 184,
        185, 186, 187, 188, 189,
        190, 191, 192, 193, 194,
        195, 196, 197, 198, 199,
        200, 201, 202, 203, 204,
        205, 206, 207, 208, 209,
        210, 211, 212, 213, 214,
        215, 216, 217, 218, 219,
        220, 221, 222, 223, 224,
        225, 226, 227, 228, 229,
        230, 231, 232, 233, 234,
        235, 236, 237, 238, 239,
        240, 241, 242, 243, 244,
        245, 246, 247, 248, 249,
        250, 251, 252, 253, 254,
        255, 256, 257, 258, 259,
        260, 261, 262, 263, 264,
        265, 266, 267, 268, 269,
        270, 271, 272, 273, 274,
        275, 276, 277, 278, 279,
        280, 281, 282, 283, 284,
        285, 286, 287, 288, 289,
        290, 291, 292, 293, 294,
        295, 296, 297, 298, 299,
        300, 301, 302, 303, 304,
        305, 306, 307, 308, 309,
        310, 311, 312, 313, 314,
        315, 316, 317, 318, 319,
        320, 321, 322, 323, 324,
        325, 326, 327, 328, 329,
        330, 331, 332, 333, 334,
        335, 336, 337, 338, 339,
        340, 341, 342, 343, 344,
        345, 346, 347, 348, 349,
        350, 351, 352, 353, 354,
        355, 356, 357, 358, 359,
        360, 361, 362, 363, 364,
        365, 366, 367, 368, 369,
        370, 371, 372, 373, 374,
        375, 376, 377, 378, 379,
        380, 381, 382, 383, 384,
        385, 386, 387, 388, 389,
        390, 391, 392, 393, 394,
        395, 396, 397, 398, 399,
        400, 401, 402, 403, 404,
        405, 406, 407, 408, 409,
        410, 411, 412, 413, 414,
        415, 416, 417, 418, 419,
        420, 421, 422, 423, 424,
        425, 426, 427, 428, 429,
        430, 431, 432, 433, 434,
        435, 436, 437, 438, 439,
        440, 441, 442, 443, 444,
        445, 446, 447, 448, 449,
        450, 451, 452, 453, 454,
        455, 456, 457, 458, 459,
        460, 461, 462, 463, 464,
        465, 466, 467, 468, 469,
        470, 471, 472, 473, 474,
        475, 476, 477, 478, 479,
        480, 481, 482, 483, 484,
        485, 486, 487, 488, 489
     ) ),
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("rawDataRepacker")
)
process.hltSiStripDigiToZSRaw = cms.EDProducer( "SiStripDigiToRawModule",
    FedReadoutMode = cms.string( "ZERO_SUPPRESSED" ),
    PacketCode = cms.string( "ZERO_SUPPRESSED" ),
    UseFedKey = cms.bool( False ),
    UseWrongDigiType = cms.bool( False ),
    CopyBufferHeader = cms.bool( True ),
    InputDigis = cms.InputTag( 'hltSiStripZeroSuppression','ZeroSuppressed' ),
    RawDataTag = cms.InputTag( "rawDataCollector" )
)

process.hltSiStripClusters2ApproxClusters = cms.EDProducer( "SiStripClusters2ApproxClusters",
    inputClusters = cms.InputTag( "hltSiStripClusterizerForRawPrime" ),
    maxSaturatedStrips = cms.uint32( 3 ),
    clusterShapeHitFilterLabel = cms.string( "ClusterShapeHitFilter" ),
    beamSpot = cms.InputTag( "hltOnlineBeamSpot" ),
    dnnvars  = cms.vstring("size",
        "charge",
        "mean_x",
        "mean_y",
        "mean_z",
        "adc_at_idx_0",
        "adc_at_idx_1",
        "adc_at_idx_2",
        "adc_at_idx_3"
    ),
    dnncutvalue = cms.double(0.328168)
)

process.streams = cms.PSet(  PhysicsHLTPhysics0 = cms.vstring( 'EphemeralHLTPhysics4' ) )
process.datasets = cms.PSet(  EphemeralHLTPhysics4 = cms.vstring( 'HLT_EphemeralPhysics_v10' ) )

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('FEVTDEBUGHLT'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string(options.outputFile),
    outputCommands = cms.untracked.vstring('drop *', 
    'keep *_hltSiStripClusters*_*_*',
    'keep *_hltSiStripClusterizerForRawPrime_*_*',
    'keep *_hltPixelTracks*_*_*',
    'keep *_hltMergedTracks_*_*',
    'keep *_hltPixelVertices*_*_*',
    'keep DetIds_hltSiStripRawToDigi_*_reHLT',
    'keep FEDRawDataCollection_rawPrimeDataRepacker*_*_reHLT',
    #'keep FEDRawDataCollection_rawDataCollector_*_reHLT',
    'keep GlobalObjectMapRecord_hltGtStage2ObjectMap_*_reHLT',
    'keep edmTriggerResults_*_*_reHLT'
    )
    #splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from HLTrigger.Configuration.CustomConfigs import ProcessName
process = ProcessName(process)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run3_hlt', '')

# Path and EndPath definitions
process.L1RePack_step = cms.Path(process.SimL1Emulator)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# Schedule definition
# process.schedule imported from cff in HLTrigger.Configuration
#process.hltSiStripRawToClustersFacility.Clusterizer.clusterChargeCut.refToPSet_ = cms.string('HLTSiStripClusterChargeCutTight')
process.HLTDoSiStripZeroSuppression = cms.Sequence( process.hltSiStripRawToDigi + process.hltSiStripZeroSuppression )

process.HLTDoHIStripZeroSuppressionAndRawPrimeRepacker = cms.Sequence( process.HLTDoSiStripZeroSuppression+process.hltSiStripDigiToZSRaw + process.hltSiStripClusterizerForRawPrime + process.hltSiStripClusters2ApproxClusters + process.rawDataRepacker + process.rawPrimeDataRepacker )
process.HLT_EphemeralPhysics_v10 = cms.Path(process.HLTBeginSequence+process.hltL1sL1APhysicsMaskingL1ZeroBiasCopy+process.hltPreEphemeralPhysics+process.HLTDoHIStripZeroSuppressionAndRawPrimeRepacker+process.HLTEndSequence)
process.schedule = cms.Schedule(*[ process.L1RePack_step, process.HLTriggerFirstPath, process.Status_OnCPU, process.Status_OnGPU,process.HLT_EphemeralPhysics_v10,process.Dataset_EphemeralHLTPhysics4,process.FEVTDEBUGHLToutput_step])
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#process.options.wantSummary = True

# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
