#include "rechit_matcher.h"

TH1F* create_hist(TFileDirectory& dir, const std::string& name, const int& nbin, const int& lowbin, const int& highbin) {
   TH1F* h = dir.make<TH1F>(name.c_str(), name.c_str(), nbin, lowbin, highbin);
   return h;
}

template <typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value>>
T
constrainValue(T value,
               T lowerBound,
               T upperBound)
  {
    assert(lowerBound <= upperBound);
    value = std::max(value, lowerBound);
    value = std::min(value, upperBound);
    return value;
}

void fillWithOverflow(TH1 * histogram,
      double x,
      double evtWeight=1.,
      double evtWeightErr=0.)
{
  if(!histogram) assert(0);
  const TAxis * const xAxis = histogram->GetXaxis();
  const int bin = constrainValue(xAxis->FindBin(x), 1, xAxis->GetNbins());
  const double binContent = histogram->GetBinContent(bin);
  const double binError   = histogram->GetBinError(bin);
  histogram->SetBinContent(bin, binContent + evtWeight);
  histogram->SetBinError(bin, std::sqrt(pow(binError,2) + 1));
}

edm::ParameterSet buildConf(){
      edm::ParameterSet ps;
      ps.addParameter<bool>("associateStrip", true);
      ps.addParameter<bool>("associatePixel", false);
      ps.addParameter<bool>("associateRecoTracks", false);
      ps.addParameter<bool>("usePhase2", false);
      ps.addParameter<edm::InputTag>("stripSimLinkSrc", edm::InputTag("simSiStripDigis"));
      ps.addParameter<std::vector<std::string>>("ROUList", {"TrackerHitsTIBLowTof", "TrackerHitsTIBHighTof", "TrackerHitsTIDLowTof", "TrackerHitsTIDHighTof", "TrackerHitsTOBLowTof", "TrackerHitsTOBHighTof", "TrackerHitsTECLowTof", "TrackerHitsTECHighTof"});
      return ps;
}

rechit_matcher::rechit_matcher(const edm::ParameterSet& conf):
 trackerHitAssociatorConfig_(buildConf(), consumesCollector())
{
  rphiToken_ = consumes<edmNew::DetSetVector<SiStripRecHit2D>>(
    edm::InputTag("siStripMatchedRecHits", "rphiRecHit")
  );
  stereoToken_ = consumes<edmNew::DetSetVector<SiStripRecHit2D>>(
    edm::InputTag("siStripMatchedRecHits", "stereoRecHit")
  );

  tracksToken_  = consumes<reco::TrackCollection>(
   edm::InputTag("generalTracks", "")
  );

  edm::Service<TFileService> fs;
  TFileDirectory matched_dir = fs->mkdir("matched");
  TFileDirectory unmatched_dir = fs->mkdir("unmatched");

  h_matched_cluster_pt = create_hist(matched_dir, "matched_cluster_pt", 100, 0, 10);
  h_unmatched_cluster_pt = create_hist(unmatched_dir, "unmatched_cluster_pt", 100, 0, 10);
  h_matched_cluster_particle_type = create_hist(matched_dir, "matched_cluster_particle_type", 300, 0, 300);
  h_unmatched_cluster_particle_type = create_hist(unmatched_dir, "unmatched_cluster_particletype", 300, 0, 300);
}

rechit_matcher::~rechit_matcher() {}  // must define it

void rechit_matcher::do_matching(const SiStripRecHit2D& hit, const std::vector<const TrackingRecHit*>& tracker_hits, TrackerHitAssociator& hitAssociator) {

 bool matched = false;
 for ( const auto & tracker_hit : tracker_hits) {
    if (dynamic_cast<const SiStripRecHit1D *>(&(*tracker_hit))) {
       if (dynamic_cast<const SiStripRecHit1D *>(&(*tracker_hit))->cluster().get() == hit.cluster().get())
	    matched = true;
       break;
       }
    else if (dynamic_cast<const SiStripRecHit2D *>(&(*tracker_hit))) {
       if (dynamic_cast<const SiStripRecHit2D *>(&(*tracker_hit))->cluster().get() == hit.cluster().get())
	       matched = true;
        break;
     }
  }
  std::vector<PSimHit> simHits =
          hitAssociator.associateHit(hit);
  //std::cout << "size: " << simHits.size() << std::endl;
  for (const auto& simHit : simHits) {
        DetId detId(simHit.detUnitId());
        int trackId = simHit.trackId();
        //std::cout << "matched: " << matched << "\t" << "p: " << simHit.pabs() << "\t" << simHit.particleType() << std::endl;
        if ( matched ) {
             fillWithOverflow(h_matched_cluster_pt, simHit.pabs());
             fillWithOverflow(h_matched_cluster_particle_type, simHit.particleType());
        }
        else {
             fillWithOverflow(h_unmatched_cluster_pt, simHit.pabs());
             fillWithOverflow(h_unmatched_cluster_particle_type, simHit.particleType());
        }
     }
}

void rechit_matcher::analyze(const edm::Event& event, const edm::EventSetup& setup) {

  // Build associator (config must enable strip association)
  TrackerHitAssociator hitAssociator(event, trackerHitAssociatorConfig_);

  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D>> rphiHandle;
  event.getByToken(rphiToken_, rphiHandle);
  
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D>> stereoHandle;
  event.getByToken(stereoToken_, stereoHandle);

  if (!rphiHandle.isValid() || !stereoHandle.isValid()) return;

  const auto& tracksHandle = event.getHandle(tracksToken_);

  if (!tracksHandle.isValid()) {
	edm::LogError("flatNtuple_producer") << "No valid track collection found";
	return;
  }
  
  const reco::TrackCollection* tracks = tracksHandle.product();

  // Get siStripMatchedRecHits
  // Loop over all DetIds
  std::map<DetId, std::vector<const TrackingRecHit*>> tracker_hits_detIds;
  for(unsigned int i=0; i<tracks->size(); i++) {
     auto& trk = tracks->at(i);
     for (auto ih = trk.recHitsBegin(); ih != trk.recHitsEnd(); ih++) {
         const TrackingRecHit& hit = **ih;
         const DetId detId((hit).geographicalId());
         if (detId.det() == DetId::Tracker) {
           if (detId.subdetId() == kBPIX || detId.subdetId() == kFPIX) continue;  // pixel is always 2D
           else {        // should be SiStrip now
               tracker_hits_detIds[detId].push_back(&hit);
           }
	  }
	 }
     }
  
  for (const auto& detSet : *rphiHandle) {
     uint32_t detId  = detSet.id();
     const auto tracker_hits = tracker_hits_detIds[detId];
     for (const auto& rhit : detSet) {
	 do_matching(rhit, tracker_hits, hitAssociator);
      // -------------------------------------
     }
  }

  for (const auto& detSet : *stereoHandle) {
     uint32_t detId  = detSet.id();
     const auto tracker_hits = tracker_hits_detIds[detId];
     for (const auto& stereohit : detSet) {
         do_matching(stereohit, tracker_hits, hitAssociator);
      // -------------------------------------
     }
  }
}

void rechit_matcher::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(rechit_matcher);
