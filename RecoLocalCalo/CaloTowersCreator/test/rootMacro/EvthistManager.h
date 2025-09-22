#ifndef EVTHIST_MANAGER_H
#define EVTHIST_MANAGER_H

#include "histManagerBase.h"
#include "TMath.h"

class EvthistManager
   : public histManagerBase
{
     public:
       EvthistManager(string obj):
       histManagerBase(obj) {

       ///// jet ////

       hists["jet_pt"]      = createhist(Form("%s_jet_pt", base_name.c_str()), "jet_pt;jet_pt;yield", numBins, customBins);
       hists["jet_eta"]     = createhist(Form("%s_jet_eta", base_name.c_str()), "jet_eta;jet_eta;yield", 16, -2.4, 2.4);
       hists["jet_phi"]     = createhist(Form("%s_jet_phi", base_name.c_str()), "jet_phi;jet_phi;yield", 16, -3.15, 3.15);
       hists["jet_mass"]      = createhist(Form("%s_jet_mass", base_name.c_str()), "jet_mass;jet_mass;yield", 20, 20, 100);
       }
};


#endif //EVTHIST_MANAGER

