#pragma once
#include "histManagerBase.h"
#include "TrackAlgo.h"
#include "object.h"

class match_obj_histManager
   : public histManagerBase
{
     public:
       match_obj_histManager(const string& obj, const float& in_drcut):
       histManagerBase(obj)
       ,drcut(in_drcut) {}


     const float get_drcut() const
     {
         return drcut;
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
     };

     protected:
       float drcut;
};

class match_trackobj_histManager
      : public match_obj_histManager
{
        public:
          match_trackobj_histManager(const string& obj, const float& in_drcut):
          match_obj_histManager(obj, in_drcut)
          {

              bool lowpt = string(base_name).find("lowpt") != std::string::npos;
              for (const auto match_type: {"matched", "unmatched"})
              {
                for (const auto var_type: {"pt"})
                {
                  for (const auto raw_type: {"r", "rp"})
                  {
                     auto key = Form("%s_%s_%s", match_type, var_type, raw_type);
                     hists[key] = createhist(Form("%s_%s", base_name.c_str(), key), Form("%s;pt;yield", key), numBins, (lowpt) ? customBins_lowpt : customBins_highpt);
                  }
                }
              }

              for (int i=TrackAlgorithm::undefAlgorithm; i<=TrackAlgorithm::displacedRegionalStep;i++) {
                 std::string name = "deltar_" + to_string(i);
                 hists[name] = createhist(Form("%s_%s", base_name.c_str(), name.c_str()), Form("%s;delta_r;yield", algoNames[i].c_str()), 50, 0., drcut);
              }
              hists["ratio"] = createhist(Form("%s_ratio", base_name.c_str()), ";Raw_pt/Raw'_pt;yield", 50, 0.95, 1.05);

        };

          void compareMatching()
          {
             vector<string> plots;
            for (int i=TrackAlgorithm::undefAlgorithm; i<=TrackAlgorithm::displacedRegionalStep;i++)
              plots.push_back("deltar_"+to_string(i));
            plots.push_back("ratio");
            Plot_single(plots);
            match_obj_histManager::compareMatching();

          };

          void fill_deltar(const float& val, const int& r_algo, const int& rp_algo) 
          {
            //if(r_algo != rp_algo) cout << r_algo << "\t" << rp_algo << endl;
            fill("deltar_"+ to_string(r_algo), val);
          };
};

class match_jetobj_histManager
      : public match_obj_histManager
{         
        public:
          match_jetobj_histManager(const string& obj, const float& in_drcut):
          match_obj_histManager(obj, in_drcut)
          {
          
              for (const auto match_type: {"matched", "unmatched"})
              {
                for (const auto var_type: {"pt"})
                {
                  for (const auto raw_type: {"r", "rp"})
                  {
                     auto key = Form("%s_%s_%s", match_type, var_type, raw_type);
                     hists[key] = createhist(Form("%s_%s", base_name.c_str(), key), Form("%s;pt;yield", key), numBins_jets, customBins_jets);
                  }
                } 
              }

              hists["deltar"] = createhist(Form("%s_delta_r", base_name.c_str()), "delta_r;delta_r;yield", 50, 0., drcut);
              hists["ratio"] = createhist(Form("%s_ratio", base_name.c_str()), ";Raw_pt/Raw'_pt;yield", 50, 0.95, 1.05);
         
        };

        void compareMatching()
        {
           Plot_single({"deltar", "ratio"});
           match_obj_histManager::compareMatching();
       };

       void fill_deltar(const float& val, const int& r_algo, const int& rp_algo)
       {
            //if(r_algo != rp_algo) cout << r_algo << "\t" << rp_algo << endl;
            fill("deltar", val);
       };
};

