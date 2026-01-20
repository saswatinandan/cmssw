#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2DCollection.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "DataFormats/TrackerRecHit2D/interface/SiTrackerMultiRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "FWCore/Framework/interface/Event.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "hist_auxiliary.h"

const int kBPIX = PixelSubdetector::PixelBarrel;
const int kFPIX = PixelSubdetector::PixelEndcap;

class rechit_matcher  : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit rechit_matcher(const edm::ParameterSet&);
  ~rechit_matcher() override;

  void do_matching(const SiStripRecHit2D& hit, const std::vector<const TrackingRecHit*>& tracker_hits, TrackerHitAssociator& hitAssociator, const std::string type);
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  TrackerHitAssociator::Config trackerHitAssociatorConfig_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiStripRecHit2D>> rphiToken_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiStripRecHit2D>> stereoToken_;
  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiStripCluster>> clusterToken_;

  std::map<std::string, std::map<std::string, TH2F*>> hists_2d;
  std::map<std::string, std::map<std::string, TH1F*>> hists_1d;
  TH1F *h_count, *h_pt;
  std::vector<double> ptBins_;
};
