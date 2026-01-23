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

  genparticlesToken_  = consumes<reco::GenParticleCollection>(
   edm::InputTag("genParticles", "")
  );

  clusterToken_ = consumes<edmNew::DetSetVector<SiStripCluster>>(
   edm::InputTag("siStripClusters", "")
  );

  ptBins_ = conf.getParameter<std::vector<double>>("pt_bins");
  pBins_  = conf.getParameter<std::vector<double>>("p_bins");

  edm::Service<TFileService> fs;

  double arr[] = {0.0,0.005,0.01,0.014,0.018,0.022,0.026,0.03,0.034,0.038,0.042,0.045,0.05};

  for (const auto match : {"matched", "unmatched"}) {
     TFileDirectory matched_dir = fs->mkdir(Form("%s", match));
     for (const auto hit : {"rphiHit", "stereoHit"}) {
       TFileDirectory hit_dir = fs->mkdir(Form("%s/%s", match, hit));
       std::string key = Form("%s_%s", match, hit);
       hists_2d[key]["p_vs_particle_type"] = create_hist(hit_dir, "p_vs_PDGID", Form("Associated PSimHit p_vs_particle_type for %s %s with tracker hit", hit, match), pBins_, 250,0,250, "p() of genPartcle()", "PDGID");
       std::vector<double> particle_bins;
       std::vector<std::string> particles{"other", "electron", "muon", "pion"};
       for (auto& particle : particles) {
         if (particle != "pion") particle_bins = {0,0.02,0.04,0.06,0.08,1.,1.2,1.4,1.6,1.8,2,2.2,2.4,2.6,2.8,5};
	 else particle_bins = pBins_; 
	 hists_2d[key][Form("%s_p_vs_energyloss", particle.c_str())] = create_hist(hit_dir, Form("%s_p_vs_energyloss", particle.c_str()), Form("Associated PSimHit p_vs_eneryloss for %s %s %s with tracker hit", particle.c_str(), hit, match), particle_bins, arr, "p() of genParticle", "energyLoss of genParticle");
         hists_2d[key][Form("%s_cluster", particle.c_str())] = create_hist(hit_dir, Form("%s_cluster", particle.c_str()), Form("strip vs ADC count for %s %s from %s", match, particle.c_str(), hit), 10, 0.5, 10.5, 256, -0.5, 255.5, "strip idx", "ADC count");
       }
       hists_1d[key]["dx"] = create_hist(hit_dir, "dx", "|#Delta(hit.x(),PSimHit.x())|", 50, 0., 5.);
     }
  }
  h_count = fs->make<TH1F>("count", "count", 10, 0.5, 10.5);
  h_pt    = create_hist(fs, "pt", ptBins_);
  h_gen_p_vs_pt = create_hist(fs, "p_vs_pt", "p vs pt for gen Pion", pBins_, ptBins_, "p() of genParticle", "pT of genParticle()"); 
}

rechit_matcher::~rechit_matcher() {
     h_count->Write();
     delete h_count;
}

const reco::GenParticle* rechit_matcher::getFirstPion(const reco::GenParticle* pion) {
    const reco::GenParticle* current = pion;
    while (true) {
        if (current->numberOfMothers() == 0) break; // reached top
        const reco::Candidate* mom = current->mother(0);
        int pdg = abs(mom->pdgId());
        if (pdg == 211 || pdg == 111) {
            current = dynamic_cast<const reco::GenParticle*>(mom);
        } else {
            break; // mother is not a pion
        }
    }

    return current;
}

void rechit_matcher::do_matching(const SiStripRecHit2D& hit, const std::vector<const TrackingRecHit*>& tracker_hits, TrackerHitAssociator& hitAssociator, const std::string type) {

 bool matched = false;
 auto hit_cluster = hit.cluster().get();
 for ( const auto & tracker_hit : tracker_hits) {
    if (dynamic_cast<const SiStripRecHit1D *>(&(*tracker_hit))) {
       auto tracker_cluster = dynamic_cast<const SiStripRecHit1D *>(&(*tracker_hit))->cluster().get();
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
	float dx = abs(simHit.exitPoint().x() - hit.localPosition().x());
	if (dx < min_dx) {
           min_dx = dx;
           mhit   = &simHit;
	   //std::cout << "particletpe: " << mhit.particleType() << std::endl;
	}
  } // simHits
  
  if (mhit) {
    assert(mhit->detUnitId() == hit.geographicalId());
    std::string key;
    if ( matched )
	key = Form("matched_%s", type.c_str());
    else
	key = Form("unmatched_%s", type.c_str());
    int pdgid = abs(mhit->particleType()); 
    fillWithOverFlow(hists_2d[key]["p_vs_particle_type"], mhit->pabs(), pdgid);
    fillWithOverFlow(hists_1d[key]["dx"], min_dx);
    int firstStrip = hit_cluster->firstStrip();
    int endStrip   = hit_cluster->endStrip();
    std::string particle;
    if ( pdgid==11 ) particle = "electron";
    else if ( pdgid==13 )  particle = "muon";
    else if ( pdgid==211 ) particle = "pion";
    else                   particle = "other";
    fillWithOverFlow(hists_2d[key][Form("%s_p_vs_energyloss", particle.c_str())], mhit->pabs(), mhit->energyLoss()*100);
    for (int strip = firstStrip; strip < endStrip; ++strip) {
	fillWithOverFlow(hists_2d[key][Form("%s_cluster", particle.c_str())], strip-firstStrip, (*hit_cluster)[strip-firstStrip]);
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
  const auto& genparticlesHandle = event.getHandle(genparticlesToken_);

  if (!tracksHandle.isValid() || !genparticlesHandle.isValid()) {
	edm::LogError("flatNtuple_producer") << "No valid track or genparticle collection found";
	return;
  }
  
  const reco::TrackCollection* tracks = tracksHandle.product();
  const reco::GenParticleCollection* genparticles = genparticlesHandle.product();
  std::vector<const reco::GenParticle*> pions;
  for (unsigned int i=0; i<genparticles->size(); i++) {
    auto genparticle = &genparticles->at(i);
    auto pdgid = abs(genparticle->pdgId());
    if (pdgid == 211 || pdgid == 111) pions.push_back(getFirstPion(genparticle));
  }
  assert(pions.size()==2);
  for ( auto pion : pions) fillWithOverFlow(h_gen_p_vs_pt, pion->p(), pion->pt());

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
