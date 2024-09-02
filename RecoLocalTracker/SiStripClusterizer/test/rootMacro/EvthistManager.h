#ifndef EVTHIST_MANAGER_H
#define EVTHIST_MANAGER_H

#include "histManagerBase.h"
#include "TMath.h"

enum cuts {nocut=1, dzSig, chi2, ptRes, nhits};
map<int, std::string> cutToname = { {cuts::nocut, "nocut"}, {cuts::dzSig, "dzSig"}, {cuts::ptRes, "ptRes"},{cuts::chi2, "chi2"},{cuts::nhits, "nhits"}};

class EvthistManager
   : public histManagerBase
{
     public:
       EvthistManager(string obj):
       histManagerBase(obj) {

	 hists["trk_pt"] = createhist(Form("%s_trk_pt_tot", base_name.c_str()), "trk_pt;trk_pt;yield", numBins, customBins);
	 hists["trk_eta"] = createhist(Form("%s_trk_eta", base_name.c_str()), "trk_eta;trk_eta;yield", 16, -2.4, 2.4);
	 hists["trk_dxyDdxyerr"] = createhist(Form("%s_trk_dxyDdxyerr", base_name.c_str()), "trk_dxyDdxyerr;xy/#sigma;yield", 30, -5., 5.);
         hists["trk_dzDdzerr"] = createhist(Form("%s_trk_dzDdzerr", base_name.c_str()), "trk_dzDdzerr;z/#sigma;yield", 50, 0., 10.);
         hists["trk_chi2"] = createhist(Form("%s_trk_chi2", base_name.c_str()), "trk_chi2;trk_chi2;yield", 400, 0,100);
         hists["trk_nhits"] = createhist(Form("%s_trk_nhits", base_name.c_str()), "trk_nhits;trk_nhits;yield", 100, -0.5, 99.5);
         hists["trk_pterrDpt"] = createhist(Form("%s_trk_pterrDpt", base_name.c_str()), "trk_pterrDpt;trk_pterrDpt;yield", 50, 0, 1);
         hists["cutflow"] = createhist(Form("%s_cutflow", base_name.c_str()), "cutflow;cutflow;yield", cuts::nhits, cuts::nocut, cuts::nhits+1);
      
	hists_2d["trk_eta_phi"] = createhist(Form("%s_trk_eta_phi", base_name.c_str()), "trk_eta_phi;#eta; #phi;yield", 16, -2.4, 2.4, 30, -TMath::Pi(), TMath::Pi());
       };
};


#endif //EVTHIST_MANAGER
