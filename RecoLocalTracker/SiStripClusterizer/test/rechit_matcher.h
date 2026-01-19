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

#include "TH1F.h"

const int kBPIX = PixelSubdetector::PixelBarrel;
const int kFPIX = PixelSubdetector::PixelEndcap;

class rechit_matcher  : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit rechit_matcher(const edm::ParameterSet&);
  ~rechit_matcher() override;

  void do_matching(const SiStripRecHit2D& hit, const std::vector<const TrackingRecHit*>& tracker_hits, TrackerHitAssociator& hitAssociator);
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  TH1F* h_matched_cluster_pt;
  TH1F* h_unmatched_cluster_pt;
  TH1F* h_matched_cluster_particle_type;
  TH1F* h_unmatched_cluster_particle_type;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  TrackerHitAssociator::Config trackerHitAssociatorConfig_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiStripRecHit2D>> rphiToken_;
  edm::EDGetTokenT<edmNew::DetSetVector<SiStripRecHit2D>> stereoToken_;
  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
};
