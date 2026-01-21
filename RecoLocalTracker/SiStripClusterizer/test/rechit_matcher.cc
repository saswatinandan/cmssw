#include "rechit_matcher.h"

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

  clusterToken_ = consumes<edmNew::DetSetVector<SiStripCluster>>(
   edm::InputTag("siStripClusters", "")
  );

  ptBins_ = conf.getParameter<std::vector<double>>("pt_bins");

  edm::Service<TFileService> fs;

  for (const auto match : {"matched", "unmatched"}) {
     TFileDirectory matched_dir = fs->mkdir(Form("%s", match));
     for (const auto hit : {"rphiHit", "stereoHit"}) {
       TFileDirectory hit_dir = fs->mkdir(Form("%s/%s", match, hit));
       std::string key = Form("%s_%s", match, hit);
       hists_2d[key]["pt_vs_particle_type"] = create_hist(hit_dir, "pt_vs_PDGID", Form("Associated PSimHit pt_vs_particle_type for %s %s with tracker hit", hit, match), ptBins_, 250,0,250);
       hists_1d[key]["dx"] = create_hist(hit_dir, "dx", "|#Delta(hit.x(),PSimHit.x())|", 50, 0., 5.);
     }
  }
  h_count = fs->make<TH1F>("count", "count", 10, 0.5, 10.5);
  h_pt    = create_hist(fs, "pt", ptBins_);
}

rechit_matcher::~rechit_matcher() {
     h_count->Write();
     delete h_count;
}

void rechit_matcher::do_matching(const SiStripRecHit2D& hit, const std::vector<const TrackingRecHit*>& tracker_hits, TrackerHitAssociator& hitAssociator, const std::string type) {

 bool matched = false;
 for ( const auto & tracker_hit : tracker_hits) {
    if (dynamic_cast<const SiStripRecHit1D *>(&(*tracker_hit))) {
       auto tracker_cluster = dynamic_cast<const SiStripRecHit1D *>(&(*tracker_hit))->cluster().get();
       auto hit_cluster     = hit.cluster().get();
       if (tracker_cluster == hit_cluster) {
	    assert(tracker_cluster->charge() == hit_cluster->charge()
	        && tracker_cluster->firstStrip() == hit_cluster->firstStrip()
	        && tracker_cluster->endStrip() == hit_cluster->endStrip()	
	    );
	    matched = true;
       break;
       }
    } // StripHit1D
    else if (dynamic_cast<const SiStripRecHit2D *>(&(*tracker_hit))) {
       auto tracker_cluster = dynamic_cast<const SiStripRecHit2D *>(&(*tracker_hit))->cluster().get();
       auto hit_cluster     = hit.cluster().get();
       if (tracker_cluster == hit_cluster) {
            assert(tracker_cluster->charge() == hit_cluster->charge()
                && tracker_cluster->firstStrip() == hit_cluster->firstStrip()
                && tracker_cluster->endStrip() == hit_cluster->endStrip()
            );
	    matched = true;
        break;
       }
    } // StripHit2D
    else if (dynamic_cast<const SiStripMatchedRecHit2D *>(&(*tracker_hit))) {
      assert(0);
    }
  } // tracker_hits

  std::vector<PSimHit> simHits =
          hitAssociator.associateHit(hit);
  float min_dx = 99;
  PSimHit* mhit = NULL;
  for (auto& simHit : simHits) {
	float dx = abs(simHit.localPosition().x() - hit.localPosition().x());
	if (dx < min_dx) {
           min_dx = dx;
           mhit   = &simHit;
	   //std::cout << "particletpe: " << mhit.particleType() << std::endl;
	}
  } // simHits
  
  if (mhit) {
    if ( matched ) {
	std::string key = Form("matched_%s", type.c_str());
	fillWithOverFlow(hists_2d[key]["pt_vs_particle_type"], mhit->energyLoss()*100, abs(mhit->particleType()));
	fillWithOverFlow(hists_1d[key]["dx"], min_dx);
    }
    else {
	std::string key = Form("unmatched_%s", type.c_str());
	fillWithOverFlow(hists_2d[key]["pt_vs_particle_type"], mhit->energyLoss()*100, abs(mhit->particleType()));
        fillWithOverFlow(hists_1d[key]["dx"], min_dx);
    }
  }

  if (type=="rphiHit") {
    h_count->Fill(1);
    if (matched) h_count->Fill(2);
    if (simHits.size() == 0) h_count->Fill(3);
  }
  else {
    assert(type=="stereoHit");
    h_count->Fill(4);
    if (matched) h_count->Fill(5);
    if (simHits.size() == 0) h_count->Fill(6);
  }

}

void rechit_matcher::analyze(const edm::Event& event, const edm::EventSetup& setup) {

  // Build associator (config must enable strip association)
  TrackerHitAssociator hitAssociator(event, trackerHitAssociatorConfig_);

  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D>> rphiHandle;
  event.getByToken(rphiToken_, rphiHandle);
  
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D>> stereoHandle;
  event.getByToken(stereoToken_, stereoHandle);

  edm::Handle<edmNew::DetSetVector<SiStripCluster>> clusterHandle;
  event.getByToken(clusterToken_, clusterHandle);

  if (!rphiHandle.isValid() || !stereoHandle.isValid() || !clusterHandle.isValid()) return;

  const auto& tracksHandle = event.getHandle(tracksToken_);

  if (!tracksHandle.isValid()) {
	edm::LogError("flatNtuple_producer") << "No valid track collection found";
	return;
  }
  
  const reco::TrackCollection* tracks = tracksHandle.product();

  std::map<DetId, std::vector<const TrackingRecHit*>> tracker_hits_detIds;
  for(unsigned int i=0; i<tracks->size(); i++) {
     auto& trk = tracks->at(i);
     h_pt->Fill(trk.pt());
     for (auto ih = trk.recHitsBegin(); ih != trk.recHitsEnd(); ih++) {
         const TrackingRecHit& hit = **ih;
         const DetId detId((hit).geographicalId());
         if (detId.det() == DetId::Tracker) {
           if (detId.subdetId() == kBPIX || detId.subdetId() == kFPIX) continue;  // pixel is always 2D
           else {        // should be SiStrip now
               tracker_hits_detIds[detId].push_back(&hit);
           }
	 } // detId
     } // recHit
  } // tracks

  for (const auto& detSet : *rphiHandle) {
     uint32_t detId  = detSet.id();
     const auto tracker_hits = tracker_hits_detIds[detId];
     for (const auto& rhit : detSet) {
	 do_matching(rhit, tracker_hits, hitAssociator, "rphiHit");
      // -------------------------------------
     } // detSet
  } // rphiHandle

  int total_rphi = std::accumulate(
		      rphiHandle->begin(),
		      rphiHandle->end(),
		      size_t(0),
		      [](size_t sum, const edmNew::DetSet<SiStripRecHit2D>& detSet) {
                      return sum + detSet.size();
                      }
		  );
  for (const auto& detSet : *stereoHandle) {
     uint32_t detId  = detSet.id();
     const auto tracker_hits = tracker_hits_detIds[detId];
     for (const auto& stereohit : detSet) {
         do_matching(stereohit, tracker_hits, hitAssociator, "stereoHit");
      // -------------------------------------
     } //detSet
  } // stereohandle

  int total_stereo = std::accumulate(
                      stereoHandle->begin(),
                      stereoHandle->end(),
                      size_t(0),
                      [](size_t sum, const edmNew::DetSet<SiStripRecHit2D>& detSet) {
                      return sum + detSet.size();
                      }
                  );
  int total_cluster = std::accumulate(
                      clusterHandle->begin(),
                      clusterHandle->end(),
                      size_t(0),
                      [](size_t sum, const edmNew::DetSet<SiStripCluster>& detSet) {
                      return sum + detSet.size();
                      }
                  );
  assert(total_cluster == (total_rphi+total_stereo));
}

void rechit_matcher::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(rechit_matcher);
