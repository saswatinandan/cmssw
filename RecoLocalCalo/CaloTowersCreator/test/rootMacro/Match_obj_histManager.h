#pragma once
#include "histManagerBase.h"
#include "object.h"

class match_obj_histManager
   : public histManagerBase
{
     public:
       match_obj_histManager(const string& obj, 
		       const std::string& type1, const std::string type2,
		       const float& in_drcut):
       histManagerBase(obj)
       ,type1_(type1)
       ,type2_(type2)
       ,drcut(in_drcut) {

        hists["deltar"] = createhist(Form("%s_delta_r", base_name.c_str()), "delta_r;delta_r;yield", 50, 0., drcut);
        hists["ratio"] = createhist(Form("%s_ratio", base_name.c_str()), Form(";#Delta p_{T}(%s, %s)/p_{T};Normalized yield", type2_.c_str(), type1_.c_str()), 50, -0.1, .1);
       }


     const float get_drcut() const
     {
         return drcut;
     };

     const string type1() const {
       return type1_;
     }

     const string type2() const {
       return type2_;
     }

     void compareMatching()
     {

         map<string, TH1F*> hists_1 = { {"matched_pt", hists[Form("matched_pt_%s", type1_.c_str())]},
                                       {"unmatched_pt", hists[Form("unmatched_pt_%s", type1_.c_str())]}
         };

         map<string, TH1F*> hists_2 = { {"matched_pt", hists[Form("matched_pt_%s", type2_.c_str())]},
                                       {"unmatched_pt", hists[Form("unmatched_pt_%s", type2_.c_str())]}
         };

        compareDist(hists_1, type1_,
                     hists_2, type2_,
                     get_base_name()+"_");
        Plot_single({"deltar", "ratio"});
     };

     protected:
       std::string type1_, type2_;
       float drcut;
};

class match_jetobj_histManager
      : public match_obj_histManager
{         
        public:
          match_jetobj_histManager(const string& obj, 
	        const std::string& type1, std::string type2,
		const float& in_drcut):
          match_obj_histManager(obj, 
	        type1, type2,in_drcut)
          {
          
              for (const auto match_type: {"matched", "unmatched"})
              {
                for (const auto var_type: {"pt"})
                {
                  for (const auto type: {type1, type2})
                  {
                     auto key = Form("%s_%s_%s", match_type, var_type, type.c_str());
                     hists[key] = createhist(Form("%s_%s", base_name.c_str(), key), Form("%s;pt;yield", key), numBins, customBins);
                  }
                } 
              }
          };

};


