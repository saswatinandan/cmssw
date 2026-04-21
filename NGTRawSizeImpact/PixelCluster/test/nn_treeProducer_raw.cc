#include "nn_treeProducer_raw.h"

auto deltaR(float e1, float e2, float p1, float p2) {
	                   
       auto dp = std::abs(p1 -p2);
       if (dp > float(M_PI))
           dp -= float(2 * M_PI);
       return std::sqrt(pow((e1 - e2), 2) + pow(dp, 2));
}    

nn_tupleProducer_raw::nn_tupleProducer_raw(const edm::ParameterSet& conf) :
  ttbToken_(esConsumes(edm::ESInputTag("", "TransientTrackBuilder")))
  ,propagatorToken_(esConsumes(edm::ESInputTag("", "PropagatorWithMaterialParabolicMf")))
  ,pixelCPEToken_(esConsumes(edm::ESInputTag("", conf.getParameter<std::string>("pixelCPE"))))
  ,geometryToken_(esConsumes())
  {
  inputTagClusters       = conf.getParameter<edm::InputTag>("pixelClustersTag");
  clusterToken           = consumes<edmNew::DetSetVector<SiPixelCluster>>(inputTagClusters);
  tracksToken_           = consumes<reco::TrackCollection>(conf.getParameter<edm::InputTag>("tracks"));
  pixeltracksToken_           = consumes<reco::TrackCollection>(conf.getParameter<edm::InputTag>("pixeltracks"));

  tkGeomToken_ = esConsumes();
  hist_sig = fs->make<TH2F>("adc_idx_sig","", 20,0,20,260,0,260);
  hist_bkg = fs->make<TH2F>("adc_idx_bkg","", 20,0,20,260,0,260);
  hist_pt_dr = fs->make<TH2F>("pt_dr","", 100,0,2, 50,0,5);
  h_dr = fs->make<TH1F>("dr", "dr", 100,0,1.);
  h_dp = fs->make<TH1F>("dp", "dp", 100,0,20);
  create_tree();
}

nn_tupleProducer_raw::~nn_tupleProducer_raw() = default;

void nn_tupleProducer_raw::analyze(const edm::Event& event, const edm::EventSetup& es) {
  edm::Handle<edmNew::DetSetVector<SiPixelCluster>> clusterCollection = event.getHandle(clusterToken);
  const auto& tracksHandle = event.getHandle(tracksToken_);
  const auto& pixeltracksHandle = event.getHandle(pixeltracksToken_);

  theTTrackBuilder = &es.getData(ttbToken_);
  const Propagator* thePropagator = &es.getData(propagatorToken_);

  const PixelClusterParameterEstimator* pixelCPE = &es.getData(pixelCPEToken_);
  const GlobalTrackingGeometry* geometry_ = &es.getData(geometryToken_);;
 
  if (!tracksHandle.isValid()) {
    edm::LogError("flatNtuple_producer") << "No valid track collection found";
    return;
  }
  if (!pixeltracksHandle.isValid()) {
    edm::LogError("flatNtuple_producer") << "No valid pixeltrack collection found";
    return;
  }

  const reco::TrackCollection* tracks = tracksHandle.product();
  const reco::TrackCollection* pixeltracks = pixeltracksHandle.product();
  std::map<uint32_t, std::vector<const SiPixelCluster*>> matched_cluster;
  for(unsigned int i=0; i<tracks->size(); i++) {
     auto& trk = tracks->at(i);
     for (auto ih = trk.recHitsBegin(); ih != trk.recHitsEnd(); ih++) {
         const SiPixelCluster* pixel=NULL;
         const TrackingRecHit& hit = **ih;
         const DetId detId((hit).geographicalId());
         if (detId.det() == DetId::Tracker) { 
           if ( !(detId.subdetId() == kBPIX || detId.subdetId() == kFPIX) ) continue;  // pixel is always 2D
	   if (dynamic_cast<const SiPixelRecHit *>(&hit)) {
	   pixel = dynamic_cast<const SiPixelRecHit *>(&hit)->cluster().get();; 
	   }
         }
         if(pixel) {
               matched_cluster[detId].push_back(pixel);
         }
     }
  }
  const auto& tkGeom = &es.getData(tkGeomToken_);
  for (const auto& detSiPixelClusters : *clusterCollection) {
    uint32_t detId  = detSiPixelClusters.id(); 
    eventN = event.id().event();
    runN   = (int) event.id().run();
    lumi   = (int) event.id().luminosityBlock();
    std::vector<const SiPixelCluster*> track_clusters = {};
    if ( matched_cluster.find(detId) != matched_cluster.end() ) track_clusters = matched_cluster[detId];
    std::map<reco::TrackRef, TrajectoryStateOnSurface> recotrk_tsosCache;
    std::map<reco::TrackRef, TrajectoryStateOnSurface> pixeltrk_tsosCache;
    bool firstcluster = true;
    const GeomDetUnit* geomDet = tkGeom->idToDetUnit(detId);

    for (const auto& pixelCluster : detSiPixelClusters) {
      initialize_vars();
      size                = pixelCluster.size();
      charge              = pixelCluster.charge();
      x                   = pixelCluster.x();
      y                   = pixelCluster.y();

      for(auto trk_cluster: track_clusters)
        {
           if (trk_cluster == &pixelCluster)
           {
               assert( (size == trk_cluster->size())
                      && (charge == trk_cluster->charge())
               );
	       target = 1;
	       break;
           }
        }

      const reco::Track* recotrk = NULL;
      const reco::Track* pixeltrk = NULL;
      if ( firstcluster ) {
      for ( size_t i=0; i<tracks->size(); i++) {
        reco::TrackRef trackRef(tracks, i);
        TrajectoryStateOnSurface tsos;
        for (auto const& hit : trackRef->recHits()) {
             if (!hit->isValid()) continue;
             if (hit->geographicalId() != detId) continue;
             reco::TransientTrack tkTT = theTTrackBuilder->build(*trackRef);
             tsos = thePropagator->propagate(tkTT.innermostMeasurementState(), geomDet->surface());
             if ( tsos.isValid() ) {
                  recotrk_tsosCache[trackRef] = tsos;
                  break;
             }
	}
       }
      for ( size_t i=0; i<pixeltracks->size(); i++) {
        reco::TrackRef trackRef(pixeltracks, i);
        TrajectoryStateOnSurface tsos;
        reco::TransientTrack tkTT = theTTrackBuilder->build(*trackRef);
          if(!tkTT.impactPointState().isValid()) continue;
	  for (auto const& hit : trackRef->recHits()) {
             if (!hit->isValid()) continue;
             if (hit->geographicalId() != detId) continue;
	     tsos = thePropagator->propagate(tkTT.impactPointState(), geomDet->surface());
	     if( tsos.isValid() ) {
		  pixeltrk_tsosCache[trackRef] = tsos;
                  break;
	     }
	  }
          //tsos = thePropagator->propagate(tkTT.impactPointState(), geomDet->surface());
          //if( tsos.isValid() ) pixeltrk_tsosCache[trackRef] = tsos;
      }
      firstcluster = false;
      }
      LocalPoint clustPos_local = pixelCPE->localParametersV(pixelCluster, (*geometry_->idToDetUnit(detId)))[0].first;
      for (const auto& [trackRef, tsos] : recotrk_tsosCache) {
       LocalPoint trackLocal = geomDet->surface().toLocal(tsos.globalPosition());
       float dr = abs(trackLocal.x() - clustPos_local.x()); //std::sqrt( pow( (trackLocal.x() - clusterLocal.x()), 2 ) +
       if (dr < recotrk_dr_min) {
           recotrk_dr_min = dr;
           recotrk = &(*trackRef);
       }
     }
     if (recotrk) {
	     std::cout << "recotrk_dr_min " << recotrk_dr_min << std::endl;
     }

      for (const auto& [trackRef, tsos] : pixeltrk_tsosCache) {
       LocalPoint trackLocal = geomDet->surface().toLocal(tsos.globalPosition());
       float dr = abs(trackLocal.x() - clustPos_local.x()); //std::sqrt( pow( (trackLocal.x() - clusterLocal.x()), 2 ) +
       if (dr < pixeltrk_dr_min) {
           pixeltrk_dr_min = dr;
           pixeltrk = &(*trackRef);
       }
     }
     if (pixeltrk) {
             std::cout << "pixeltrk_dr_min " << pixeltrk_dr_min << std::endl;
     }

     tree->Fill();
    }
  }
}

void nn_tupleProducer_raw::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("pixelClustersTag", edm::InputTag("pixelClustersTag"));
  desc.add<edm::InputTag>("tracks", edm::InputTag("generalTracks","","reRECO"));
  desc.add<edm::InputTag>("pixeltracks", edm::InputTag("hltPhase2PixelTracks","","HLT"));
  desc.add<std::string>("pixelCPE", "PixelCPEGeneric");
  descriptions.add("nn_tupleProducer_raw", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(nn_tupleProducer_raw);
