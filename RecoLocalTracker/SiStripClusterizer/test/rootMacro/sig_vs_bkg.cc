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
#include "TH1D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TStopwatch.h"

#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TError.h"

#include "hist_auxiliary.h"
#include "TrackAlgo.h"

#define DEBUG 0

using namespace std;

void formatLegend(TLegend* leg, double textsize=27)
{
        leg->SetBorderSize(0);
        leg->SetTextFont(43);
        leg->SetTextSize(textsize);
        leg->SetFillStyle(0);
        leg->SetFillColor(0);
        leg->SetLineColor(0);
}


int main(int argc, char const *argv[])
{
	TFile* f1 = TFile::Open(argv[1],"read");//RawPrimeFlatTupleInt8.root", "read");
        TDirectoryFile* _1      = (TDirectoryFile*) f1->Get("sep19_2_2_dump_raw");//dump_rawprime");
        TTree* sigTree= (TTree*) _1->Get("signalTree");
        TTree* bkgTree= (TTree*) _1->Get("bkgTree");

	const static int nMax = 5000;
	////// for rawprime
	unsigned int bkg_event;
	int bkg_run;
	int bkg_lumi;

	// for approxCluster
	uint32_t    bkg_detId;
	uint16_t    bkg_firstStrip;
	uint16_t    bkg_endStrip;
	float       bkg_barycenter;
	uint16_t    bkg_size;
	int         bkg_charge;
        int         bkg_trk_algo;
	int         bkg_max_adc;
	int         bkg_max_adc_idx;
	int         bkg_adc_mone;
        int         bkg_adc_mtwo;
        int         bkg_adc_mthree;
        int         bkg_adc_mfour;
        int         bkg_adc_pone;
        int         bkg_adc_ptwo;
        int         bkg_adc_pthree;
        int         bkg_adc_pfour;


	////// for raw
	unsigned int sig_event;
	int sig_run;
	int sig_lumi;

	// for stripCluster
	uint32_t    sig_detId;
	uint16_t    sig_firstStrip;
	uint16_t    sig_endStrip;
	float       sig_barycenter;
	uint16_t    sig_size;
	int         sig_charge;
        int         ig_trk_algo;
	int         sig_max_adc;
        int         sig_max_adc_idx;
	int         sig_adc_mone;
	int         sig_adc_mtwo;
	int         sig_adc_mthree;
        int         sig_adc_mfour;
	int         sig_adc_pone;
        int         sig_adc_ptwo;
        int         sig_adc_pthree;
        int         sig_adc_pfour;

	sigTree->SetBranchAddress("event", &sig_event);
	sigTree->SetBranchAddress("run",   &sig_run);
	sigTree->SetBranchAddress("lumi",  &sig_lumi);

	sigTree->SetBranchAddress("detId", &sig_detId);
	sigTree->SetBranchAddress("firstStrip", &sig_firstStrip);
	sigTree->SetBranchAddress("endStrip", &sig_endStrip);
	sigTree->SetBranchAddress("barycenter", &sig_barycenter);
	sigTree->SetBranchAddress("size", &sig_size);
	sigTree->SetBranchAddress("charge", &sig_charge);
	sigTree->SetBranchAddress("max_adc", &sig_max_adc);
        sigTree->SetBranchAddress("max_adc_idx", &sig_max_adc_idx);
	sigTree->SetBranchAddress("adc_none", &sig_adc_mone);
        sigTree->SetBranchAddress("adc_ntwo", &sig_adc_mtwo);
        sigTree->SetBranchAddress("adc_nthree", &sig_adc_mthree);
        sigTree->SetBranchAddress("adc_nfour", &sig_adc_mfour);
        sigTree->SetBranchAddress("adc_pone", &sig_adc_pone);
        sigTree->SetBranchAddress("adc_ptwo", &sig_adc_ptwo);
        sigTree->SetBranchAddress("adc_pthree", &sig_adc_pthree);
        sigTree->SetBranchAddress("adc_pfour", &sig_adc_pfour);

	bkgTree->SetBranchAddress("event", &bkg_event);
	bkgTree->SetBranchAddress("run",   &bkg_run);
	bkgTree->SetBranchAddress("lumi",  &bkg_lumi);

	bkgTree->SetBranchAddress("detId", &bkg_detId);
	bkgTree->SetBranchAddress("firstStrip", &bkg_firstStrip);
	bkgTree->SetBranchAddress("endStrip", &bkg_endStrip);
	bkgTree->SetBranchAddress("barycenter", &bkg_barycenter);
	bkgTree->SetBranchAddress("size", &bkg_size);
	bkgTree->SetBranchAddress("charge", &bkg_charge);
	bkgTree->SetBranchAddress("max_adc", &bkg_max_adc);
	bkgTree->SetBranchAddress("max_adc_idx", &bkg_max_adc_idx);
	bkgTree->SetBranchAddress("adc_none", &bkg_adc_mone);
	bkgTree->SetBranchAddress("adc_ntwo", &bkg_adc_mtwo);
	bkgTree->SetBranchAddress("adc_nthree", &bkg_adc_mthree);
        bkgTree->SetBranchAddress("adc_nfour", &bkg_adc_mfour);
	bkgTree->SetBranchAddress("adc_pone", &bkg_adc_pone);
        bkgTree->SetBranchAddress("adc_ptwo", &bkg_adc_ptwo);
        bkgTree->SetBranchAddress("adc_pthree", &bkg_adc_pthree);
        bkgTree->SetBranchAddress("adc_pfour", &bkg_adc_pfour);

	TCanvas *canv0 = new TCanvas("canv0", "canv0", 600*3, 600*1);
	//gStyle->SetOptTitle(0);
	//gStyle->SetOptStat(0);
	gErrorIgnoreLevel = kWarning;
	canv0->Divide(3,1,0.001,0.001);

	TFile * f = new TFile("sig_vs_bkg_study.root", "recreate");

	TH1F * h_size_tot_sig      = new TH1F( "signal_size", "(offline) raw cluster; size; yield",  
	                                    50, 0., 50. );
	TH1F * h_charge_tot_sig     = new TH1F( "sig_charge", "(offline) raw cluster; charge; yield",  
	                                    88, 0., 704. );
	TH1F * h_max_adc_sig     = new TH1F( "max_adc_sig", "(offline) adc; adc; yield",
                                            205, 0., 255. );
	TH1F * h_max_adc_idx_sig     = new TH1F( "max_adc_idx_sig", "(offline) adc_idx; adc; yield",
                                            50, 0., 50. );
	TH2F * h_adc_sig     = new TH2F( "adc_sig", "(offline) adc; adc; yield",
                                            30, -5., 25.,255,0,255 );
	/*TH1F * h_barycentebkg_tot_sc = new TH1F( "RAW_offline_barrycenter", "(offline) raw cluster barycenter; yield",  
	                                    950, 0., 950. );
        TH1F* h_falling_barycentebkg_tot_ac = new TH1F("falling_barycenter", ";compressed #Delta barycenter;yield", 328, 0, 32800.);*/
	TH1F * h_size_tot_bkg     = new TH1F( "bkg_size", "(online) raw' cluster; size; yield",  
	                                    50, 0., 50. );
	TH1F * h_charge_tot_bkg     = new TH1F( "bkg_charge", "(online) raw' cluster; charge; yield",  
	                                    88, 0., 704. );
	TH1F * h_max_adc_bkg     = new TH1F( "max_adc_bkg", "max adc; adc; yield",
                                            205, 0., 255. );
	TH1F * h_max_adc_idx_bkg     = new TH1F( "max_adc_idx_bkg", "(offline) adc_idx; adc; yield",
                                            50, 0., 50. );
	TH2F * h_adc_bkg     = new TH2F( "adc_bkg", "(offline) adc; adc; yield",
                                            30, -5., 25.,255,0,255 );
	/*TH1F * h_barycentebkg_tot_ac = new TH1F( "RAW'_online_barycenter", "(online) raw' cluster; barycenter; yield",  
	                                    950, 0., 950. );
*/


        gStyle->SetOptStat(0);
	const Int_t bkg_nEntries = bkgTree->GetEntries();
	for (int bkg_idx = 0; bkg_idx < bkg_nEntries; ++bkg_idx)
	{
		if(bkg_idx%1000000 == 0) std::cout << "Scanning raw clusters: " << bkg_idx << "/" << bkg_nEntries << std::endl;
		bkgTree->GetEntry(bkg_idx);
                h_size_tot_bkg->Fill(bkg_size);
                h_charge_tot_bkg->Fill(bkg_charge);
		h_max_adc_bkg->Fill(bkg_max_adc);
		h_max_adc_idx_bkg->Fill(bkg_max_adc_idx);
		h_adc_bkg->Fill(bkg_max_adc_idx,bkg_max_adc);
		h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)-1, bkg_adc_mone);
		h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)-2, bkg_adc_mtwo);
		h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)-3, bkg_adc_mthree);
		h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)-4, bkg_adc_mfour);
		h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)+1, bkg_adc_pone);
                h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)+2, bkg_adc_ptwo);
                h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)+3, bkg_adc_pthree);
                h_adc_bkg->Fill(h_adc_bkg->FindBin(bkg_max_adc_idx)+4, bkg_adc_pfour);

	}
	const Int_t sig_nEntries = sigTree->GetEntries();
	for (int sig_idx = 0; sig_idx < sig_nEntries; ++sig_idx)
	{
		if(sig_idx%1000000 == 0) std::cout << "Scanning rawprime clusters: " << sig_idx << "/" << sig_nEntries << std::endl;
		sigTree->GetEntry(sig_idx);
		h_size_tot_sig->Fill( sig_size );
		h_charge_tot_sig->Fill(sig_charge);
		h_max_adc_sig->Fill(sig_max_adc);
		h_max_adc_idx_sig->Fill(sig_max_adc_idx);
		h_adc_sig->Fill(sig_max_adc_idx,sig_max_adc);
		//std::cout << (h_adc_sig->FindBin(sig_max_adc_idx)-1) << "\t" << sig_max_adc_idx << std::endl;
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)-1, sig_adc_mone);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)-2, sig_adc_mtwo);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)-3, sig_adc_mthree);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)-4, sig_adc_mfour);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)+1, sig_adc_pone);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)+2, sig_adc_ptwo);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)+3, sig_adc_pthree);
                h_adc_sig->Fill(h_adc_sig->FindBin(sig_max_adc_idx)+4, sig_adc_pfour);
	}

        h_size_tot_sig->Write();
	h_size_tot_bkg->Write();
	h_charge_tot_sig->Write();
        h_charge_tot_bkg->Write();
	h_max_adc_sig->Write();
	h_max_adc_bkg->Write();
	h_max_adc_idx_sig->Write();
	h_max_adc_idx_bkg->Write();
	h_adc_sig->Write();
	h_adc_bkg->Write();

	delete h_size_tot_sig;
	delete h_size_tot_bkg;
	delete h_charge_tot_sig;
        delete h_charge_tot_bkg;
	delete h_max_adc_sig;
	delete h_max_adc_bkg;
	delete h_max_adc_idx_sig;
	delete h_max_adc_idx_bkg;
	delete h_adc_sig;
	delete h_adc_bkg;
        f->Close();
	return 0;
}

