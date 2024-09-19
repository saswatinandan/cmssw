////// Saswati Nandan, Inida/INFN,Pisa /////
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <functional>
#include <cassert>

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraphErrors.h"
#include "TStopwatch.h"

#include "EvthistManager.h"

using namespace std;

constexpr float cut_dzSig = 3.0;  
constexpr float cut_dxySig = 3.0; 
constexpr float cut_ptRes = 0.10; 
constexpr float cut_chi2 = 0.18;  
constexpr float cut_nhits = 11;

auto deltaR(float e1, float e2, float p1, float p2) {

        auto dp = std::abs(p1 -p2);
        if (dp > float(M_PI))
           dp -= float(2 * M_PI);
        return TMath::Sqrt(pow((e1 - e2), 2) + pow(dp, 2));
}


struct TreeReader{

   TTree* tree = NULL;

   long long nentries = 0;
   UInt_t event = 0;
   int run = 0;
   int lumi = 0;
   int nTrk = 0;

   static constexpr int nMax = 40000;
   static constexpr int nMax_jet = 800;

   float trkPt[nMax] = {0};
   float trkEta[nMax] = {0};
   float trkPhi[nMax] = {0};
   float trkDxy1[nMax] = {0};
   float trkDxyError1[nMax] = {0};
   float trkDz1[nMax] = {0};
   float trkDzError1[nMax] = {0};

   UShort_t trkAlgo[nMax] = {0};
   Int_t trkNHit[nMax] = {0};
   Int_t trkNdof[nMax] = {0};
   Int_t trkNlayer[nMax] = {0};
   float trkChi2[nMax] = {0};
   float trkPtError[nMax] = {0};

   int nJet = 0;

   float jetPt[nMax_jet] = {0};
   float jetEta[nMax_jet] = {0};
   float jetPhi[nMax_jet] = {0};
   float jetMass[nMax_jet] = {0};

   TreeReader(TTree* in_tree):
   tree(in_tree)
   {
     nentries = tree->GetEntries();
     tree->SetBranchAddress("event", &event);
     tree->SetBranchAddress("run",  &run);
     tree->SetBranchAddress("lumi", &lumi);
     tree->SetBranchAddress("nTracks", &nTrk);
     tree->SetBranchAddress("nJets",  &nJet);

     tree->SetBranchAddress("trkPt",  trkPt);
     tree->SetBranchAddress("trkEta", trkEta);
     tree->SetBranchAddress("trkPhi", trkPhi);
     tree->SetBranchAddress("trkDxy1",trkDxy1);
     tree->SetBranchAddress("trkDxyError1", trkDxyError1);
     tree->SetBranchAddress("trkDz1",  trkDz1);
     tree->SetBranchAddress("trkDzError1", trkDzError1);
     tree->SetBranchAddress("trkAlgo",   trkAlgo);
     tree->SetBranchAddress("trkNHit", trkNHit);
     tree->SetBranchAddress("trkNdof", trkNdof);
     tree->SetBranchAddress("trkNlayer",trkNlayer);
     tree->SetBranchAddress("trkChi2",  trkChi2);
     tree->SetBranchAddress("trkPtError", trkPtError);

     tree->SetBranchAddress("jetPt", jetPt);
     tree->SetBranchAddress("jetEta", jetEta);
     tree->SetBranchAddress("jetPhi",jetPhi);
     tree->SetBranchAddress("jetMass", jetMass);

 };
   ~TreeReader()
   {
    delete tree;
   };

};

struct Jet{

        int   idx;
        float pt;
        float eta;
        float phi;
        float mass;

        Jet(): idx(0),
        pt(0), eta(0), phi(0), mass(0) {};

        Jet(const int& in_idx,
            const float& in_pt,
            const float& in_eta,
            const float& in_phi,
            const float& in_mass
           ):
           idx(in_idx),
           pt(in_pt),
           eta(in_eta),
           phi(in_phi),
           mass(in_mass)
        {};
};

struct Track{

        int idx;
        float pt;
        float eta;
        float phi;
        float Dxy1;
        float DxyError1;
        float Dz1;
        float DzError1;

        UShort_t trkAlgo;
        Int_t    trkNHit;
        Int_t    trkNdof;
        Int_t    trkNlayer;
        float    trkChi2;
        float    trkPtError;

        Track(): idx(0),
        pt(0), eta(0), phi(0), Dxy1(0), DxyError1(0),
        Dz1(0), DzError1(0) {};

        Track(int& in_idx,
            const float& in_pt,
            const float& in_eta,
            const float& in_phi,
            const float& in_Dxy1,
            const float& in_DxyError1,
            const float& in_Dz1,
            const float& in_DzError1,
            const UShort_t& in_trkAlgo,
            const Int_t&  in_trkNHit,
            const Int_t&  in_trkNdof,
            const Int_t&  in_trkNlayer,
            const float&  in_trkChi2,
            const float&  in_trkPtError
           ):
           idx(in_idx),
           pt(in_pt),
           eta(in_eta),
           phi(in_phi),
           Dxy1(in_Dxy1),
           DxyError1(in_DxyError1),
           Dz1(in_Dz1),
           DzError1(in_DzError1),
           trkAlgo(in_trkAlgo),
           trkNHit(in_trkNHit),
           trkNdof(in_trkNdof),
           trkNlayer(in_trkNlayer),
           trkChi2(in_trkChi2),
           trkPtError(in_trkPtError)

        {};

};

void event_loop( map< int, map< int, map<int, bool> > >& evtMatchedMap,
                   const TreeReader& treereader,
                   EvthistManager& evthist,
                   map<int, vector<Track> >& r_goodtrk,
                   map<int, vector<Jet> >& r_goodjet){
       
	std::cout << "analyzing " << std::endl;

	for (int idx = 0; idx < treereader.nentries; ++idx) {

		if(idx%1000 == 0) cout << "Scanning raw tracks: " << idx << "/" << treereader.nentries << endl;

		treereader.tree->GetEntry(idx);

		if ( !evtMatchedMap[treereader.run][treereader.lumi][treereader.event]) continue;

		for (int trkIdx = 0; trkIdx < treereader.nTrk; ++trkIdx)
                {

  	          evthist.fill("trk_dzDdzerr", std::abs(treereader.trkDz1[trkIdx]/treereader.trkDzError1[trkIdx]));
  	          evthist.fill("trk_chi2", treereader.trkChi2[trkIdx]/((int) treereader.trkNdof[trkIdx]));
                  evthist.fill("trk_nhits", treereader.trkNHit[trkIdx]);
                  evthist.fill("trk_pterrDpt", std::abs(treereader.trkPtError[trkIdx]/treereader.trkPt[trkIdx]));

                  evthist.fill("cutflow", cuts::nocut);
                  evthist.fill("cutflow", cuts::dzSig);

                  if(treereader.trkChi2[trkIdx]/((int) treereader.trkNdof[trkIdx])
                         /((int) treereader.trkNlayer[trkIdx]) >= cut_chi2) continue;
                  if(std::abs(treereader.trkPtError[trkIdx]/treereader.trkPt[trkIdx]) >= cut_ptRes) continue;
                  if((int) treereader.trkNHit[trkIdx] < cut_nhits) continue;

                  evthist.fill("trk_pt", treereader.trkPt[trkIdx]);
		  evthist.fill("trk_eta", treereader.trkEta[trkIdx]);
		  evthist.fill("trk_dxyDdxyerr", treereader.trkDxy1[trkIdx]/treereader.trkDxyError1[trkIdx]);
                  
                  evthist.fill("trk_eta_phi", treereader.trkEta[trkIdx], treereader.trkPhi[trkIdx]);

                  r_goodtrk[treereader.event].emplace_back(trkIdx, treereader.trkPt[trkIdx],
                       treereader.trkEta[trkIdx], treereader.trkPhi[trkIdx],
                       treereader.trkDxy1[trkIdx], treereader.trkDxyError1[trkIdx],
                       treereader.trkDz1[trkIdx], treereader.trkDzError1[trkIdx],
                       treereader.trkAlgo[trkIdx], treereader.trkNHit[trkIdx],
                       treereader.trkNdof[trkIdx], treereader.trkNlayer[trkIdx],
                       treereader.trkChi2[trkIdx], treereader.trkPtError[trkIdx]);
                }

                     ///// jet ////

                 for (int jetIdx = 0; jetIdx < treereader.nJet; ++jetIdx)
                 {
                    if(treereader.jetPt[jetIdx] > 20 && abs(treereader.jetEta[jetIdx]) < 2.4)
                    {
                       r_goodjet[treereader.event].emplace_back(jetIdx,
                           treereader.jetPt[jetIdx], treereader.jetEta[jetIdx],
                           treereader.jetPhi[jetIdx], treereader.jetMass[jetIdx]);
                    }
                 }
           }
}

class match_obj_histManager
   : public histManagerBase
{
     public:
       match_obj_histManager(const string& obj, const float& in_drcut):
       histManagerBase(obj)
       ,drcut(in_drcut)	{

	 
	  for (const auto match_type: {"matched", "unmatched"})
	  {
            for (const auto var_type: {"pt"})
            {
              for (const auto raw_type: {"r", "rp"})
              {
	        auto key = Form("%s_%s_%s", match_type, var_type, raw_type);
                hists[key] = createhist(Form("%s_%s", base_name.c_str(), key), Form("%s;pt;yield", key), numBins, customBins);
	      } 
	     }
	  }

          hists["deltar"] = createhist(Form("%s_delta_r", base_name.c_str()), "delta_r;delta_r;yield", 50, 0., drcut);
         hists["ratio"] = createhist(Form("%s_ratio", base_name.c_str()), ";Raw_pt/Raw'_pt;yield", 50, 0.95, 1.05);

       };

       void compareMatching()
       {
	
	 map<string, TH1F*> hists_1 = { {"matched_pt", hists["matched_pt_r"]},
                                       {"unmatched_pt", hists["unmatched_pt_r"]} 
      	 };

	 map<string, TH1F*> hists_2 = { {"matched_pt", hists["matched_pt_rp"]},
                                       {"unmatched_pt", hists["unmatched_pt_rp"]}
         };
	 
	 compareDist(hists_1, "raw",
	             hists_2, "rawp",
	             get_base_name()+"_");
	 
	 Plot_single({"deltar", "ratio"});

       };

       const float get_drcut() const
       {
         return drcut;
       }; 

     private:
       float drcut;
};


template<class T>

void do_matching(const map<int, vector<T> > & r_objs, const map<int, vector<T> >& rp_objs,
	        match_obj_histManager & obj_hists
		) 
        {

	  cout << "doing matching " << endl;
	
	  std::map<int, std::vector<int> >matched_trk_p;
	  std::map<int, std::vector<int> >matched_trk;

	  int not_matched_obj_r = 0;
	  int not_matched_obj_rp = 0;

          int total_obj_r(0);
          int total_obj_rp(0);

	  for(auto const & [e_r, objs_r]: r_objs)
	  {
	     for(auto const & obj_r: objs_r)
             {
               ++total_obj_r;
	       int matched_trk_p_idx(-1);
               float drmin = 9999;
	       float matched_pt_rp(-1);

	       for(auto const & [e_rp, objs_rp]: rp_objs)
               {
	          if(e_r != e_rp) continue;
	          for(auto const & obj_rp: objs_rp)
	          {
		      if(matched_trk_p.find(e_rp) != matched_trk_p.end()) {
		         if(std::find(matched_trk_p[e_rp].begin(), matched_trk_p[e_rp].end(), obj_rp.idx) != matched_trk_p[e_rp].end()) continue; // obj_rp already matched with obj_r
		      }
	             auto dr = deltaR(obj_r.eta, obj_rp.eta, obj_r.phi, obj_rp.phi);
		     if (dr < obj_hists.get_drcut() && dr < drmin) {
                        drmin = dr;
		        matched_trk_p_idx = obj_rp.idx;
		        matched_pt_rp = obj_rp.pt;
		     }
	         } // end of obj_rp loop
	       } // end of e_rp loop

	     obj_hists.fill("deltar", drmin);
	    
	     if(matched_trk_p_idx != -1) {
	         if (matched_trk_p.find(e_r) == matched_trk_p.end())
	              matched_trk_p[e_r] = {};
		 obj_hists.fill("matched_pt_r", obj_r.pt);
                 obj_hists.fill("matched_pt_rp", matched_pt_rp);
                 obj_hists.fill("ratio", obj_r.pt/matched_pt_rp);
	         matched_trk_p[e_r].push_back(matched_trk_p_idx);
	     }
	     else {
	        obj_hists.fill("unmatched_pt_r", obj_r.pt);
	        not_matched_obj_r += 1;
	     }
	   } // end of obj_r loop
	  }  // end of e_r loop
        
	  for(auto const & [e_rp, objs_rp]: rp_objs)
          {
	     for( auto const & obj_rp: objs_rp)
             {
                ++total_obj_rp;

                if (matched_trk_p.find(e_rp) != matched_trk_p.end()) {
                    if(std::find(matched_trk_p[e_rp].begin(), matched_trk_p[e_rp].end(), obj_rp.idx) != matched_trk_p[e_rp].end()) continue; // obj_rp is already matched with other obj_r
	        }
	       obj_hists.fill("unmatched_pt_rp", obj_rp.pt);
               not_matched_obj_rp += 1;
            } // end of obj_rp loop
	  } // end of e_rp loop
        
	  cout << setprecision(2);
          cout << Form("total %s in raw: ", obj_hists.get_base_name().c_str()) << total_obj_r << "\t in raw_p: " << total_obj_rp << endl;
          cout << Form("total unmatched %s in raw: ", obj_hists.get_base_name().c_str()) << not_matched_obj_r << "\t in raw_p: " << not_matched_obj_rp << endl;	
          cout << Form("not matched %s: in raw ", obj_hists.get_base_name().c_str()) << (100.*not_matched_obj_r/total_obj_r) << "%\t in raw_p " << (not_matched_obj_rp*100./total_obj_rp) << "%" << endl;

}

int main(int argc, char const *argv[]) { //LHCC_raw_vs_rawprime() {

	std::string expTag = "test";
	map< int, map< int, map<int, bool> > > evtMatchedMap;
	map< int, map< int, map<int, bool> > > evtMap;
 
	TFile* f1                = TFile::Open(argv[1], "read");
        TreeReader treereader_rp ((TTree*) f1->Get("flatNtuple/tree"));
        
	TFile* f2               = TFile::Open(argv[2], "read");
        TreeReader treereader_r ((TTree*) f2->Get("flatNtuple/tree"));

	TFile* f = new TFile("jet_study.root", "recreate"); 

	for (int r_idx = 0; r_idx < treereader_r.nentries; ++r_idx) {
                treereader_r.tree->GetEntry(r_idx);
		evtMatchedMap[treereader_r.run][treereader_r.lumi][treereader_r.event] = 0;
	}

	for (int rp_idx = 0; rp_idx < treereader_rp.nentries; ++rp_idx) {

		treereader_rp.tree->GetEntry(rp_idx);
		
		if (  evtMatchedMap.find(treereader_rp.run) == evtMatchedMap.end() ) continue;
                if (  evtMatchedMap[treereader_rp.run].find(treereader_rp.lumi) == evtMatchedMap[treereader_rp.run].end() ) continue;
		if (  evtMatchedMap[treereader_rp.run][treereader_rp.lumi].find(treereader_rp.event) == evtMatchedMap[treereader_rp.run][treereader_rp.lumi].end() ) continue;

		evtMatchedMap[treereader_rp.run][treereader_rp.lumi][treereader_rp.event] = 1;

	}

	cout << "creating hists for raw " << endl;

	EvthistManager evthist_r("raw");
	match_obj_histManager trk_hists("tracks", 0.05), jet_hists("jets", 0.8);
        
	//// raw ///

	map<int, vector<Track> > r_goodtrk;
        map<int, vector<Jet> > r_goodjet;

	cout << "calling eventloop for raw" << endl;

	event_loop(evtMatchedMap, treereader_r, evthist_r,
		      r_goodtrk, r_goodjet
		     );	      
	
	/////// rawprime ////

	cout << "creating hists for rawp" << endl;

	EvthistManager evthist_rp("rawp");
	map<int, vector<Track> > rp_goodtrk;
       	map<int, vector<Jet> > rp_goodjet;	

	cout << "calling eventloop for rawp" << endl;

	event_loop(evtMatchedMap, treereader_rp, evthist_rp,
                      rp_goodtrk, rp_goodjet
                     );	
	
	cout << "calling matching" << endl;

	do_matching(r_goodtrk, rp_goodtrk,
	            trk_hists		
        );

	do_matching(r_goodjet, rp_goodjet,
                      jet_hists
        );

	cout << "matching done" << endl;

	f->cd();

	evthist_r.write();
	evthist_rp.write();
	evthist_r.compareDist(evthist_rp);

        trk_hists.write();
        trk_hists.compareMatching();

	jet_hists.write();
	jet_hists.compareMatching();

        f->Close();	

	return 0;
}
