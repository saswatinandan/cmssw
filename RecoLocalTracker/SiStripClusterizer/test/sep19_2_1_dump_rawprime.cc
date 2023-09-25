/*
 * Dump the online (HLT) clusters' hits info
 *    - onlineClusterTree: (approximated) cluster collection that is produced at the HLT level
 *      - The approximated cluster collection is the output of SiStripClusters2ApproxClusters module, with the default value being hltSiStripClusters2ApproxClusters
 *      - If doDumpInputOfSiStripClusters2ApproxClusters,
 *        The input cluster collection of SiStripClusters2ApproxClusters would also be stored out, with the default value being hltSiStripClusterizerForRawPrime
 */
// system includes
#include <memory>
#include <iostream>

// user include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/SiStripCluster/interface/SiStripApproximateCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripApproximateClusterCollection.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

//ROOT inclusion
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TMath.h"
#include "TList.h"
#include "TString.h"

//
// class decleration
//

class sep19_2_1_dump_rawprime : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit sep19_2_1_dump_rawprime(const edm::ParameterSet&);
  ~sep19_2_1_dump_rawprime() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  bool doDumpInputOfSiStripClusters2ApproxClusters;

  edm::InputTag inputTagApproxClusters;
  edm::InputTag inputTagClustersForRawPrime;

  // Event Data
  edm::EDGetTokenT<SiStripApproximateClusterCollection> approxClusterToken;
  edm::EDGetTokenT<edmNew::DetSetVector<SiStripCluster>> clusterForRawPrimeToken;

  // Event Setup Data
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tkGeomToken_;

  TTree* onlineClusterTree;
  edm::Service<TFileService> fs;

  edm::EventNumber_t eventN;
  int runN;
  int lumi;

  // for approxCluster
  uint32_t    detId;
  uint16_t    firstStrip;
  uint16_t    endStrip;
  float       barycenter;
  uint16_t    size;
  int         charge;

  const static int nMax = 30000;
  float       hitX[nMax];
  float       hitY[nMax];
  uint16_t    channel[nMax];
  uint16_t    adc[nMax];

  // for reference of approxCluster
  uint16_t    ref_firstStrip;
  uint16_t    ref_endStrip;
  float       ref_barycenter;
  uint16_t    ref_size;
  int         ref_charge;

  float       ref_hitX[nMax];
  float       ref_hitY[nMax];
  uint16_t    ref_channel[nMax];
  uint16_t    ref_adc[nMax];
};

sep19_2_1_dump_rawprime::sep19_2_1_dump_rawprime(const edm::ParameterSet& conf) {
  inputTagApproxClusters = conf.getParameter<edm::InputTag>("approxSiStripClustersTag");
  approxClusterToken 	 = consumes<SiStripApproximateClusterCollection>(inputTagApproxClusters);
  doDumpInputOfSiStripClusters2ApproxClusters = conf.getParameter<bool>("doDumpInputOfSiStripClusters2ApproxClusters");
  inputTagClustersForRawPrime = conf.getParameter<edm::InputTag>("hltSiStripClusterizerForRawPrimeTag");
  clusterForRawPrimeToken = consumes<edmNew::DetSetVector<SiStripCluster>>(inputTagClustersForRawPrime);

  tkGeomToken_ = esConsumes();

  usesResource("TFileService");


  onlineClusterTree = fs->make<TTree>("onlineClusterTree", "onlineClusterTree");
  onlineClusterTree->Branch("event", &eventN, "event/i");
  onlineClusterTree->Branch("run",   &runN, "run/I");
  onlineClusterTree->Branch("lumi",  &lumi, "lumi/I");

  onlineClusterTree->Branch("detId", &detId, "detId/i");
  onlineClusterTree->Branch("firstStrip", &firstStrip, "firstStrip/s");
  onlineClusterTree->Branch("endStrip", &endStrip, "endStrip/s");
  onlineClusterTree->Branch("barycenter", &barycenter, "barycenter/F");
  onlineClusterTree->Branch("size", &size, "size/s");
  onlineClusterTree->Branch("charge", &charge, "charge/I");

  onlineClusterTree->Branch("x", hitX, "x[size]/F");
  onlineClusterTree->Branch("y", hitY, "y[size]/F");
  onlineClusterTree->Branch("channel", channel, "channel[size]/s");
  onlineClusterTree->Branch("adc", adc, "adc[size]/s");

  if (doDumpInputOfSiStripClusters2ApproxClusters) {
    onlineClusterTree->Branch("ref_firstStrip", &ref_firstStrip, "ref_firstStrip/s");
    onlineClusterTree->Branch("ref_endStrip", &ref_endStrip, "ref_endStrip/s");
    onlineClusterTree->Branch("ref_barycenter", &ref_barycenter, "ref_barycenter/F");
    onlineClusterTree->Branch("ref_size", &ref_size, "ref_size/s");
    onlineClusterTree->Branch("ref_charge", &ref_charge, "ref_charge/I");

    onlineClusterTree->Branch("ref_x", ref_hitX, "ref_x[ref_size]/F");
    onlineClusterTree->Branch("ref_y", ref_hitY, "ref_y[ref_size]/F");
    onlineClusterTree->Branch("ref_channel", ref_channel, "ref_channel[ref_size]/s");
    onlineClusterTree->Branch("ref_adc", ref_adc, "ref_adc[ref_size]/s");
  }
}

sep19_2_1_dump_rawprime::~sep19_2_1_dump_rawprime() = default;

void sep19_2_1_dump_rawprime::analyze(const edm::Event& event, const edm::EventSetup& es) {
  edm::Handle<SiStripApproximateClusterCollection>  approxClusterCollection 	= event.getHandle(approxClusterToken);
  edm::Handle<edmNew::DetSetVector<SiStripCluster>> clusterForRawPrimeCollection = event.getHandle(clusterForRawPrimeToken);


  using namespace edm;

  const auto& tkGeom = &es.getData(tkGeomToken_);
  const auto tkDets = tkGeom->dets();

  for (const auto& detApproxClusters : *approxClusterCollection) {
    eventN = event.id().event();
    runN   = (int) event.id().run();
    lumi   = (int) event.id().luminosityBlock();
    detId  = detApproxClusters.id();


    for (const auto& approxCluster : detApproxClusters) {

      ///// 1. converting approxCluster to stripCluster: for the estimation of firstStrip, endStrip, adc info
      uint16_t nStrips{0};
      const auto& _detId = detId; // for the capture clause in the lambda function
      auto det = std::find_if(tkDets.begin(), tkDets.end(), [_detId](auto& elem) -> bool {
        return (elem->geographicalId().rawId() == _detId);
      });
      const StripTopology& p = dynamic_cast<const StripGeomDetUnit*>(*det)->specificTopology();
      nStrips = p.nstrips() - 1;
      const auto convertedCluster = SiStripCluster(approxCluster, nStrips);

      firstStrip = convertedCluster.firstStrip();
      endStrip   = convertedCluster.endStrip();
      barycenter = convertedCluster.barycenter();
      size       = convertedCluster.size();
      charge     = convertedCluster.charge();

      for (int strip = firstStrip; strip < endStrip+1; ++strip)
      {
        GlobalPoint gp = (tkGeom->idToDet(detId))->surface().toGlobal(p.localPosition((float) strip));

        hitX   [strip - firstStrip] = gp.x();
        hitY   [strip - firstStrip] = gp.y();
        channel[strip - firstStrip] = strip;
        adc    [strip - firstStrip] = convertedCluster[strip - firstStrip];
      }

      if (doDumpInputOfSiStripClusters2ApproxClusters) {
        ///// 2. calculating distance metric (delta barycenter), and finding the reference
        float distance{9999.};
        const SiStripCluster* closestCluster{nullptr};

        for (const auto& detSiStripClusters : *clusterForRawPrimeCollection) // the reference of the approxCluster
        {
          if (detId == detSiStripClusters.detId()) 
          {
            for (const auto& stripCluster: detSiStripClusters) 
            {
              float deltaBarycenter = convertedCluster.barycenter() - stripCluster.barycenter();
              if (std::abs(deltaBarycenter) < distance) 
              {
                closestCluster = &stripCluster;
                distance = std::abs(deltaBarycenter);
              }
            }
          }
        }

        ref_firstStrip = closestCluster->firstStrip();
        ref_endStrip   = closestCluster->endStrip();
        ref_barycenter = closestCluster->barycenter();
        ref_size       = closestCluster->size();
        ref_charge     = closestCluster->charge();

        for (int strip = ref_firstStrip; strip < ref_endStrip+1; ++strip)
        {
          GlobalPoint gp = (tkGeom->idToDet(detId))->surface().toGlobal(p.localPosition((float) strip));

          ref_hitX   [strip - ref_firstStrip] = gp.x();
          ref_hitY   [strip - ref_firstStrip] = gp.y();
          ref_channel[strip - ref_firstStrip] = strip;
          ref_adc    [strip - ref_firstStrip] = (*closestCluster)[strip - ref_firstStrip];
        }
      }
      onlineClusterTree->Fill();
    }
  }
}

void sep19_2_1_dump_rawprime::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("approxSiStripClustersTag", edm::InputTag("hltSiStripClusters2ApproxClusters"));
  desc.add<bool>("doDumpInputOfSiStripClusters2ApproxClusters" , false);
  desc.add<edm::InputTag>("hltSiStripClusterizerForRawPrimeTag", edm::InputTag("hltSiStripClusterizerForRawPrime"));
  descriptions.add("sep19_2_1_dump_rawprime", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(sep19_2_1_dump_rawprime);
