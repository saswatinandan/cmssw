#include "MuonAnalyzer.h"

MuonAnalyzer::MuonAnalyzer(const edm::ParameterSet &iConfig)
{
    create_tree();
    muonToken_ = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
    vertexToken_ = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertex"));
    minMass_ = iConfig.getParameter<double>("minMass");
    maxMass_ = iConfig.getParameter<double>("maxMass");
    partMass_ = iConfig.getParameter<double>("partMass");
}

MuonAnalyzer::~MuonAnalyzer() {
  tree->Write();
  delete tree;
}

void MuonAnalyzer::analyze(const edm::Event &evt, const edm::EventSetup &iSetup) {

    edm::Handle<pat::MuonCollection> muons;
    evt.getByToken(muonToken_, muons);
    if (!muons.isValid()) {
       std::cout << "valid muon not found " << std::endl;
       return;
    }
    const auto& vertexHandle = evt.getHandle(vertexToken_);
    if (!vertexHandle.isValid()) {
       edm::LogError("flatNtuple_producer") << "No valid vertex collection found";
       return;
    }
    event = evt.id().event();
    run   = evt.id().run();
    lumi  = evt.id().luminosityBlock();

    if (!(run==398121 && lumi==122 && (event==173120564 || event==173531564 || event==174586220 || event==174928212))) return;
    std::cout << "r:" << 398121 << "l:" << lumi << "e:" << event << std::endl;
    const reco::VertexCollection& vertices = *vertexHandle;
    auto pv = vertices.at(0);
    for(const auto& v : vertices) {
      std::cout << "vertrex: " << v.position().z() << "\t" << v.tracksSize() << "\t" << v.normalizedChi2() << std::endl;  
    }
    std::vector<const pat::Muon*> all_;
    for (collection::const_iterator i = muons->begin(), iE = muons->end(); i != iE; ++i) {
      all_.push_back(&*i);
    }
    std::cout << "all size: " << all_.size() << std::endl;
    auto res = basicCuts(all_, pv);
    std::cout << "size: " << res.size() << std::endl;
    if ( ! (res.size() >= 2) ) return;
    theBestMassPairCombinationMuons(res);
    
}

Muons MuonAnalyzer::basicCuts(const Muons& muons, const reco::Vertex& pv) const {
  Muons result;

  for (Muons::const_iterator it = muons.begin(); it != muons.end(); ++it) {
    const pat::Muon* muonp = *it;
    std::cout << "pt:" << (*it)->pt() << "eta:" << (*it)->eta() << "phi:" << (*it)->phi() << std::endl;
    if (!muonp->isTrackerMuon() || abs(muonp->eta() > 2.4)) continue;
    const reco::Track &trk = *(muonp->innerTrack());
    double dz = trk.dz(pv.position());
    std::cout << "pv,x:" << pv.position().x() << "\t" << "y:" << pv.position().y() << "\t" << pv.position().z() << std::endl;
    double dxy = trk.dxy(pv.position());
    if( !(dz < 20 && dxy < 0.3) ) continue;
    std::cout << "Besttrack,pt: " << muonp->muonBestTrack()->pt() << "\tpx:" << muonp->muonBestTrack()->px() << "\tpy:" << muonp->muonBestTrack()->py() << "\tdxy:" << std::abs(muonp->muonBestTrack()->dxy(pv.position())) << "\tdz:" << std::abs(muonp->muonBestTrack()->dz(pv.position())) << std::endl;
    if (!muonp->isTightMuon(pv)) continue;
    result.push_back(muonp);
    }
  return result;
}

void MuonAnalyzer::theBestMassPairCombinationMuons(const Muons& muons) {
  Muons sortedMuons = muons;
  TLorentzVector mu1, mu2, pair;
  double mass;
  // sort in pt
  std::sort(sortedMuons.begin(), sortedMuons.end(), ptComparator);
  bool foundmuonpair(false);
  double diff_mass = 99;
  for (Muons::const_iterator it1 = sortedMuons.begin(); it1 != sortedMuons.end(); ++it1) {
    for (Muons::const_iterator it2 = it1 + 1; it2 != sortedMuons.end(); ++it2) {
      if ( (*it1)->charge() * (*it2)->charge() >= 0 ) continue;
      /*double dx = (*it1)->innerTrack().vx() - (*it2)->innerTrack().vx();
      double dy = (*it1)->innerTrack().vy() - (*it2)->innerTrack().vy();
      if ( TMath::Sqrt( pow(dx,2) + pow(dy,2) ) > ) continue;*/
      mu1 = TLorentzVector((*it1)->momentum().x(), (*it1)->momentum().y(), (*it1)->momentum().z(), (*it1)->p());
      mu2 = TLorentzVector((*it2)->momentum().x(), (*it2)->momentum().y(), (*it2)->momentum().z(), (*it2)->p());
      pair = mu1 + mu2;
      mass = pair.M();

     if (mass < maxMass_ && mass > minMass_) {//100,3.5 && mass > 80,2.6) {
/*	if (abs(mass-partMass_) < diff_mass) { //91,3.09
          diff_mass = abs(mass-partMass_); // 3.09,91*/
	part_mass = mass;
        auto trk1 = (*it1)->track();
	auto trk2 = (*it2)->track();
	std::cout << "final pt:" << (*it1)->pt() << "eta:" << (*it1)->eta() << "phi:" << (*it1)->phi() << std::endl;
	std::cout << "final pt:" << (*it2)->pt() << "eta:" << (*it2)->eta() << "phi:" << (*it2)->phi() << std::endl;
	pt[0] = trk1->pt();
        pt[1] = trk2->pt();
        eta[0] = trk1->eta();
        eta[1] = trk2->eta();
	phi[0] = trk1->phi();
        phi[1] = trk2->phi();
	numberOfValidhits[0] = 0;
        /*for (const auto& hit : trk1->recHits()){
          if (!hit->isValid()) continue;
	  auto detid = hit->geographicalId();
          if ( detid.subdetId() == kBPIX || detid.subdetId() == kFPIX ) continue;
          numberOfValidhits[0] += 1;
	}
        numberOfValidhits[1] = 0;
        for (const auto& hit : trk2->recHits()){
          if (!hit->isValid()) continue;
          auto detid = hit->geographicalId();
          if ( detid.subdetId() == kBPIX || detid.subdetId() == kFPIX ) continue;
          numberOfValidhits[1] += 1;
        }*/
	foundmuonpair = true;
	break;
	}
     }
     if (foundmuonpair) break;
    }
  //}
  //break;
   //  }
  // }
   if (foundmuonpair) {
     tree->Fill();
     //break;
   }
 //}
}

DEFINE_FWK_MODULE(MuonAnalyzer);

