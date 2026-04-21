/*
 * Dump the online (HLT) clusters' hits info
 *    - bkgTree: (approximated) cluster collection that is produced at the HLT level
 *      - The approximated cluster collection is the output of SiStripClusters2ApproxClusters module, with the default value being hltSiStripClusters2ApproxClusters
 *      - If doDumpInputOfSiStripClusters2ApproxClusters,
 *        The input cluster collection of SiStripClusters2ApproxClusters would also be stored out, with the default value being hltSiStripClusterizerForRawPrime
 */
// system includes
#include <memory>
#include <iostream>

// user include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/SiStripCluster/interface/SiStripApproximateClusterCollection.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "CondFormats/SiStripObjects/interface/SiStripNoises.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include "assert.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH1F.h"

const int kBPIX = PixelSubdetector::PixelBarrel;
const int kFPIX = PixelSubdetector::PixelEndcap;

class nn_tupleProducer_raw : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit nn_tupleProducer_raw(const edm::ParameterSet&);
  ~nn_tupleProducer_raw() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void create_tree();
  void initialize_vars();

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  edm::InputTag inputTagApproxClusters;
  edm::InputTag inputTagClusters;

  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
  edm::EDGetTokenT<reco::TrackCollection> pixeltracksToken_;
  // Event Data
  edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>> clusterToken;

  // Event Setup Data
  TTree* tree;
  edm::Service<TFileService> fs;

  edm::EventNumber_t eventN;
  int runN;
  int lumi;

  edm::InputTag beamSpot_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;

  edm::FileInPath fileInPath_;

  edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
  const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttbToken_;
  const TransientTrackBuilder* theTTrackBuilder;
  const edm::ESGetToken<Propagator, TrackingComponentsRecord> propagatorToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> tkGeomToken_;
  const edm::ESGetToken<PixelClusterParameterEstimator, TkPixelCPERecord> pixelCPEToken_;
  const edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> geometryToken_;
  const GlobalTrackingGeometry* geometry_ = nullptr;
  // for approxCluster
  int    size;

  int         charge;
  float       x;
  float       y;
  float       hlttrk_dr_min;

  const static int nMax = 768;
  float       eta;
  float       phi;
  float       adc_std;


  float recotrk_dr_min;
  float pixeltrk_dr_min;
  float pixeltrk_pt;
  float pixeltrk_pterr;
  float pixeltrk_eta;
  float pixeltrk_phi;
  float pixeltrk_dz;
  float pixeltrk_dxy;
  int pixeltrk_validhits;
  float pixeltrk_chi2;
  float pixeltrk_d0sigma;
  float pixeltrk_dzsigma;
  float pixeltrk_qoverp;
  float pixeltrk_qoverperror;
  float dr_reco_vs_pixel;

  float hlttrk_pt;
  float hlttrk_pterr;
  float hlttrk_eta;
  float hlttrk_phi;
  float hlttrk_dz;
  float hlttrk_dxy;
  int   hlttrk_validhits;
  int   hlttrk_validpixelhits;
  float hlttrk_chi2;
  float hlttrk_d0sigma;
  float hlttrk_dzsigma;
  float hlttrk_qoverp;
  float hlttrk_qoverperror;

  float recotrk_pt;
  float recotrk_pterr;
  float recotrk_eta;
  float recotrk_phi;
  float recotrk_dz;
  float recotrk_dxy;
  int   recotrk_validhits;
  int   recotrk_validpixelhits;
  float recotrk_chi2;
  float recotrk_d0sigma;
  float recotrk_dzsigma;
  float recotrk_qoverp;
  float recotrk_qoverperror;

  bool target;

  TH2F* hist_sig;
  TH2F* hist_bkg;
  TH2F* hist_pt_dr;
  TH1F *h_dr, *h_dp;
};

void nn_tupleProducer_raw::create_tree() {

	tree = fs->make<TTree>("tree", "tree");
  tree->Branch("hlttrk_pt", &hlttrk_pt, "hlttrk_pt/F");
  tree->Branch("hlttrk_pterr", &hlttrk_pterr, "hlttrk_pterr/F");
  tree->Branch("hlttrk_eta", &hlttrk_eta, "hlttrk_eta/F");
  tree->Branch("hlttrk_phi", &hlttrk_phi, "hlttrk_phi/F");
  tree->Branch("hlttrk_dz", &hlttrk_dz, "hlttrk_dz/F");
  tree->Branch("hlttrk_dxy", &hlttrk_dxy, "hlttrk_dxy/F");
  tree->Branch("hlttrk_validhits", &hlttrk_validhits, "hlttrk_validhits/I");
  tree->Branch("hlttrk_validpixelhits", &hlttrk_validpixelhits, "hlttrk_validpixelhits/I");
  tree->Branch("hlttrk_chi2", &hlttrk_chi2, "hlttrk_chi2/F");
  tree->Branch("hlttrk_d0sigma", &hlttrk_d0sigma, "hlttrk_d0sigma/F");
  tree->Branch("hlttrk_dzsigma", &hlttrk_dzsigma, "hlttrk_dzsigma/F");
  tree->Branch("hlttrk_qoverp", &hlttrk_qoverp, "hlttrk_qoverp/F");
  tree->Branch("hlttrk_qoverperror", &hlttrk_qoverperror, "hlttrk_qoverperror/F");
  tree->Branch("dr_reco_vs_pixel", &dr_reco_vs_pixel, "dr_reco_vs_pixel/F");  

  tree->Branch("recotrk_pt", &recotrk_pt, "recotrk_pt/F");
  tree->Branch("recotrk_pterr", &recotrk_pterr, "recotrk_pterr/F");
  tree->Branch("recotrk_eta", &recotrk_eta, "recotrk_eta/F");
  tree->Branch("recotrk_phi", &recotrk_phi, "recotrk_phi/F");
  tree->Branch("recotrk_dz", &recotrk_dz, "recotrk_dz/F");
  tree->Branch("recotrk_dxy", &recotrk_dxy, "recotrk_dxy/F");
  tree->Branch("recotrk_validhits", &recotrk_validhits, "recotrk_validhits/I");
  tree->Branch("recotrk_validpixelhits", &recotrk_validpixelhits, "recotrk_validpixelhits/I");
  tree->Branch("recotrk_chi2", &recotrk_chi2, "recotrk_chi2/F");
  tree->Branch("recotrk_d0sigma", &recotrk_d0sigma, "recotrk_d0sigma/F");
  tree->Branch("recotrk_dzsigma", &recotrk_dzsigma, "recotrk_dzsigma/F");
  tree->Branch("recotrk_qoverp", &recotrk_qoverp, "recotrk_qoverp/F");
  tree->Branch("recotrk_qoverperror", &recotrk_qoverperror, "recotrk_qoverperror/F");
  tree->Branch("target", &target, "target/O");
  tree->Branch("recotrk_dr_min", &recotrk_dr_min, "recotrk_dr_min/F");
  tree->Branch("pixeltrk_dr_min", &pixeltrk_dr_min, "pixeltrk_dr_min/F");
  
  tree->Branch("pixeltrk_dr_min", &pixeltrk_dr_min, "pixeltrk_dr_min/F");
  tree->Branch("pixeltrk_validhits", &pixeltrk_validhits, "pixeltrk_validhits/I");
  tree->Branch("pixeltrk_chi2", &pixeltrk_chi2, "pixeltrk_chi2/F");
  tree->Branch("pixeltrk_d0sigma", &pixeltrk_d0sigma, "pixeltrk_d0sigma/F");
  tree->Branch("pixeltrk_dzsigma", &pixeltrk_dzsigma, "pixeltrk_dzsigma/F");
  tree->Branch("pixeltrk_qoverp", &pixeltrk_qoverp, "pixeltrk_qoverp/F");
  tree->Branch("pixeltrk_qoverperror", &pixeltrk_qoverperror, "pixeltrk_qoverperror/F");
  tree->Branch("pixeltrk_pt", &pixeltrk_pt, "pixeltrk_pt/F");
  tree->Branch("pixeltrk_pterr", &pixeltrk_pterr, "pixeltrk_pterr/F");
  tree->Branch("pixeltrk_eta", &pixeltrk_eta, "pixeltrk_eta/F");
  tree->Branch("pixeltrk_phi", &pixeltrk_phi, "pixeltrk_phi/F");
  tree->Branch("pixeltrk_dz", &pixeltrk_dz, "pixeltrk_dz/F");
  tree->Branch("pixeltrk_dxy", &pixeltrk_dxy, "pixeltrk_dxy/F");
  
  tree->Branch("event", &eventN, "event/i");
  tree->Branch("run",   &runN, "run/I");
  tree->Branch("lumi",  &lumi, "lumi/I");

  tree->Branch("size", &size, "size/I");
  tree->Branch("charge", &charge, "charge/I");
  tree->Branch("x", &x, "x/F");
    tree->Branch("y", &y, "y/F");
  tree->Branch("pixeltrk_dr_min", &pixeltrk_dr_min, "pixeltrk_dr_min/F");
  tree->Branch("hlttrk_dr_min", &hlttrk_dr_min, "hlttrk_dr_min/F");

  tree->Branch("eta", &eta, "eta/F");
  tree->Branch("phi", &phi, "phi/F");
}

void nn_tupleProducer_raw::initialize_vars() {

   target = 0;
   x = y = 0;
   pixeltrk_dr_min = recotrk_dr_min = 10000;

}
