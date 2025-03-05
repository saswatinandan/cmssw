# hltGetConfiguration /users/vmuralee/PREmenu/V9 --globaltag 140X_dataRun3_HLT_for2024TSGStudies_v1 --data --unprescale --max-events 100 --eras Run3 --input /store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root

# /users/vmuralee/PREmenu/V9 (CMSSW_14_0_11)

import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_cff import Run3
process = cms.Process( "HLTX", Run3 )

process.HLTConfigVersion = cms.PSet(
  tableName = cms.string("/users/vmuralee/PREmenu/V9")
)

process.HLTSiStripClusterChargeCutNone = cms.PSet(  value = cms.double( -1.0 ) )
process.HLTSiStripClusterChargeCutTight = cms.PSet(  value = cms.double( 1945.0 ) )
process.streams = cms.PSet(  PhysicsHIPhysicsRawPrime0 = cms.vstring( 'HIPhysicsRawPrime' ) )
process.datasets = cms.PSet(  HIPhysicsRawPrime = cms.vstring( 'HLT_HIL1NotBptxOR_v8' ) )

process.GlobalTag = cms.ESSource( "PoolDBESSource",
    DBParameters = cms.PSet( 
      authenticationPath = cms.untracked.string( "." ),
      messageLevel = cms.untracked.int32( 0 ),
      connectionTimeout = cms.untracked.int32( 0 ),
      connectionRetrialTimeOut = cms.int32( 60 ),
      idleConnectionCleanupPeriod = cms.int32( 10 ),
      enableReadOnlySessionOnUpdateConnection = cms.bool( False ),
      enablePoolAutomaticCleanUp = cms.bool( False ),
      enableConnectionSharing = cms.bool( True )
    ),
    connect = cms.string( "frontier://FrontierProd/CMS_CONDITIONS" ),
    globaltag = cms.string( "132_dataRun3_HLT_v2" ),
    snapshotTime = cms.string( "" ),
    toGet = cms.VPSet( 
      cms.PSet(  refreshTime = cms.int64( 2 ),
        record = cms.string( "BeamSpotOnlineLegacyObjectsRcd" )
      ),
      cms.PSet(  refreshTime = cms.int64( 2 ),
        record = cms.string( "BeamSpotOnlineHLTObjectsRcd" )
      )
    ),
    DumpStat = cms.untracked.bool( False ),
    ReconnectEachRun = cms.untracked.bool( True ),
    RefreshAlways = cms.untracked.bool( False ),
    RefreshEachRun = cms.untracked.bool( True ),
    RefreshOpenIOVs = cms.untracked.bool( False )
)
process.GlobalParametersRcdSource = cms.ESSource( "EmptyESSource",
    recordName = cms.string( "L1TGlobalParametersRcd" ),
    iovIsRunNotTime = cms.bool( True ),
    firstValid = cms.vuint32( 1 )
)

process.hltOnlineBeamSpotESProducer = cms.ESProducer( "OnlineBeamSpotESProducer",
  timeThreshold = cms.int32( 48 ),
  sigmaZThreshold = cms.double( 2.0 ),
  sigmaXYThreshold = cms.double( 4.0 ),
  appendToDataLabel = cms.string( "" )
)
process.ClusterShapeHitFilterESProducer = cms.ESProducer( "ClusterShapeHitFilterESProducer",
  PixelShapeFile = cms.string( "RecoTracker/PixelLowPtUtilities/data/pixelShapePhase1_noL1.par" ),
  PixelShapeFileL1 = cms.string( "RecoTracker/PixelLowPtUtilities/data/pixelShapePhase1_loose.par" ),
  ComponentName = cms.string( "ClusterShapeHitFilter" ),
  isPhase2 = cms.bool( False ),
  doPixelShapeCut = cms.bool( True ),
  doStripShapeCut = cms.bool( True ),
  clusterChargeCut = cms.PSet( 
    value = cms.double( -1.0 ),
    refToPSet_ = cms.string( "HLTSiStripClusterChargeCutTight" )
  ),
  appendToDataLabel = cms.string( "" )
)
process.VolumeBasedMagneticFieldESProducer = cms.ESProducer( "VolumeBasedMagneticFieldESProducerFromDB",
  debugBuilder = cms.untracked.bool( False ),
  valueOverride = cms.int32( -1 )
)
process.ParametrizedMagneticFieldProducer = cms.ESProducer( "AutoParametrizedMagneticFieldProducer",
  version = cms.string( "Parabolic" ),
  label = cms.untracked.string( "ParabolicMf" ),
  valueOverride = cms.int32( -1 )
)
process.trackerTopology = cms.ESProducer( "TrackerTopologyEP",
  appendToDataLabel = cms.string( "" )
)
process.sistripconn = cms.ESProducer( "SiStripConnectivity" )
process.siStripLorentzAngleDepESProducer = cms.ESProducer( "SiStripLorentzAngleDepESProducer",
  LatencyRecord = cms.PSet( 
    record = cms.string( "SiStripLatencyRcd" ),
  ),
  LorentzAnglePeakMode = cms.PSet( 
    record = cms.string( "SiStripLorentzAngleRcd" ),
    label = cms.untracked.string( "peak" )
  ),
  LorentzAngleDeconvMode = cms.PSet( 
    record = cms.string( "SiStripLorentzAngleRcd" ),
    label = cms.untracked.string( "deconvolution" )
  )
)
process.siStripBackPlaneCorrectionDepESProducer = cms.ESProducer( "SiStripBackPlaneCorrectionDepESProducer",
  LatencyRecord = cms.PSet( 
    record = cms.string( "SiStripLatencyRcd" ),
  ),
  BackPlaneCorrectionPeakMode = cms.PSet( 
    record = cms.string( "SiStripBackPlaneCorrectionRcd" ),
    label = cms.untracked.string( "peak" )
  ),
  BackPlaneCorrectionDeconvMode = cms.PSet( 
    record = cms.string( "SiStripBackPlaneCorrectionRcd" ),
    label = cms.untracked.string( "deconvolution" )
  )
)
process.TrackerGeometricDetESModule = cms.ESProducer( "TrackerGeometricDetESModule",
  fromDDD = cms.bool( False ),
  fromDD4hep = cms.bool( False ),
  appendToDataLabel = cms.string( "" )
)
process.TrackerDigiGeometryESModule = cms.ESProducer( "TrackerDigiGeometryESModule",
  appendToDataLabel = cms.string( "" ),
  fromDDD = cms.bool( True ),
  applyAlignment = cms.bool( True ),
  alignmentsLabel = cms.string( "" )
)
process.SiStripRegionConnectivity = cms.ESProducer( "SiStripRegionConnectivity",
  EtaDivisions = cms.untracked.uint32( 20 ),
  PhiDivisions = cms.untracked.uint32( 20 ),
  EtaMax = cms.untracked.double( 2.5 )
)
process.SiStripRecHitMatcherESProducer = cms.ESProducer( "SiStripRecHitMatcherESProducer",
  ComponentName = cms.string( "StandardMatcher" ),
  NSigmaInside = cms.double( 3.0 ),
  PreFilter = cms.bool( False )
)
process.SiStripQualityESProducer = cms.ESProducer( "SiStripQualityESProducer",
  appendToDataLabel = cms.string( "" ),
  ListOfRecordToMerge = cms.VPSet( 
    cms.PSet(  record = cms.string( "SiStripDetVOffRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripDetCablingRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "RunInfoRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadStripRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadChannelRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadFiberRcd" ),
      tag = cms.string( "" )
    ),
    cms.PSet(  record = cms.string( "SiStripBadModuleRcd" ),
      tag = cms.string( "" )
    )
  ),
  ReduceGranularity = cms.bool( False ),
  ThresholdForReducedGranularity = cms.double( 0.3 ),
  PrintDebugOutput = cms.bool( False ),
  UseEmptyRunInfo = cms.bool( False )
)
process.SiStripGainESProducer = cms.ESProducer( "SiStripGainESProducer",
  appendToDataLabel = cms.string( "" ),
  printDebug = cms.untracked.bool( False ),
  AutomaticNormalization = cms.bool( False ),
  APVGain = cms.VPSet( 
    cms.PSet(  Record = cms.string( "SiStripApvGainRcd" ),
      NormalizationFactor = cms.untracked.double( 1.0 )
    ),
    cms.PSet(  Record = cms.string( "SiStripApvGain2Rcd" ),
      NormalizationFactor = cms.untracked.double( 1.0 )
    )
  )
)
process.SiStripClusterizerConditionsESProducer = cms.ESProducer( "SiStripClusterizerConditionsESProducer",
  QualityLabel = cms.string( "" ),
  Label = cms.string( "" ),
  appendToDataLabel = cms.string( "" )
)
process.SiPixelTemplateStoreESProducer = cms.ESProducer( "SiPixelTemplateStoreESProducer",
  appendToDataLabel = cms.string( "" )
)
process.GlobalParameters = cms.ESProducer( "StableParametersTrivialProducer",
  TotalBxInEvent = cms.int32( 5 ),
  NumberPhysTriggers = cms.uint32( 512 ),
  NumberL1Muon = cms.uint32( 8 ),
  NumberL1EGamma = cms.uint32( 12 ),
  NumberL1Jet = cms.uint32( 12 ),
  NumberL1Tau = cms.uint32( 8 ),
  NumberChips = cms.uint32( 1 ),
  PinsOnChip = cms.uint32( 512 ),
  OrderOfChip = cms.vint32( 1 ),
  NumberL1IsoEG = cms.uint32( 4 ),
  NumberL1JetCounts = cms.uint32( 12 ),
  UnitLength = cms.int32( 8 ),
  NumberL1ForJet = cms.uint32( 4 ),
  IfCaloEtaNumberBits = cms.uint32( 4 ),
  IfMuEtaNumberBits = cms.uint32( 6 ),
  NumberL1TauJet = cms.uint32( 4 ),
  NumberL1Mu = cms.uint32( 4 ),
  NumberConditionChips = cms.uint32( 1 ),
  NumberPsbBoards = cms.int32( 7 ),
  NumberL1CenJet = cms.uint32( 4 ),
  PinsOnConditionChip = cms.uint32( 512 ),
  NumberL1NoIsoEG = cms.uint32( 4 ),
  NumberTechnicalTriggers = cms.uint32( 0 ),
  NumberPhysTriggersExtended = cms.uint32( 0 ),
  WordLength = cms.int32( 0 ),
  OrderConditionChip = cms.vint32( 1 ),
  appendToDataLabel = cms.string( "" )
)

process.hltOnlineBeamSpot = cms.EDProducer( "BeamSpotOnlineProducer",
    changeToCMSCoordinates = cms.bool( False ),
    maxZ = cms.double( 40.0 ),
    setSigmaZ = cms.double( 0.0 ),
    beamMode = cms.untracked.uint32( 11 ),
    src = cms.InputTag( "" ),
    gtEvmLabel = cms.InputTag( "" ),
    maxRadius = cms.double( 2.0 ),
    useTransientRecord = cms.bool( True )
)

process.hltOnlineMetaDigis = cms.EDProducer( "OnlineMetaDataRawToDigi",
    onlineMetaDataInputLabel = cms.InputTag( "rawDataCollector" )
)
process.hltGtStage2Digis = cms.EDProducer( "L1TRawToDigi",
    FedIds = cms.vint32( 1404 ),
    Setup = cms.string( "stage2::GTSetup" ),
    FWId = cms.uint32( 0 ),
    DmxFWId = cms.uint32( 0 ),
    FWOverride = cms.bool( False ),
    TMTCheck = cms.bool( True ),
    CTP7 = cms.untracked.bool( False ),
    MTF7 = cms.untracked.bool( False ),
    InputLabel = cms.InputTag( "rawDataCollector" ),
    lenSlinkHeader = cms.untracked.int32( 8 ),
    lenSlinkTrailer = cms.untracked.int32( 8 ),
    lenAMCHeader = cms.untracked.int32( 8 ),
    lenAMCTrailer = cms.untracked.int32( 0 ),
    lenAMC13Header = cms.untracked.int32( 8 ),
    lenAMC13Trailer = cms.untracked.int32( 8 ),
    debug = cms.untracked.bool( False ),
    MinFeds = cms.uint32( 0 )
)
process.hltGtStage2ObjectMap = cms.EDProducer( "L1TGlobalProducer",
    MuonInputTag = cms.InputTag( 'hltGtStage2Digis','Muon' ),
    MuonShowerInputTag = cms.InputTag( 'hltGtStage2Digis','MuonShower' ),
    EGammaInputTag = cms.InputTag( 'hltGtStage2Digis','EGamma' ),
    TauInputTag = cms.InputTag( 'hltGtStage2Digis','Tau' ),
    JetInputTag = cms.InputTag( 'hltGtStage2Digis','Jet' ),
    EtSumInputTag = cms.InputTag( 'hltGtStage2Digis','EtSum' ),
    EtSumZdcInputTag = cms.InputTag( 'hltGtStage2Digis','EtSumZDC' ),
    CICADAInputTag = cms.InputTag( "" ),
    ExtInputTag = cms.InputTag( "hltGtStage2Digis" ),
    AlgoBlkInputTag = cms.InputTag( "hltGtStage2Digis" ),
    GetPrescaleColumnFromData = cms.bool( False ),
    AlgorithmTriggersUnprescaled = cms.bool( True ),
    RequireMenuToMatchAlgoBlkInput = cms.bool( True ),
    AlgorithmTriggersUnmasked = cms.bool( True ),
    useMuonShowers = cms.bool( True ),
    resetPSCountersEachLumiSec = cms.bool( True ),
    semiRandomInitialPSCounters = cms.bool( False ),
    ProduceL1GtDaqRecord = cms.bool( True ),
    ProduceL1GtObjectMapRecord = cms.bool( True ),
    EmulateBxInEvent = cms.int32( 1 ),
    L1DataBxInEvent = cms.int32( 5 ),
    AlternativeNrBxBoardDaq = cms.uint32( 0 ),
    BstLengthBytes = cms.int32( -1 ),
    PrescaleSet = cms.uint32( 1 ),
    Verbosity = cms.untracked.int32( 0 ),
    PrintL1Menu = cms.untracked.bool( False ),
    TriggerMenuLuminosity = cms.string( "startup" )
)
process.HLTTriggerTypeFilter = cms.EDFilter( "HLTTriggerTypeFilter",
    SelectedTriggerType = cms.int32( 1 )
)
process.hltPreHIL1NotBptxOR = cms.EDFilter( "HLTPrescaler",
    offset = cms.uint32( 0 ),
    L1GtReadoutRecordTag = cms.InputTag( "hltGtStage2Digis" )
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
    RawDigiProducersList = cms.VInputTag( 'hltSiStripRawToDigi:VirginRaw','hltSiStripRawToDigi:ProcessedRaw','hltSiStripRawToDigi:ScopeMode','hltSiStripRawToDigi:ZeroSuppressed' ),
    storeCM = cms.bool( False ),
    fixCM = cms.bool( False ),
    produceRawDigis = cms.bool( False ),
    produceCalculatedBaseline = cms.bool( False ),
    produceBaselinePoints = cms.bool( False ),
    storeInZScollBadAPV = cms.bool( True ),
    produceHybridFormat = cms.bool( False )
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
      clusterChargeCut = cms.PSet(  refToPSet_ = cms.string( "HLTSiStripClusterChargeCutNone" ) ),
      ConditionsLabel = cms.string( "" )
    ),
    DigiProducersList = cms.VInputTag( 'hltSiStripRawToDigi:ZeroSuppressed','hltSiStripZeroSuppression:VirginRaw','hltSiStripZeroSuppression:ProcessedRaw','hltSiStripZeroSuppression:ScopeMode' )
)
process.hltSiStripClusters2ApproxClusters = cms.EDProducer( "SiStripClusters2ApproxClusters",
    inputClusters = cms.InputTag( "hltSiStripClusterizerForRawPrime" ),
    maxSaturatedStrips = cms.uint32( 3 ),
    clusterShapeHitFilterLabel = cms.string( "ClusterShapeHitFilter" ),
    beamSpot = cms.InputTag( "hltOnlineBeamSpot" )
)
process.rawDataRepacker = cms.EDProducer( "RawDataCollectorByLabel",
    verbose = cms.untracked.int32( 0 ),
    RawCollectionList = cms.VInputTag( 'hltSiStripDigiToZSRaw','source','rawDataCollector' )
)
'''
process.rawPrimeDataRepacker = cms.EDProducer( "EvFFEDSelector",
    inputTag = cms.InputTag( "rawDataCollector" ),
    fedList = cms.vuint32( 812, 1023 )
)
'''


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



process.HLTBool = cms.EDFilter( "HLTBool",
    result = cms.bool( True )
)
process.hltDatasetHIPhysicsRawPrime1 = cms.EDFilter( "TriggerResultsFilter",
    usePathStatus = cms.bool( True ),
    hltResults = cms.InputTag( "" ),
    l1tResults = cms.InputTag( "" ),
    l1tIgnoreMaskAndPrescale = cms.bool( False ),
    throw = cms.bool( True ),
    triggerConditions = cms.vstring( 'HLT_HIL1NotBptxOR_v8' )
)
process.hltPreDatasetHIPhysicsRawPrime = cms.EDFilter( "HLTPrescaler",
    offset = cms.uint32( 0 ),
    L1GtReadoutRecordTag = cms.InputTag( "hltGtStage2Digis" )
)

process.hltOutputPhysicsHIPhysicsRawPrime0 = cms.OutputModule( "PoolOutputModule",
    fileName = cms.untracked.string( "outputPhysicsHIPhysicsRawPrime0.root" ),
    compressionAlgorithm = cms.untracked.string( "LZMA" ),
    compressionLevel = cms.untracked.int32( 4 ),
    fastCloning = cms.untracked.bool( False ),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string( "" ),
        dataTier = cms.untracked.string( "RAW" )
    ),
    SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( 'Dataset_HIPhysicsRawPrime' ) ),
     outputCommands = cms.untracked.vstring('drop *',
      'keep *_hltSiStripClusters2ApproxClusters_*_*',
      'keep DetIds_hltSiStripRawToDigi_*_HLTX',
      'keep FEDRawDataCollection_rawPrime*_*_HLTX',
      'keep GlobalObjectMapRecord_hltGtStage2ObjectMap_*_HLTX',
      'keep edmTriggerResults_*_*_HLTX')

    )

process.HLTBeamSpot = cms.Sequence( process.hltOnlineBeamSpot + process.hltOnlineMetaDigis )
process.HLTL1UnpackerSequence = cms.Sequence( process.hltGtStage2Digis + process.hltGtStage2ObjectMap )
process.HLTBeginSequence = cms.Sequence( process.HLTBeamSpot + process.HLTL1UnpackerSequence + process.HLTTriggerTypeFilter )
process.HLTDoSiStripZeroSuppression = cms.Sequence( process.hltSiStripRawToDigi + process.hltSiStripZeroSuppression )
process.HLTDoHIStripZeroSuppressionAndRawPrimeRepacker = cms.Sequence( process.hltSiStripDigiToZSRaw + process.hltSiStripClusterizerForRawPrime + process.hltSiStripClusters2ApproxClusters + process.rawDataRepacker + process.rawPrimeDataRepacker )
process.HLTDoHIStripZeroSuppressionAndRawPrime = cms.Sequence( process.HLTDoSiStripZeroSuppression + process.HLTDoHIStripZeroSuppressionAndRawPrimeRepacker )
process.HLTEndSequence = cms.Sequence( process.HLTBool )
process.HLTDatasetPathBeginSequence = cms.Sequence( process.hltGtStage2Digis )

process.HLT_HIL1NotBptxOR_v8 = cms.Path( process.HLTBeginSequence + process.hltPreHIL1NotBptxOR + process.HLTDoHIStripZeroSuppressionAndRawPrime + process.HLTEndSequence )
process.Dataset_HIPhysicsRawPrime = cms.Path( process.HLTDatasetPathBeginSequence + process.hltDatasetHIPhysicsRawPrime1 + process.hltPreDatasetHIPhysicsRawPrime )
process.PhysicsHIPhysicsRawPrime0Output = cms.FinalPath( process.hltOutputPhysicsHIPhysicsRawPrime0 )


process.schedule = cms.Schedule( *(process.HLT_HIL1NotBptxOR_v8, process.Dataset_HIPhysicsRawPrime, process.PhysicsHIPhysicsRawPrime0Output, ))


# source module (EDM inputs)
process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root',
        #'/store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/229/00000/0fd42df0-c533-49df-ae69-8a7195b340c3.root',
    ),
    inputCommands = cms.untracked.vstring(
        'keep *'
    )
)

# limit the number of events to be processed
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

# enable TrigReport, TimeReport and MultiThreading
process.options.wantSummary = True
process.options.numberOfThreads = 100
process.options.numberOfStreams = 0

# override the GlobalTag, connection string and pfnPrefix
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = '140X_dataRun3_Prompt_v3')

# show summaries from trigger analysers used at HLT
if 'MessageLogger' in process.__dict__:
    process.MessageLogger.TriggerSummaryProducerAOD = cms.untracked.PSet()
    process.MessageLogger.L1GtTrigReport = cms.untracked.PSet()
    process.MessageLogger.L1TGlobalSummary = cms.untracked.PSet()
    process.MessageLogger.HLTrigReport = cms.untracked.PSet()
    process.MessageLogger.FastReport = cms.untracked.PSet()
    process.MessageLogger.ThroughputService = cms.untracked.PSet()

# load the DQMStore and DQMRootOutputModule
process.load( "DQMServices.Core.DQMStore_cfi" )

process.dqmOutput = cms.OutputModule("DQMRootOutputModule",
    fileName = cms.untracked.string("DQMIO.root")
)

process.DQMOutput = cms.FinalPath( process.dqmOutput )
process.schedule.append( process.DQMOutput )

# add specific customizations
_customInfo = {}
_customInfo['menuType'  ]= "GRun"
_customInfo['globalTags']= {}
_customInfo['globalTags'][True ] = "auto:run3_hlt_GRun"
_customInfo['globalTags'][False] = "auto:run3_mc_GRun"
_customInfo['inputFiles']={}
_customInfo['inputFiles'][True]  = "file:RelVal_Raw_GRun_DATA.root"
_customInfo['inputFiles'][False] = "file:RelVal_Raw_GRun_MC.root"
_customInfo['maxEvents' ]=  -1
_customInfo['globalTag' ]= "140X_dataRun3_Prompt_v3"
_customInfo['inputFile' ]=  ['file:/scratch/nandan/inputfile_for_prehlt/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root']#'/store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/229/00000/0fd42df0-c533-49df-ae69-8a7195b340c3.root']
_customInfo['realData'  ]=  True

from HLTrigger.Configuration.customizeHLTforALL import customizeHLTforAll
process = customizeHLTforAll(process,"GRun",_customInfo)

from HLTrigger.Configuration.customizeHLTforCMSSW import customizeHLTforCMSSW
process = customizeHLTforCMSSW(process,"GRun")

# Eras-based customisations
from HLTrigger.Configuration.Eras import modifyHLTforEras
modifyHLTforEras(process)

process.source.inputCommands = cms.untracked.vstring('drop *_*_*_RECO', 'keep FEDRawDataCollection_*_*_*')
