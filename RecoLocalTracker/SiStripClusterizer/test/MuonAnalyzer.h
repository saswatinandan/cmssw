#pragma once
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

#define MUMASS 0.108
#define JPSI_MASS 2.8

typedef pat::MuonCollection collection;
typedef std::vector<const pat::Muon*> Muons;

const int kBPIX = PixelSubdetector::PixelBarrel;
const int kFPIX = PixelSubdetector::PixelEndcap;

struct ComparePt {
    bool operator()(const pat::Muon* t1, const pat::Muon* t2) const { return t1->pt() > t2->pt(); }
  };
  ComparePt ptComparator;

class MuonAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit MuonAnalyzer(const edm::ParameterSet&);
  ~MuonAnalyzer() override;

  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void create_tree();
  void initialize_vars();
  Muons basicCuts(const Muons& muons, const reco::Vertex& pv) const;
  void theBestMassPairCombinationMuons(const Muons& muons);

private:
  edm::Service<TFileService> fs;
  edm::EDGetTokenT<pat::MuonCollection> muonToken_;
  edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
  double minMass_, maxMass_, partMass_;
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> bFieldToken_;
  TTree* tree;

  unsigned int run;
  unsigned int lumi;
  unsigned long long event;
  float pt[2];
  float eta[2];
  float phi[2];
  int numberOfValidhits[2];
  float part_mass;

};

void MuonAnalyzer::create_tree() {
   
   tree = fs->make<TTree>("tree", "tree");
   tree->Branch("pt", pt, "pt[2]/F");
   tree->Branch("eta", eta, "eta[2]/F");
   tree->Branch("phi", phi, "phi[2]/F");
   tree->Branch("numberOfValidhits", numberOfValidhits, "numberOfValidhits[2]/I");
   tree->Branch("part_mass", &part_mass, "part_mass/F");
   tree->Branch("run", &run, "run/i");
   tree->Branch("lumi", &lumi, "lumi/i");
   tree->Branch("event", &event, "event/l");
}

void MuonAnalyzer::initialize_vars() {
  
}
