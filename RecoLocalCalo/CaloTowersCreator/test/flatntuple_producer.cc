#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/JetReco/interface/CaloJetCollection.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
//#include "TH1.h"
//
// class declaration
//
//ROOT inclusion
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TTree.h"
//#include "TMath.h"
//#include "TList.h"
//#include "TString.h"

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

class flatNtuple_producer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit flatNtuple_producer(const edm::ParameterSet&);
  ~flatNtuple_producer() override;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  // ----------member data ---------------------------
  edm::EDGetTokenT<reco::CaloJetCollection> jetsToken_;
  edm::EDGetTokenT<reco::CaloJetCollection> jetsTokenfromScouting_; 
  TTree* tree;
  edm::Service<TFileService> fs;

  edm::EventNumber_t eventN;
  constexpr static int nMax = 1000;

  uint16_t nJets;
  uint16_t nJetsfromScouting;

  edm::RunNumber_t runN;
  edm::LuminosityBlockNumber_t lumi;

  float jetPt[nMax];
  float jetEta[nMax];
  float jetPhi[nMax];
  float jetMass[nMax];

  float jetPtfromScouting[nMax];
  float jetEtafromScouting[nMax];
  float jetPhifromScouting[nMax];
  float jetMassfromScouting[nMax];

};


flatNtuple_producer::flatNtuple_producer(const edm::ParameterSet& iConfig){
  
  jetsToken_ = consumes<reco::CaloJetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
  jetsTokenfromScouting_ = consumes<reco::CaloJetCollection>(iConfig.getParameter<edm::InputTag>("jets_fromScouting"));
  usesResource("TFileService");

  tree = fs->make<TTree>("tree","tree");
  tree->Branch("event", &eventN, "event/l");
  tree->Branch("run",   &runN, "run/i");
  tree->Branch("lumi",  &lumi, "lumi/i");

  tree->Branch("nJets",  &nJets, "nJets/s");
  tree->Branch("nJetsfromScouting",  &nJetsfromScouting, "nJetsfromScouting/s");

  tree->Branch("jetPt",  jetPt, "jetPt[nJets]/F");
  tree->Branch("jetEta",  jetEta, "jetEta[nJets]/F");
  tree->Branch("jetPhi",  jetPhi, "jetPhi[nJets]/F");
  tree->Branch("jetMass",  jetMass, "jetMass[nJets]/F");

  tree->Branch("jetPtfromScouting",  jetPtfromScouting, "jetPtfromScouting[nJetsfromScouting]/F");
  tree->Branch("jetEtafromScouting",  jetEtafromScouting, "jetEtafromScouting[nJetsfromScouting]/F");
  tree->Branch("jetPhifromScouting",  jetPhifromScouting, "jetPhifromScouting[nJetsfromScouting]/F");
  tree->Branch("jetMassfromScouting",  jetMassfromScouting, "jetMassfromScouting[nJetsfromScouting]/F");

}

flatNtuple_producer::~flatNtuple_producer() = default;

// ------------ method called for each event  ------------
void flatNtuple_producer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  runN = iEvent.id().run();
  eventN = iEvent.id().event();
  lumi = iEvent.id().luminosityBlock();
  const auto& jetsHandle = iEvent.getHandle(jetsToken_);
  const auto& jetsHandlefromScouting = iEvent.getHandle(jetsTokenfromScouting_);
  
  if (!jetsHandle.isValid()) {
    edm::LogError("flatNtuple_producer") << "No valid jet collection found";
    return;
  }

  const reco::CaloJetCollection& jets = *jetsHandle;
  const reco::CaloJetCollection& jetsfromScouting = *jetsHandlefromScouting;

  nJets = jets.size();
  for(int i=0; i<nJets; i++)
  {
     jetPt[i] =  jets.at(i).pt();
     jetEta[i]  = jets.at(i).eta();
     jetPhi[i]  = jets.at(i).phi();
     jetMass[i] = jets.at(i).mass();
  }

  nJetsfromScouting = jetsfromScouting.size();
  for(int i=0; i<nJetsfromScouting; i++)
  {
     jetPtfromScouting[i] =  jetsfromScouting.at(i).pt();
     jetEtafromScouting[i] =  jetsfromScouting.at(i).eta();
     jetPhifromScouting[i] =  jetsfromScouting.at(i).phi();
     jetMassfromScouting[i] =  jetsfromScouting.at(i).mass();
  }

  tree->Fill();
}

//define this as a plug-in
DEFINE_FWK_MODULE(flatNtuple_producer);

