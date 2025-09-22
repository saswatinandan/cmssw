////// Saswati Nandan, Inida/INFN,Pisa /////
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <cassert>

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TChain.h"

#include "EvthistManager.h"
#include "Match_obj_histManager.h"
#include "object.h"

using namespace std;

auto deltaR(float e1, float e2, float p1, float p2) {

        auto dp = std::abs(p1 -p2);
        if (dp > float(M_PI))
           dp -= float(2 * M_PI);
        return TMath::Sqrt(pow((e1 - e2), 2) + pow(dp, 2));
}


struct TreeReader{

   TTree* tree = NULL;

   ULong64_t nentries = 0;
   ULong64_t event = 0;
   UInt_t run = 0;
   UInt_t lumi = 0;
   uint16_t nJet = 0;
   uint16_t nJetfromScouting = 0;

   static constexpr int nMax_jet = 1000;

   float jetPt[nMax_jet] = {0};
   float jetEta[nMax_jet] = {0};
   float jetPhi[nMax_jet] = {0};
   float jetMass[nMax_jet] = {0};

   float jetPtfromScouting[nMax_jet] = {0};
   float jetEtafromScouting[nMax_jet] = {0};
   float jetPhifromScouting[nMax_jet] = {0};
   float jetMassfromScouting[nMax_jet] = {0};

   TreeReader(TTree* in_tree):
   tree(in_tree)
   {
     nentries = tree->GetEntries();
     tree->SetBranchAddress("event", &event);
     tree->SetBranchAddress("run",  &run);
     tree->SetBranchAddress("lumi", &lumi);
     tree->SetBranchAddress("nJets",  &nJet);
     tree->SetBranchAddress("nJetsfromScouting",  &nJetfromScouting);

     tree->SetBranchAddress("jetPt", jetPt);
     tree->SetBranchAddress("jetEta", jetEta);
     tree->SetBranchAddress("jetPhi",jetPhi);
     tree->SetBranchAddress("jetMass", jetMass);

     tree->SetBranchAddress("jetPtfromScouting", jetPtfromScouting);
     tree->SetBranchAddress("jetEtafromScouting", jetEtafromScouting);
     tree->SetBranchAddress("jetPhifromScouting",jetPhifromScouting);
     tree->SetBranchAddress("jetMassfromScouting", jetMassfromScouting);

 };
   ~TreeReader()
   {
    delete tree;
   };

};

void event_loop(   const TreeReader& treereader,
                   EvthistManager& evthist,
		   EvthistManager& evthistfromScouting,
                   map<int, vector<Jet> >& jet,
		   map<int, vector<Jet> >& jetfromScouting
		  ){
       
	std::cout << "analyzing " << std::endl;

	for (int idx = 0; idx < treereader.nentries; ++idx) {

	    if(idx%1000 == 0) cout << "Scanning raw tracks: " << idx << "/" << treereader.nentries << endl;

		treereader.tree->GetEntry(idx);


		for (int jetIdx = 0; jetIdx < treereader.nJet; ++jetIdx)
                {

                  evthist.fill("jet_pt", treereader.jetPt[jetIdx]);
		  evthist.fill("jet_eta", treereader.jetEta[jetIdx]);
		  evthist.fill("jet_phi", treereader.jetPhi[jetIdx]);
                  evthist.fill("jet_mass", treereader.jetMass[jetIdx]);
                  jet[treereader.event].emplace_back(jetIdx, treereader.jetPt[jetIdx],
                       treereader.jetEta[jetIdx], treereader.jetPhi[jetIdx],
                       treereader.jetMass[jetIdx]);
                }

                 ///// jetfromScouting ////

                 for (int jetIdx = 0; jetIdx < treereader.nJetfromScouting; ++jetIdx)
                 {

                   evthistfromScouting.fill("jet_pt",  treereader.jetPtfromScouting[jetIdx]);
                   evthistfromScouting.fill("jet_eta", treereader.jetEtafromScouting[jetIdx]);
		   evthistfromScouting.fill("jet_phi",  treereader.jetPhifromScouting[jetIdx]);
                   evthistfromScouting.fill("jet_mass", treereader.jetMassfromScouting[jetIdx]);

                   jetfromScouting[treereader.event].emplace_back(jetIdx,
                      treereader.jetPtfromScouting[jetIdx], treereader.jetEtafromScouting[jetIdx],
                      treereader.jetPhifromScouting[jetIdx], treereader.jetMassfromScouting[jetIdx]);
                 }
           }
}

struct match_property{

float drmin;
float t1_pt;
float t2_pt;
int   t1_idx;
int   t2_idx;

  match_property():
          drmin(-1)
          ,t1_pt(-1)
          ,t2_pt(-1)
          ,t1_idx(-1)
          ,t2_idx(-1)
          {};

  match_property(float in_drmin, float in_t1_pt, float in_t2_pt,
                 int in_t1_idx,  int in_t2_idx
                ):
                drmin(in_drmin)
               ,t1_pt(in_t1_pt)
               ,t2_pt(in_t2_pt)
               ,t1_idx(in_t1_idx)
               ,t2_idx(in_t2_idx)
               {};
};

template<class T, class M>

void do_matching(const map<int, vector<T> > & t1_objs, const map<int, vector<T> >& t2_objs,
	        M & obj_hists
		) 
        {

	  cout << "doing matching " << endl;

	  int not_matched_obj_t1 = 0;
	  int not_matched_obj_t2 = 0;

          int total_obj_t1(0);
          int total_obj_t2(0);

	  for(auto const & [e_t1, objs_t1]: t1_objs)
	  {
             map<int, match_property> matched_objs; // key:index of obj_rp, value: pt values of matched objs, drmin, idx of objs
             map<int, match_property> unmatched_objs_t1;
             bool t2_event_present(true);
	     for(auto const & obj_t1: objs_t1)
             {
               ++total_obj_t1;
               float drmin = 9999;
               match_property tmp = match_property();
               if (t2_objs.find(e_t1) == t2_objs.end()) {
                   cout << "no object found in type2 event with event # " << e_t1 << endl;
                   t2_event_present = false;
                   break;
               }
               auto objs_t2 = t2_objs.at(e_t1);
	       for(auto const & obj_t2: objs_t2)
	       {
	           auto dr = deltaR(obj_t1.eta, obj_t2.eta, obj_t1.phi, obj_t2.phi);
		   if (dr < obj_hists.get_drcut() && dr < drmin) {
                      drmin = dr;
		      tmp = match_property(drmin, obj_t1.pt, obj_t2.pt, obj_t1.idx, obj_t2.idx);
		   }
	       } // end of objs_rp loop
               if (tmp.t2_idx != -1)
               {
                   if(matched_objs.count(tmp.t2_idx) != 0)
                   {
                     if (drmin < matched_objs[tmp.t2_idx].drmin)
                     {
                        unmatched_objs_t1[matched_objs[tmp.t2_idx].t1_idx] = match_property(matched_objs[tmp.t2_idx].drmin,-1,-1, -1, -1);
                        matched_objs[tmp.t2_idx] = tmp;
                     }
                   }
                   else
                      matched_objs[tmp.t2_idx] = tmp;
               }
               else
                  unmatched_objs_t1[obj_t1.idx] = match_property();
             } // end of objs_r loop

             for(auto const & obj_t1: objs_t1)
             {
               if(unmatched_objs_t1.count(obj_t1.idx))
               {
                  not_matched_obj_t1++;
                  obj_hists.fill(Form("unmatched_pt_%s", obj_hists.type1().c_str()), obj_t1.pt);
               }
             }
             if (!t2_event_present) continue;
             for(auto const & obj_t2: t2_objs.at(e_t1))
             {
               total_obj_t2++;
               if(matched_objs.count(obj_t2.idx) == 0)
               {
                  not_matched_obj_t2++;
                  obj_hists.fill(Form("unmatched_pt_%s", obj_hists.type2().c_str()), obj_t2.pt);
               }
             }

             for(auto const& [matched_idx, matched_obj]: matched_objs)
             {
               obj_hists.fill("deltar", matched_obj.drmin);
               obj_hists.fill(Form("matched_pt_%s", obj_hists.type1().c_str()), matched_obj.t1_pt);
               obj_hists.fill(Form("matched_pt_%s", obj_hists.type2().c_str()), matched_obj.t2_pt);
               obj_hists.fill("ratio", (matched_obj.t2_pt-matched_obj.t1_pt)/matched_obj.t1_pt);
             }

          } // end of r_objs loop

	  cout << setprecision(2);
          cout << Form("total %s in type %s: ", obj_hists.get_base_name().c_str(), obj_hists.type1().c_str()) << total_obj_t1 << endl;
          cout << Form("total %s in type %s: ", obj_hists.get_base_name().c_str(), obj_hists.type2().c_str()) << total_obj_t2 << endl;
          cout << Form("total unmatched %s in type %s: ", obj_hists.get_base_name().c_str(), obj_hists.type1().c_str()) << not_matched_obj_t1 << endl;
          cout << Form("total unmatched %s in type %s: ", obj_hists.get_base_name().c_str(), obj_hists.type2().c_str()) << not_matched_obj_t2 << endl;	
          cout << Form("not matched %s: in type %s ", obj_hists.get_base_name().c_str(), obj_hists.type1().c_str()) << (100.*not_matched_obj_t1/total_obj_t1) << "%" << endl;
          cout << Form("not matched %s: in type %s ", obj_hists.get_base_name().c_str(), obj_hists.type2().c_str()) << (not_matched_obj_t2*100./total_obj_t2) << "%" << endl;

}

int main(int argc, char const *argv[]) {

	TFile* f1 = TFile::Open(argv[1], "read");
        TreeReader treereader ((TTree*) f1->Get("flatNtuple/tree"));
        
	TFile* f = new TFile("object_study.root", "recreate"); 

	cout << "creating hists for jet" << endl;

	EvthistManager evthist("woScouting");
        EvthistManager evthistfromScouting("wScouting");
        

	map<int, vector<Jet> > jet, jetfromScouting;
	cout << "calling eventloop" << endl;

	event_loop(treereader,
		   evthist, evthistfromScouting,
		   jet, jetfromScouting
		  );

	f->cd();

	evthist.write();
	evthistfromScouting.write();
	evthist.compareDist(evthistfromScouting);

	cout << "calling matching" << endl;

        {
           match_jetobj_histManager matchObj_hists("jet", 
	       "woScouting", "wScouting",
	       0.4
	   );
           do_matching(jet, jetfromScouting,
                    matchObj_hists
           );
           matchObj_hists.write();
           matchObj_hists.compareMatching();
        }

        cout << "matching done" << endl;
        f->Close();	

	return 0;
}

