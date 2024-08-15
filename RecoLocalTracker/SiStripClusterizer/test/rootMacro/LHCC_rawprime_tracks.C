#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <functional>
#include <cassert>

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TTree.h"
#include "TChain.h"
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
#include "TMath.h"

#include "hist_auxiliary.h"

using namespace std;

std::vector<std::string> getFileList(std::string inFileName) // fileListTxt, root file
{
        std::vector<std::string> fileList;
        
        int type;
        if(inFileName.size() < 5)
        {
                std::cout << "Given inFileName \'" << inFileName << "\' is invalid, exit 1." << std::endl;
                exit(1);
        }
        else if(inFileName.substr(inFileName.size()-4, 4).find(".txt") != std::string::npos)    type = 0;
        else if(inFileName.substr(inFileName.size()-5, 5).find(".root") != std::string::npos)   type = 1;
        else{
                std::cout << "Given inFileName \'" << inFileName << "\' is invalid, exit 1." << std::endl;
                exit(1);
        }

        if(type==0) {
                std::ifstream pathFile(inFileName.c_str());
                std::string paths;
                while(std::getline(pathFile, paths)){
                        if(paths.size() == 0) continue;
                        fileList.push_back(paths);
                }
                pathFile.close();
        }
        else { // 1
                fileList.push_back(inFileName);
        }
        return(fileList);
}

template<class T>
void PlotStyle(T* h)
{
	//fonts
    int defaultFont       = 43;
    float x_title_size    = 28;
    float y_title_size    = 28;

    float x_title_offset  = 1.5;
    float y_title_offset  = 2.2;

    float label_size      = 28;
    float label_offset    = 0.013;

	h->GetXaxis()->SetLabelFont(defaultFont);
	h->GetXaxis()->SetTitleFont(defaultFont);
	h->GetYaxis()->SetLabelFont(defaultFont);
	h->GetYaxis()->SetTitleFont(defaultFont);
	h->GetZaxis()->SetLabelFont(defaultFont);
	h->GetZaxis()->SetTitleFont(defaultFont);

	// gStyle->SetTitleFontSize(16);
	h->SetTitleOffset  (0);


	h->GetYaxis()->SetTitleOffset  (y_title_offset);
	h->GetYaxis()->CenterTitle();
	h->GetYaxis()->SetTitleSize    (x_title_size);
	h->GetYaxis()->SetLabelOffset  (label_offset);
	h->GetYaxis()->SetLabelSize    (label_size);
	h->GetYaxis()->SetNdivisions(508);

	h->GetXaxis()->SetTitleOffset  (1.2);
	h->GetXaxis()->CenterTitle();
	h->GetXaxis()->SetTitleSize    (x_title_size);
	h->GetXaxis()->SetLabelOffset  (label_offset);
	h->GetXaxis()->SetLabelSize    (label_size);
	h->GetXaxis()->SetNdivisions(508);

	h->GetZaxis()->SetTitleOffset  (1.8);
	h->GetZaxis()->CenterTitle();
	h->GetZaxis()->SetTitleSize    (x_title_size);
	h->GetZaxis()->SetLabelOffset  (label_offset);
	h->GetZaxis()->SetLabelSize    (label_size);
	h->GetZaxis()->SetNdivisions(508);

	h->SetLineWidth(2);
	// h->SetMinimum(-0.001);

}


void formatLegend(TLegend* leg, double textsize=27)
{
        leg->SetBorderSize(0);
        leg->SetTextFont(43);
        leg->SetTextSize(textsize);
        leg->SetFillStyle(0);
        leg->SetFillColor(0);
        leg->SetLineColor(0);
}

enum cuts {nocut=1, dzSig, chi2, ptRes, nhits};
std::map<int, std::string> cutToname = { {cuts::nocut, "nocut"}, {cuts::dzSig, "dzSig"}, {cuts::ptRes, "ptRes"},{cuts::chi2, "chi2"},{cuts::nhits, "nhits"}};



struct test{
	string f1name;
	string f2name;
	string matchtxt;

	test(string in_expTag)
	{
		if (in_expTag=="LHCC_run374345_ls29") 
		{
			cout << "run374345 (small: ls29)" << endl;
			f1name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run374345_ls29_HIPhysicsRawPrime0.root";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run374345_ls29_HIMinimumBias0.root";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run374345_ls29.txt";
		} else if (in_expTag=="LHCC_run374345") 
		{
			cout << "run374345" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_HIPhysicsRawPrime0.txt";
			f2name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_HIMinimumBias0.txt";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run374345.txt";
		} else if (in_expTag=="LHCC_run374345_run374347") 
		{
			cout << "run374345, run374347" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_HIPhysicsRawPrime0.txt";
			f2name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_HIMinimumBias0.txt";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run374345_run374347.txt";
		} else if (in_expTag=="LHCC_run374345_run374347_run374354") 
		{
			cout << "run374345, run374347, run374354" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_HIPhysicsRawPrime0.txt";
			f2name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_HIMinimumBias0.txt";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run374345_run374347_run374354.txt";
		} else if (in_expTag=="LHCC_run374345_run374347_run374354_run375007_run375055") 
		{
			cout << "run374345, run374347, run374354, run375007, run375055" << endl;
			// f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_run375007_run375055_HIPhysicsRawPrime0.txt";
			// f2name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_run375007_run375055_HIMinimumBias0.txt";
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_run375007_run375055_HIPhysicsRawPrime0_selected.root";
			f2name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_run375007_run375055_HIMinimumBias0_selected.root";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run374345_run374347_run374354_run375007_run375055.txt";
		} else if (in_expTag=="LHCC_run374345_expressStream") 
		{
			cout << "run374345" << endl;
			f1name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/run374354_rawprime.root";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/run374354_raw.root";
			matchtxt = "all";
		} else if (in_expTag=="LHCC_run375820") 
		{
			cout << "run375820" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run375820_HIPhysicsRawPrime0.txt";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375820_HIMinimumBias0.root";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run375820.txt";
		} else if (in_expTag=="LHCC_run375790") 
		{
			cout << "run375790" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run375790_HIPhysicsRawPrime0.txt";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375790_HIMinimumBias0.root";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run375790.txt";
		} else if (in_expTag=="LHCC_run375820_selected") 
		{
			cout << "run375820 (selected)" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run375820_HIPhysicsRawPrime0_selected.txt";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375820_HIMinimumBias0_selected.root";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run375820.txt";
		} else if (in_expTag=="LHCC_run375790_selected") 
		{
			cout << "run375790 (selected)" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run375790_HIPhysicsRawPrime0_selected.txt";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375790_HIMinimumBias0_selected.root";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run375790.txt";
		} else if (in_expTag=="LHCC_run375790_run375820_run375823_selected") 
		{
			cout << "run375790, run375820, run375823 (selected)" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run375790_run375820_run375823_HIPhysicsRawPrime0_selected.txt";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375790_run375820_run375823_HIMinimumBias0_selected.root";
			// hadd /afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375790_run375820_run375823_HIMinimumBias0_selected.root /afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375790_HIMinimumBias0_selected.root /afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375820_HIMinimumBias0_selected.root /afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375823_HIMinimumBias0_selected.root
			matchtxt = "lumi_run_event_txt/matching_rows_check_run375790_run375820_run375823.txt";
		}
	}
};


///IMPORTANT: in case need to test other track selections, should change here	
float cut_dzSig = 3.0;
float cut_dxySig = 3.0;
float cut_ptRes = 0.10;
float cut_chi2 = 0.18;
float cut_nhits = 11;


void Divide_w_sameDsets(TH1F* num, TH1F* denom, TH1F* ratio)
{
	for (int _x = 1; _x < num->GetNbinsX()+1; ++_x)
	{
		float _r 	= denom->GetBinContent(_x) ? num->GetBinContent(_x)/denom->GetBinContent(_x) : 0.;
		float _n_relErr = TMath::Abs(   num->GetBinError(_x)/  num->GetBinContent(_x) );
		float _d_relErr = TMath::Abs( denom->GetBinError(_x)/denom->GetBinContent(_x) );
		float _r_err 	= TMath::Abs(_r) * ( (_n_relErr > _d_relErr) ? _n_relErr : _d_relErr  );
		ratio->SetBinContent(_x, _r);
		ratio->SetBinError(_x, _r_err);
	}
}

auto deltaR(float& e1, float& e2, float& p1, float& p2) {

	auto dp = std::abs(p1 -p2);
	if (dp > float(M_PI))
           dp -= float(2 * M_PI);
	return TMath::Sqrt(pow((e1 - e2), 2) + pow(dp, 2));
}

auto createhist(std::string name, std::string title, int nbins, Double_t array[8], float xlow=9999, float xhigh=0){

	if(xlow==9999) {
	 TH1F* h = new TH1F(name.c_str(), title.c_str(), nbins, array);
	 h->Sumw2();
	 return h;
	}
	else {
         TH1F* h = new TH1F(name.c_str(), title.c_str(), nbins, xlow, xhigh);
         h->Sumw2();
         return h;
	}
}

int LHCC_rawprime_tracks() {

	std::string expTag = "test";
	map< int, map< int, map<int, bool> > > evtMatchedMap;
	map< int, map< int, map<int, bool> > > evtMap;
	
        TFile* f1               = TFile::Open("flatTrackRawprime.root", "read");
        TDirectoryFile* _1      = (TDirectoryFile*) f1->Get("trackAnalyzer");
        TTree* trackTree_rp     = (TTree*) _1->Get("trackTree");
        
        TFile* f2               = TFile::Open("flatTrackRaw.root", "read");
        TDirectoryFile* _2      = (TDirectoryFile*) f2->Get("trackAnalyzer");
        TTree* trackTree_r      = (TTree*) _2->Get("trackTree");

	const static int nMax = 40000;
	////// for rawprime
	UInt_t rp_event;
	int rp_run;
	int rp_lumi;
	int rp_nTrk;

	float       rp_trkPt[nMax];
	float       rp_trkEta[nMax];
	float       rp_trkPhi[nMax];
	float       rp_trkDxy1[nMax];
	float       rp_trkDxyError1[nMax];
	float       rp_trkDz1[nMax];
	float       rp_trkDzError1[nMax];
	
	UShort_t rp_trkAlgo[nMax];
	Int_t rp_trkNHit[nMax];
	Int_t rp_trkNdof[nMax];
	Int_t rp_trkNlayer[nMax];
	float       rp_trkChi2[nMax];
	float       rp_trkPtError[nMax];


	////// for raw
	UInt_t r_event;
	int r_run;
	int r_lumi;
	int r_nTrk;

	float       r_trkPt[nMax];
	float       r_trkEta[nMax];
	float       r_trkPhi[nMax];
	float       r_trkDxy1[nMax];
	float       r_trkDxyError1[nMax];
	float       r_trkDz1[nMax];
	float       r_trkDzError1[nMax];
	
	UShort_t r_trkAlgo[nMax];
	Int_t r_trkNHit[nMax];
	Int_t r_trkNdof[nMax];
	Int_t r_trkNlayer[nMax];
	float       r_trkChi2[nMax];
	float       r_trkPtError[nMax];

	trackTree_rp->SetBranchAddress("event", 	&rp_event);
	trackTree_rp->SetBranchAddress("run",  &rp_run);
	trackTree_rp->SetBranchAddress("lumi", &rp_lumi);
	trackTree_rp->SetBranchAddress("nTracks",  &rp_nTrk);

	trackTree_rp->SetBranchAddress("trkPt", 	rp_trkPt);
	trackTree_rp->SetBranchAddress("trkEta", 	rp_trkEta);
	trackTree_rp->SetBranchAddress("trkPhi", 	rp_trkPhi);
	trackTree_rp->SetBranchAddress("trkDxy1", 	rp_trkDxy1);
	trackTree_rp->SetBranchAddress("trkDxyError1", 	rp_trkDxyError1);
	trackTree_rp->SetBranchAddress("trkDz1", 	rp_trkDz1);
	trackTree_rp->SetBranchAddress("trkDzError1", 	rp_trkDzError1);
	trackTree_rp->SetBranchAddress("trkAlgo", 	rp_trkAlgo);
	trackTree_rp->SetBranchAddress("trkNHit", 	rp_trkNHit);
	trackTree_rp->SetBranchAddress("trkNdof", 	rp_trkNdof);
	trackTree_rp->SetBranchAddress("trkNlayer", 	rp_trkNlayer);
	trackTree_rp->SetBranchAddress("trkChi2", 	rp_trkChi2);
	trackTree_rp->SetBranchAddress("trkPtError", 	rp_trkPtError);

	trackTree_r->SetBranchAddress("event", 	&r_event);
	trackTree_r->SetBranchAddress("run",   &r_run);
	trackTree_r->SetBranchAddress("lumi",  &r_lumi);
	trackTree_r->SetBranchAddress("nTracks",  	&r_nTrk);

	trackTree_r->SetBranchAddress("trkPt", 		r_trkPt);
	trackTree_r->SetBranchAddress("trkEta", 	r_trkEta);
	trackTree_r->SetBranchAddress("trkPhi", 	r_trkPhi);
	trackTree_r->SetBranchAddress("trkDxy1", 	r_trkDxy1);
	trackTree_r->SetBranchAddress("trkDxyError1", 	r_trkDxyError1);
	trackTree_r->SetBranchAddress("trkDz1", 	r_trkDz1);
	trackTree_r->SetBranchAddress("trkDzError1", 	r_trkDzError1);
	trackTree_r->SetBranchAddress("trkAlgo", 	r_trkAlgo);
	trackTree_r->SetBranchAddress("trkNHit", 	r_trkNHit);
	trackTree_r->SetBranchAddress("trkNdof", 	r_trkNdof);
	trackTree_r->SetBranchAddress("trkNlayer", 	r_trkNlayer);
	trackTree_r->SetBranchAddress("trkChi2", 	r_trkChi2);
	trackTree_r->SetBranchAddress("trkPtError", 	r_trkPtError);

	TFile * f = new TFile("tracker_study.root", "recreate"); 
	// Define custom bin edges
	const int numBins = 7;
	Double_t customBins[numBins + 1] = {0.1, 0.5, 1.0, 2.0, 6.0, 9.0, 30.0, 100.0};

	TH1F * h_pt_tot_r      = new TH1F( "RAW_pt_tot",
	                                    "; track p_{T} [GeV/#it{c}]; normalized distributions",  
	                                    numBins, customBins);
	TH1F * h_dcaxyres_tot_r     = new TH1F( "RAW_decayxyres",
	                                    "; track DCA xy/#sigma; normalized distributions", //  (0.5 < p_{T} < 0.9 [GeV/#it{c}]); normalized distributions",  
	                                    30, -5., 5. );
	TH1F * h_dcazres_tot_r = new TH1F( "RAW_dcazres",
	                                    "; track DCA z/#sigma; normalized distributions", // (0.5 < p_{T} < 0.9 [GeV/#it{c}]); normalized distributions",  
	                                    30, -3., 3. );
	TH2F * h_etaphi_r = new TH2F( "h_etaphi_r", 
	                                    "RAW\'/RAW; track #eta; track #phi",  
	                                    16, -2.4, 2.4,
	                                    30, -TMath::Pi(), TMath::Pi() );
	TH1F * h_eta_r = new TH1F( "h_eta_r", 
	                                    "RAW\'/RAW; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo0 = new TH1F( "h_eta_r_trkAlgo0", 
	                                    "trkAlgo==0; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo2 = new TH1F( "h_eta_r_trkAlgo2", 
	                                    "trkAlgo==2; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo4 = new TH1F( "h_eta_r_trkAlgo4", 
	                                    "trkAlgo==4; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo5 = new TH1F( "h_eta_r_trkAlgo5", 
	                                    "trkAlgo==5; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo6 = new TH1F( "h_eta_r_trkAlgo6", 
	                                    "trkAlgo==6; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo7 = new TH1F( "h_eta_r_trkAlgo7", 
	                                    "trkAlgo==7; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo8 = new TH1F( "h_eta_r_trkAlgo8", 
	                                    "trkAlgo==8; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo9 = new TH1F( "h_eta_r_trkAlgo9", 
	                                    "trkAlgo==9; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo10 = new TH1F( "h_eta_r_trkAlgo10", 
	                                    "trkAlgo==10; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo13 = new TH1F( "h_eta_r_trkAlgo13", 
	                                    "trkAlgo==13; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo22 = new TH1F( "h_eta_r_trkAlgo22", 
	                                    "trkAlgo==22; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo23 = new TH1F( "h_eta_r_trkAlgo23", 
	                                    "trkAlgo==23; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_r_trkAlgo24 = new TH1F( "h_eta_r_trkAlgo24", 
	                                    "trkAlgo==24; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );

	TH1F * h_pt_tot_rp      = new TH1F( "RAW'_pt_tot",
	                                    "; track p_{T} [GeV/#it{c}]; normalized distributions",  
	                                    numBins, customBins);
	TH1F * h_dcaxyres_tot_rp     = new TH1F( "RAW'_dcaxyres",
	                                    "; track DCA xy/#sigma; normalized distributions", //  (0.5 < p_{T} < 0.9 [GeV/#it{c}]); normalized distributions",  
	                                    30, -5., 5. );
	TH1F * h_dcazres_tot_rp = new TH1F( "RAW'_dcazres_tot",
	                                    "; track DCA z/#sigma; normalized distributions", // (0.5 < p_{T} < 0.9 [GeV/#it{c}]); normalized distributions",  
	                                    30, -3., 3. );
	TH2F * h_etaphi_rp = new TH2F( "h_etaphi_rp",
					    "RAW\'/RAW; track #eta; track #phi",
	                                    16, -2.4, 2.4,
	                                    30, -TMath::Pi(), TMath::Pi() );
	TH1F * h_eta_rp = new TH1F( "h_eta_rp", 
	                                    "RAW\'/RAW; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo0 = new TH1F( "h_eta_rp_trkAlgo0", 
	                                    "trkAlgo==0; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo2 = new TH1F( "h_eta_rp_trkAlgo2", 
	                                    "trkAlgo==2; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo4 = new TH1F( "h_eta_rp_trkAlgo4", 
	                                    "trkAlgo==4; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo5 = new TH1F( "h_eta_rp_trkAlgo5", 
	                                    "trkAlgo==5; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo6 = new TH1F( "h_eta_rp_trkAlgo6", 
	                                    "trkAlgo==6; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo7 = new TH1F( "h_eta_rp_trkAlgo7", 
	                                    "trkAlgo==7; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo8 = new TH1F( "h_eta_rp_trkAlgo8", 
	                                    "trkAlgo==8; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo9 = new TH1F( "h_eta_rp_trkAlgo9", 
	                                    "trkAlgo==9; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo10 = new TH1F( "h_eta_rp_trkAlgo10", 
	                                    "trkAlgo==10; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo13 = new TH1F( "h_eta_rp_trkAlgo13", 
	                                    "trkAlgo==13; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo22 = new TH1F( "h_eta_rp_trkAlgo22", 
	                                    "trkAlgo==22; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo23 = new TH1F( "h_eta_rp_trkAlgo23", 
	                                    "trkAlgo==23; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );
	TH1F * h_eta_rp_trkAlgo24 = new TH1F( "h_eta_rp_trkAlgo24", 
	                                    "trkAlgo==24; track #eta; normalized distributions",  
	                                    16, -2.4, 2.4 );

	TH1F * h_trk_dzDdzerr = new TH1F("trk_dzDdzerr", "Raw;trk_dzDdzerr;yield;", 50, 0,10);
	TH1F * h_trk_chi2 = new TH1F("trk_chi2", "Raw;trk_chi2;yield;", 400, 0,100);
	TH1F * h_trk_nhits = new TH1F("trk_nhits", "Raw;trk_nhits;yield;", 100, -0.5,100);
	TH1F * h_trk_pterrDpt = new TH1F("trk_pterrDpt", "Raw;trk_pterrDpt;yield;", 50, 0,1);
	TH1F * h_cutflow = new TH1F("cutflow", "cutflow", cuts::nhits, cuts::nocut, cuts::nhits+1);

	TH1F * h_trk_p_dzDdzerr = new TH1F("trk_p_dzDdzerr", "Raw';trk_p_dzDdzerr;yield;", 50, 0,10);
        TH1F * h_trk_p_chi2 = new TH1F("trk_p_chi2", "Raw';trk_p_chi2;yield;", 400, 0,100);
        TH1F * h_trk_p_nhits = new TH1F("trk_p_nhits", "Raw';trk_p_nhits;yield;", 100, -0.5,100);
        TH1F * h_trk_p_pterrDpt = new TH1F("trk_p_pterrDpt", "Raw';trk_p_pterrDpt;yield;", 50, 0,1);
	TH1F * h_cutflow_p = new TH1F("cutflow_p", "cutflow'", cuts::nhits, cuts::nocut, cuts::nhits+1);

	TH1F * h_matchtrack_pt_ratio = new TH1F("ratio", ";Raw_pt/Raw'_pt;yield", 50, 0.95,1.05);

	for(int ibin=cuts::nocut; ibin<=cuts::nhits; ibin++) {
		h_cutflow->GetXaxis()->SetBinLabel(ibin, cutToname[ibin].c_str());
		h_cutflow_p->GetXaxis()->SetBinLabel(ibin, cutToname[ibin].c_str());
	}

	const Int_t r_nEntries = trackTree_r->GetEntries();
	for (int r_idx = 0; r_idx < r_nEntries; ++r_idx) {
                trackTree_r->GetEntry(r_idx);
		evtMatchedMap[r_run][r_lumi][r_event] = 0;
	}
       
	const Int_t rp_nEntries = trackTree_rp->GetEntries();
        for (int rp_idx = 0; rp_idx < rp_nEntries; ++rp_idx) {

		trackTree_rp->GetEntry(rp_idx);
		
		if (  evtMatchedMap.find(rp_run) == evtMatchedMap.end() ) continue;
                if (  evtMatchedMap[rp_run].find(rp_lumi) == evtMatchedMap[rp_run].end() ) continue;
		if (  evtMatchedMap[rp_run][rp_lumi].find(rp_event) == evtMatchedMap[rp_run][rp_lumi].end() ) continue;

		evtMatchedMap[rp_run][rp_lumi][rp_event] = 1;

	}

        map< int, std::vector<int> > r_goodtrack;
        map< int, std::vector<int> > rp_goodtrack;	
        float pt_avg_r = 0;
	for (int r_idx = 0; r_idx < r_nEntries; ++r_idx) {
		if(r_idx%1000 == 0) std::cout << "Scanning raw tracks: " << r_idx << "/" << r_nEntries << std::endl;
		trackTree_r->GetEntry(r_idx);

		if ( !evtMatchedMap[r_run][r_lumi][r_event]) {
			assert(0);
			continue;
		}
		// trackTree_r->Show(r_idx);
		r_goodtrack[r_idx] = {};

		for (int r_trkIdx = 0; r_trkIdx < r_nTrk; ++r_trkIdx)
		{
			fillWithOverFlow(h_trk_dzDdzerr, std::abs(r_trkDz1[r_trkIdx]/r_trkDzError1[r_trkIdx]));
			fillWithOverFlow(h_trk_chi2, r_trkChi2[r_trkIdx]/((int) r_trkNdof[r_trkIdx]));
			fillWithOverFlow(h_trk_nhits, r_trkNHit[r_trkIdx]);
                        fillWithOverFlow(h_trk_pterrDpt, std::abs(r_trkPtError[r_trkIdx]/r_trkPt[r_trkIdx]));
                        fillWithOverFlow(h_trk_dzDdzerr, r_trkDz1[r_trkIdx]/r_trkDzError1[r_trkIdx]);
                        fillWithOverFlow(h_trk_chi2, r_trkChi2[r_trkIdx]/((int) r_trkNdof[r_trkIdx])
                                                /((int) r_trkNlayer[r_trkIdx]));
                        fillWithOverFlow(h_trk_nhits, r_trkNHit[r_trkIdx]);
                        fillWithOverFlow(h_trk_pterrDpt, std::abs(r_trkPtError[r_trkIdx]/r_trkPt[r_trkIdx]));
			h_cutflow->Fill(cuts::nocut);
                        //if(std::abs(r_trkDz1[r_trkIdx]/r_trkDzError1[r_trkIdx]) >= cut_dzSig) continue;
                        h_cutflow->Fill(cuts::dzSig);
                        if(r_trkChi2[r_trkIdx]/((int) r_trkNdof[r_trkIdx])
                                                /((int) r_trkNlayer[r_trkIdx]) >= cut_chi2) continue;
                        h_cutflow->Fill(cuts::chi2);
                        if(std::abs(r_trkPtError[r_trkIdx]/r_trkPt[r_trkIdx]) >= cut_ptRes) continue;
                        h_cutflow->Fill(cuts::ptRes);
                        if((int) r_trkNHit[r_trkIdx] < cut_nhits) continue;
                        h_cutflow->Fill(cuts::nhits);

			// if (r_trkAlgo[r_trkIdx]!=0) continue;
			// cout << r_trkPt[r_trkIdx] << ", " << r_trkEta[r_trkIdx] << ", " << r_trkPhi[r_trkIdx] << ", " << r_trkDxy1[r_trkIdx] << ", " << r_trkDxyError1[r_trkIdx] << ", " << r_trkDz1[r_trkIdx] << ", " << r_trkDzError1[r_trkIdx] << ", " << (int) r_trkNHit[r_trkIdx] << ", " << r_trkChi2[r_trkIdx] << ", " << r_trkPtError[r_trkIdx] << endl;
			r_goodtrack[r_idx].push_back(r_trkIdx);
			pt_avg_r += r_trkPt[r_trkIdx];
			h_pt_tot_r->Fill( r_trkPt[r_trkIdx] );
			h_dcaxyres_tot_r->Fill( r_trkDxy1[r_trkIdx]/r_trkDxyError1[r_trkIdx] );
			h_dcazres_tot_r->Fill( r_trkDz1[r_trkIdx]/r_trkDzError1[r_trkIdx]);
			h_etaphi_r->Fill( r_trkEta[r_trkIdx], r_trkPhi[r_trkIdx]);
			h_eta_r->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==0) h_eta_r_trkAlgo0->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==2) h_eta_r_trkAlgo2->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==4) h_eta_r_trkAlgo4->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==5) h_eta_r_trkAlgo5->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==6) h_eta_r_trkAlgo6->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==7) h_eta_r_trkAlgo7->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==8) h_eta_r_trkAlgo8->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==9) h_eta_r_trkAlgo9->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==10) h_eta_r_trkAlgo10->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==13) h_eta_r_trkAlgo13->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==22) h_eta_r_trkAlgo22->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==23) h_eta_r_trkAlgo23->Fill( r_trkEta[r_trkIdx]);
			if(r_trkAlgo[r_trkIdx]==24) h_eta_r_trkAlgo24->Fill( r_trkEta[r_trkIdx]);
		}
	}
	// h_pt_tot_r->Print();
	float pt_avg_rp = 0;
	for (int rp_idx = 0; rp_idx < rp_nEntries; ++rp_idx)
	{
		if(rp_idx%1000 == 0) std::cout << "Scanning rawprime tracks: " << rp_idx << "/" << rp_nEntries << std::endl;
		trackTree_rp->GetEntry(rp_idx);

		if ( !evtMatchedMap[rp_run][rp_lumi][rp_event]) continue;
                rp_goodtrack[rp_idx] = {};

		for (int rp_trkIdx = 0; rp_trkIdx < rp_nTrk; ++rp_trkIdx)
		{
			h_cutflow_p->Fill(cuts::nocut);
			//fillWithOverFlow(h_trk_p_dzDdzerr, std::abs(rp_trkDz1[rp_trkIdx]/rp_trkDzError1[rp_trkIdx]));
                        fillWithOverFlow(h_trk_p_chi2, rp_trkChi2[rp_trkIdx]/((int) rp_trkNdof[rp_trkIdx]));
                        fillWithOverFlow(h_trk_p_nhits, rp_trkNHit[rp_trkIdx]);
                        fillWithOverFlow(h_trk_p_pterrDpt, std::abs(rp_trkPtError[rp_trkIdx]/rp_trkPt[rp_trkIdx]));
			//if(std::abs(rp_trkDz1[rp_trkIdx]/rp_trkDzError1[rp_trkIdx]) >= cut_dzSig) continue;
		        h_cutflow_p->Fill(cuts::dzSig);
			if(rp_trkChi2[rp_trkIdx]/((int) rp_trkNdof[rp_trkIdx])
                                                /((int) rp_trkNlayer[rp_trkIdx]) >= cut_chi2) continue;
                        h_cutflow_p->Fill(cuts::chi2);
			if(std::abs(rp_trkPtError[rp_trkIdx]/rp_trkPt[rp_trkIdx]) >= cut_ptRes) continue;
			h_cutflow_p->Fill(cuts::ptRes);
                        if((int) rp_trkNHit[rp_trkIdx] < cut_nhits) continue;
			h_cutflow_p->Fill(cuts::nhits);
			// if (rp_trkAlgo[rp_trkIdx]!=0) continue;
		        rp_goodtrack[rp_idx].push_back(rp_trkIdx);
			pt_avg_rp += rp_trkPt[rp_trkIdx];
			h_pt_tot_rp->Fill( rp_trkPt[rp_trkIdx] );
			h_dcaxyres_tot_rp->Fill( rp_trkDxy1[rp_trkIdx]/rp_trkDxyError1[rp_trkIdx] );
			h_dcazres_tot_rp->Fill( rp_trkDz1[rp_trkIdx]/rp_trkDzError1[rp_trkIdx]);
			h_etaphi_rp->Fill( rp_trkEta[rp_trkIdx], rp_trkPhi[rp_trkIdx]);
			h_eta_rp->Fill( rp_trkEta[rp_trkIdx] );
			if(rp_trkAlgo[rp_trkIdx]==0) h_eta_rp_trkAlgo0->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==2) h_eta_rp_trkAlgo2->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==4) h_eta_rp_trkAlgo4->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==5) h_eta_rp_trkAlgo5->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==6) h_eta_rp_trkAlgo6->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==7) h_eta_rp_trkAlgo7->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==8) h_eta_rp_trkAlgo8->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==9) h_eta_rp_trkAlgo9->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==10) h_eta_rp_trkAlgo10->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==13) h_eta_rp_trkAlgo13->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==22) h_eta_rp_trkAlgo22->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==23) h_eta_rp_trkAlgo23->Fill( rp_trkEta[rp_trkIdx]);
			if(rp_trkAlgo[rp_trkIdx]==24) h_eta_rp_trkAlgo24->Fill( rp_trkEta[rp_trkIdx]);
		}

	}
	TH1F* h_matchedtrkPt = new TH1F("matched_trkPt", "; track p_{T} [GeV/#it{c}]; normalized distributions",
                                            numBins, customBins);
	TH1F* h_matchedtrkpPt = new TH1F("matched_trkpPt", "; track p_{T} [GeV/#it{c}]; normalized distributions",
                                            numBins, customBins);
	//TH1F* h_unmatchedtrkPt = new TH1F("unmatched_trkPt", "; track p_{T} [GeV/#it{c}]; normalized distributions",
          //                                  numBins, customBins);
	TH1F* h_unmatchedtrkPt = createhist("unmatched_trkPt", "; track p_{T} [GeV/#it{c}]; normalized distributions", numBins, customBins);
        TH1F* h_unmatchedtrkpPt = new TH1F("unmatched_trkpPt", "; track p_{T} [GeV/#it{c}]; normalized distributions",
                                            numBins, customBins);
	TH1F* h_deltar = createhist("deltaR", "; deltaR; yield", 50, {}, 0., 0.2);

	int not_matched_trk(0), not_matched_trk_p(0);
	int total_track_p(0), total_track(0);

	std::map<int, std::vector<int> >matched_trk_p;
	std::map<int, std::vector<int> >matched_trk;
	
	for(auto& evt_trk: r_goodtrack)
	  {
            int evt = evt_trk.first;
            trackTree_r->GetEntry(evt);
            
	    total_track += r_goodtrack[evt].size();
	    
	    //int matched_trk_idx;
            int matched_trk_p_idx;

	    int evt_p(-1);
	    for(auto& evt_trk_p: rp_goodtrack)
            {
             trackTree_rp->GetEntry(evt_trk_p.first);
             if ( r_event != rp_event) continue;
	     evt_p = evt_trk_p.first;
	     break;
	     }
	     if(evt_p == -1) {
	        std::cout << "common event is not found" << std::endl;
	        break;
	     }
	     total_track_p   += rp_goodtrack[evt_p].size();

	     for(unsigned int trk_idx=0; trk_idx<evt_trk.second.size(); trk_idx++) {
              
	        int r_trkidx = r_goodtrack[evt][trk_idx];
	        float drmin = 9999;
	        matched_trk_p_idx = -1;
	
	        for(unsigned int trk_p_idx=0; trk_p_idx<rp_goodtrack[evt_p].size(); trk_p_idx++) {
		    
		    int rp_trkidx = rp_goodtrack[evt_p][trk_p_idx];
		    if(matched_trk_p.find(evt_p) != matched_trk_p.end()) {
		       if(std::find(matched_trk_p[evt_p].begin(), matched_trk_p[evt_p].end(), rp_trkidx) != matched_trk_p[evt_p].end()) continue;
		    }
		    auto dr = deltaR(r_trkEta[r_trkidx], rp_trkEta[rp_trkidx], r_trkPhi[r_trkidx], rp_trkPhi[rp_trkidx]);
		    if (dr < 0.05 && dr < drmin) {
                          drmin = dr;
		          matched_trk_p_idx = rp_trkidx;
		    }
	         } // end of track_p loop
		 fillWithOverFlow(h_deltar, drmin);
		 if(matched_trk_p_idx != -1) {
		   if (matched_trk_p.find(evt_p) == matched_trk_p.end())
	              matched_trk_p[evt_p] = {};
		   h_matchedtrkPt->Fill(r_trkPt[r_trkidx]);
		   h_matchedtrkpPt->Fill(rp_trkPt[matched_trk_p_idx]);
		   fillWithOverFlow(h_matchtrack_pt_ratio, r_trkPt[r_trkidx]/rp_trkPt[matched_trk_p_idx]);
		   matched_trk_p[evt_p].push_back(matched_trk_p_idx);
		  }
		  else {
		    h_unmatchedtrkPt->Fill(r_trkPt[r_trkidx]);
		    not_matched_trk += 1;
		  }
	    } // end of track loop
	  } // end of event loop

        for(auto& evt_trk: rp_goodtrack)
          {
            int evt = evt_trk.first;
            trackTree_rp->GetEntry(evt);
            for(unsigned int trk_p_idx=0; trk_p_idx<evt_trk.second.size(); trk_p_idx++) {

                    int rp_trkidx = rp_goodtrack[evt][trk_p_idx];
                    if (matched_trk_p.find(evt) != matched_trk_p.end()) {
		       if(std::find(matched_trk_p[evt].begin(), matched_trk_p[evt].end(), rp_trkidx) != matched_trk_p[evt].end()) continue; // track_p is already matched with other track
		    }
		    h_unmatchedtrkpPt->Fill(rp_trkPt[rp_trkidx]);
                    not_matched_trk_p += 1;
            } // end of track loop
          } // end of event loop
       
	
	std::cout << "total track: " << total_track << "\t" << "total track_p: " << total_track_p << std::endl;
	std::cout << "not matched tracks: " << not_matched_trk << "\tnot matched track_p: " << not_matched_trk_p << std::endl;

	/* *******************************
	 * 1.0 Plotting total cluster distributions (matched events)
	 * *******************************/
	TLatex latex;

	TCanvas *canvSingle0 = new TCanvas("canvSingle0", "canvSingle0", 700, 600);
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);
	gErrorIgnoreLevel = kWarning;
	canvSingle0->GetPad(0)->SetMargin (0.18, 0.20, 0.12, 0.07);
	
	PlotStyle(h_pt_tot_rp); h_pt_tot_rp->SetLineColor(46);
	PlotStyle(h_pt_tot_r); h_pt_tot_r->SetLineWidth(0); h_pt_tot_r->SetFillColorAlpha(31, 0.4); h_pt_tot_r->SetLineColorAlpha(31, 0.4);

	PlotStyle(h_matchedtrkpPt); h_matchedtrkpPt->SetLineColor(46);
        PlotStyle(h_matchedtrkPt); h_matchedtrkPt->SetLineWidth(0); h_matchedtrkPt->SetFillColorAlpha(31, 0.4); h_matchedtrkPt->SetLineColorAlpha(31, 0.4);


	PlotStyle(h_dcaxyres_tot_rp); h_dcaxyres_tot_rp->SetLineColor(46);
	PlotStyle(h_dcaxyres_tot_r); h_dcaxyres_tot_r->SetLineWidth(0); h_dcaxyres_tot_r->SetFillColorAlpha(31, 0.4); h_dcaxyres_tot_r->SetLineColorAlpha(31, 0.4);
	PlotStyle(h_dcazres_tot_rp); h_dcazres_tot_rp->SetLineColor(46);
	PlotStyle(h_dcazres_tot_r); h_dcazres_tot_r->SetLineWidth(0); h_dcazres_tot_r->SetFillColorAlpha(31, 0.4); h_dcazres_tot_r->SetLineColorAlpha(31, 0.4);
	PlotStyle(h_etaphi_rp); PlotStyle(h_etaphi_r);
	PlotStyle(h_eta_rp); PlotStyle(h_eta_r);
	PlotStyle(h_eta_rp_trkAlgo0); PlotStyle(h_eta_r_trkAlgo0);
	PlotStyle(h_eta_rp_trkAlgo2); PlotStyle(h_eta_r_trkAlgo2);
	PlotStyle(h_eta_rp_trkAlgo4); PlotStyle(h_eta_r_trkAlgo4);
	PlotStyle(h_eta_rp_trkAlgo5); PlotStyle(h_eta_r_trkAlgo5);
	PlotStyle(h_eta_rp_trkAlgo6); PlotStyle(h_eta_r_trkAlgo6);
	PlotStyle(h_eta_rp_trkAlgo7); PlotStyle(h_eta_r_trkAlgo7);
	PlotStyle(h_eta_rp_trkAlgo8); PlotStyle(h_eta_r_trkAlgo8);
	PlotStyle(h_eta_rp_trkAlgo9); PlotStyle(h_eta_r_trkAlgo9);
	PlotStyle(h_eta_rp_trkAlgo10); PlotStyle(h_eta_r_trkAlgo10);
	PlotStyle(h_eta_rp_trkAlgo13); PlotStyle(h_eta_r_trkAlgo13);
	PlotStyle(h_eta_rp_trkAlgo22); PlotStyle(h_eta_r_trkAlgo22);
	PlotStyle(h_eta_rp_trkAlgo23); PlotStyle(h_eta_r_trkAlgo23);
	PlotStyle(h_eta_rp_trkAlgo24); PlotStyle(h_eta_r_trkAlgo24);

	h_pt_tot_rp->Print();
	h_pt_tot_r->Print();

	pt_avg_rp /= h_pt_tot_rp->Integral();
	pt_avg_r  /= h_pt_tot_r ->Integral();
	printf("Average pt = %.4f (RAW') %.4f (RAW)\n", pt_avg_rp, pt_avg_r);
	
	h_pt_tot_rp->Scale(1/h_pt_tot_rp->Integral());
	h_pt_tot_r->Scale(1/h_pt_tot_r->Integral());
	h_dcaxyres_tot_rp->Scale(1/h_dcaxyres_tot_rp->Integral());
	h_dcaxyres_tot_r->Scale(1/h_dcaxyres_tot_r->Integral());
	h_dcazres_tot_rp->Scale(1/h_dcazres_tot_rp->Integral());
	h_dcazres_tot_r->Scale(1/h_dcazres_tot_r->Integral());
	h_etaphi_rp->Scale(1/h_etaphi_rp->Integral());
	h_etaphi_r->Scale(1/h_etaphi_r->Integral());
	h_eta_rp->Scale(1/h_eta_rp->Integral()); h_eta_r->Scale(1/h_eta_r->Integral());
	h_eta_rp_trkAlgo0->Scale(1/h_eta_rp_trkAlgo0->Integral()); h_eta_r_trkAlgo0->Scale(1/h_eta_r_trkAlgo0->Integral());
	h_eta_rp_trkAlgo2->Scale(1/h_eta_rp_trkAlgo2->Integral()); h_eta_r_trkAlgo2->Scale(1/h_eta_r_trkAlgo2->Integral());
	h_eta_rp_trkAlgo4->Scale(1/h_eta_rp_trkAlgo4->Integral()); h_eta_r_trkAlgo4->Scale(1/h_eta_r_trkAlgo4->Integral());
	h_eta_rp_trkAlgo5->Scale(1/h_eta_rp_trkAlgo5->Integral()); h_eta_r_trkAlgo5->Scale(1/h_eta_r_trkAlgo5->Integral());
	h_eta_rp_trkAlgo6->Scale(1/h_eta_rp_trkAlgo6->Integral()); h_eta_r_trkAlgo6->Scale(1/h_eta_r_trkAlgo6->Integral());
	h_eta_rp_trkAlgo7->Scale(1/h_eta_rp_trkAlgo7->Integral()); h_eta_r_trkAlgo7->Scale(1/h_eta_r_trkAlgo7->Integral());
	h_eta_rp_trkAlgo8->Scale(1/h_eta_rp_trkAlgo8->Integral()); h_eta_r_trkAlgo8->Scale(1/h_eta_r_trkAlgo8->Integral());
	h_eta_rp_trkAlgo9->Scale(1/h_eta_rp_trkAlgo9->Integral()); h_eta_r_trkAlgo9->Scale(1/h_eta_r_trkAlgo9->Integral());
	h_eta_rp_trkAlgo10->Scale(1/h_eta_rp_trkAlgo10->Integral()); h_eta_r_trkAlgo10->Scale(1/h_eta_r_trkAlgo10->Integral());
	h_eta_rp_trkAlgo13->Scale(1/h_eta_rp_trkAlgo13->Integral()); h_eta_r_trkAlgo13->Scale(1/h_eta_r_trkAlgo13->Integral());
	h_eta_rp_trkAlgo22->Scale(1/h_eta_rp_trkAlgo22->Integral()); h_eta_r_trkAlgo22->Scale(1/h_eta_r_trkAlgo22->Integral());
	h_eta_rp_trkAlgo23->Scale(1/h_eta_rp_trkAlgo23->Integral()); h_eta_r_trkAlgo23->Scale(1/h_eta_r_trkAlgo23->Integral());
	h_eta_rp_trkAlgo24->Scale(1/h_eta_rp_trkAlgo24->Integral()); h_eta_r_trkAlgo24->Scale(1/h_eta_r_trkAlgo24->Integral());

	gPad->SetLogx(1);
	h_pt_tot_rp->GetXaxis()->SetRangeUser(1e-1,1e2);
	h_pt_tot_r->GetXaxis()->SetRangeUser(1e-1,1e2);
	h_pt_tot_rp->Draw("hist");
	h_pt_tot_r->Draw("hist same");
	h_pt_tot_rp->Draw("hist same");
	h_pt_tot_r->Write();
	h_pt_tot_rp->Write();
	TLegend* leg0 = new TLegend(.62, .6, .87, .8);
	leg0->AddEntry(h_pt_tot_r, "RAW", "f");
	leg0->AddEntry(h_pt_tot_rp, "RAW\'", "l");
	formatLegend(leg0);
	leg0->Draw();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.53,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.63,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle0->SaveAs(("../img/"+expTag+"_pt.pdf").c_str());

	canvSingle0->GetPad(0)->SetMargin (0.22, 0.16, 0.14, 0.07);
	TH1F* h_pt_tot_ratio = (TH1F*) h_pt_tot_rp->Clone("h_pt_tot_rp");
	h_pt_tot_ratio->GetXaxis()->SetTitleOffset(1.4);
	h_pt_tot_rp->SetMarkerColor(46);
	h_pt_tot_ratio->GetYaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
	h_pt_tot_ratio->GetYaxis()->SetRangeUser(0.90,1.10);
	Divide_w_sameDsets(h_pt_tot_rp, h_pt_tot_r, h_pt_tot_ratio);
	// h_pt_tot_ratio->Divide(h_pt_tot_r);
	h_pt_tot_ratio->Draw("e");
	h_pt_tot_ratio->Print("all");
	gStyle->SetGridStyle(3);  // Dashed grid lines
	gStyle->SetGridColor(kGray);
	gPad->SetGridy();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.26,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.36,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.37,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle0->SaveAs(("../img/"+expTag+"_ptRatio.pdf").c_str());

	canvSingle0->GetPad(0)->SetMargin(0.22, 0.16, 0.14, 0.07);
	//canvSingle0->SetLogy(true);
        gPad->SetLogx(0);
        h_matchedtrkpPt->Scale(1/h_matchedtrkpPt->Integral());
	h_matchedtrkPt->Scale(1/h_matchedtrkPt->Integral());
        h_matchedtrkpPt->Draw("hist");
        h_matchedtrkPt->Draw("hist same");
        //h_dcaxyres_tot_rp->Draw("hist same");
	//h_matchedtrackptR->Draw();
        gPad->SetGridy(0);
        leg0->Draw();
        latex.SetTextFont(63);
        latex.SetTextSize(31);
        latex.DrawLatexNDC(0.53,0.84,"CMS");
        latex.SetTextFont(53);
        latex.SetTextSize(22);
        latex.DrawLatexNDC(0.63,0.84,"Preliminary");
        latex.SetTextFont(43);
        latex.SetTextSize(24);
        //latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
        canvSingle0->SaveAs(("../img/"+expTag+"_matchedtrkPt.pdf").c_str());

	canvSingle0->GetPad(0)->SetMargin (0.22, 0.16, 0.14, 0.07);
	gPad->SetLogx(1);
	TH1F* h_matchedtrackptR = (TH1F*) h_matchedtrkpPt->Clone("matched_pt_ratio");
	h_matchedtrackptR->GetXaxis()->SetTitleOffset(1.4);
        //h_matchedtrackptR->Divide(h_matchedtrkPt);
	h_matchedtrackptR->SetMarkerColor(46);
        h_matchedtrackptR->GetYaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
        h_matchedtrackptR->GetYaxis()->SetRangeUser(0.90,1.10);
	Divide_w_sameDsets(h_matchedtrkPt, h_matchedtrkpPt, h_matchedtrackptR);
	h_matchedtrackptR->Draw("e");
	h_matchedtrackptR->Print("all");
        gPad->SetGridy();
        latex.SetTextFont(63);
        latex.SetTextSize(31);
        latex.DrawLatexNDC(0.26,0.84,"CMS");
        latex.SetTextFont(53);
        latex.SetTextSize(22);
        latex.DrawLatexNDC(0.36,0.84,"Preliminary");
        latex.SetTextFont(43);
        latex.SetTextSize(24);
        canvSingle0->SaveAs(("../img/"+expTag+"_matchedtrkPtRatio.pdf").c_str());
        
	canvSingle0->GetPad(0)->SetMargin(0.22, 0.16, 0.14, 0.07);
        canvSingle0->SetLogy(true);
        gPad->SetLogx(0);
	h_deltar->Scale(1/h_deltar->Integral());
        h_deltar->Draw("hist");
        gPad->SetGridy(0);
        latex.SetTextFont(63);
        latex.SetTextSize(31);
        latex.DrawLatexNDC(0.53,0.84,"CMS");
        latex.SetTextFont(53);
        latex.SetTextSize(22);
        latex.DrawLatexNDC(0.63,0.84,"Preliminary");
        latex.SetTextFont(43);
        latex.SetTextSize(24);
        //latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
        canvSingle0->SaveAs(("../img/"+expTag+"_deltaR.pdf").c_str());

        h_matchedtrkpPt->Write();
	h_unmatchedtrkpPt->Write();
	h_matchedtrkPt->Write();
	h_unmatchedtrkPt->Write();

	h_matchtrack_pt_ratio->Scale(1/h_matchtrack_pt_ratio->Integral());
	h_matchtrack_pt_ratio->Write();
	canvSingle0->GetPad(0)->SetMargin(0.22, 0.16, 0.14, 0.07);
        canvSingle0->SetLogy(true);
        gPad->SetLogx(0);
        h_matchtrack_pt_ratio->Draw("hist");
        gPad->SetGridy(0);
        latex.SetTextFont(63);
        latex.SetTextSize(31);
        latex.DrawLatexNDC(0.53,0.84,"CMS");
        latex.SetTextFont(53);
        latex.SetTextSize(22);
        latex.DrawLatexNDC(0.63,0.84,"Preliminary");
        latex.SetTextFont(43);
        latex.SetTextSize(24);
        //latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
        canvSingle0->SaveAs(("../img/"+expTag+"_ratio.pdf").c_str());

	h_trk_dzDdzerr->Write();
        h_trk_chi2->Write();
        h_trk_nhits->Write();
        h_trk_pterrDpt->Write();
	h_cutflow->Write();

	h_trk_p_dzDdzerr->Write();
        h_trk_p_chi2->Write();
        h_trk_p_nhits->Write();
        h_trk_p_pterrDpt->Write();
	h_cutflow_p->Write();
       
        h_deltar->Write();	
	delete h_matchedtrkpPt;
	delete h_matchedtrkPt;
	delete h_matchedtrackptR;
	delete h_deltar;
	delete h_matchtrack_pt_ratio;
        delete h_trk_dzDdzerr;
	delete h_trk_chi2;
	delete h_trk_nhits;
	delete h_trk_pterrDpt;
	delete h_cutflow;
	delete h_trk_p_dzDdzerr;
        delete h_trk_p_chi2;
        delete h_trk_p_nhits;
        delete h_trk_p_pterrDpt;
	delete h_cutflow_p;

	canvSingle0->GetPad(0)->SetMargin (0.18, 0.20, 0.12, 0.07);
	gPad->SetLogx(0);
	h_dcaxyres_tot_rp->Draw("hist");
	h_dcaxyres_tot_r->Draw("hist same");
	h_dcaxyres_tot_rp->Draw("hist same");
	gPad->SetGridy(0);
	leg0->Draw();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.53,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.63,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle0->SaveAs(("../img/"+expTag+"_dcaxyres.pdf").c_str());

	canvSingle0->GetPad(0)->SetMargin (0.22, 0.16, 0.14, 0.07);
	TH1F* h_dcaxyres_tot_ratio = (TH1F*) h_dcaxyres_tot_rp->Clone("h_dcaxyres_tot_rp");
	h_dcaxyres_tot_rp->SetMarkerColor(46);
	h_dcaxyres_tot_ratio->GetYaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
	h_dcaxyres_tot_ratio->GetYaxis()->SetRangeUser(0.95,1.05);
	Divide_w_sameDsets(h_dcaxyres_tot_rp, h_dcaxyres_tot_r, h_dcaxyres_tot_ratio);
	//h_dcaxyres_tot_ratio->Divide(h_dcaxyres_tot_r);
	h_dcaxyres_tot_ratio->Draw("e");
	gPad->SetGridy();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.26,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.36,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.37,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle0->SaveAs(("../img/"+expTag+"_dcaxyresRatio.pdf").c_str());

	canvSingle0->GetPad(0)->SetMargin (0.18, 0.20, 0.12, 0.07);
	h_dcazres_tot_rp->Draw("hist");
	h_dcazres_tot_r->Draw("hist same");
	h_dcazres_tot_rp->Draw("hist same");
	gPad->SetGridy(0);
	leg0->Draw();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.53,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.63,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle0->SaveAs(("../img/"+expTag+"_dcazres.pdf").c_str());
	
	canvSingle0->GetPad(0)->SetMargin (0.22, 0.16, 0.14, 0.07);
	TH1F* h_dcazres_tot_ratio = (TH1F*) h_dcazres_tot_rp->Clone("h_dcazres_tot_rp");
	h_dcazres_tot_rp->SetMarkerColor(46);
	h_dcazres_tot_ratio->GetYaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
	h_dcazres_tot_ratio->GetYaxis()->SetRangeUser(0.95,1.05);
	Divide_w_sameDsets(h_dcazres_tot_rp, h_dcazres_tot_r, h_dcazres_tot_ratio);
	//h_dcazres_tot_ratio->Divide(h_dcazres_tot_r);
	h_dcazres_tot_ratio->Draw("e");
	gPad->SetGridy();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.26,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.36,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.37,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle0->SaveAs(("../img/"+expTag+"_dcazresRatio.pdf").c_str());

	delete canvSingle0;

	TCanvas *canvSingle = new TCanvas("canvSingle", "canvSingle", 700, 600);
	gErrorIgnoreLevel = kWarning;
	canvSingle->GetPad(0)->SetMargin (0.15, 0.23, 0.12, 0.07);
	canvSingle->cd();
	h_etaphi_rp->Divide(h_etaphi_r);
	h_etaphi_rp->GetYaxis()->SetTitleOffset(1.6);
	h_etaphi_rp->GetZaxis()->SetTitleOffset(1.8);
	h_etaphi_rp->GetZaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
	h_etaphi_rp->Draw("COLZ");
	h_etaphi_rp->GetZaxis()->SetRangeUser(0.95,1.05);
	// leg0 = new TLegend(.15, .7, .4, .8);
	// leg0->AddEntry((TObject*) 0, "#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}", "");
	// formatLegend(leg0);
	// leg0->Draw();
	latex.SetTextFont(63);
	latex.SetTextSize(31);
	latex.DrawLatexNDC(0.19,0.84,"CMS");
	latex.SetTextFont(53);
	latex.SetTextSize(22);
	latex.DrawLatexNDC(0.29,0.84,"Preliminary");
	latex.SetTextFont(43);
	latex.SetTextSize(24);
	latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
	canvSingle->SaveAs(("../img/"+expTag+"_etaphi.pdf").c_str());

	h_eta_rp->Divide(h_eta_r);
	h_eta_rp->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio.pdf").c_str());

	gStyle->SetOptTitle(1);
	h_eta_rp_trkAlgo0->Divide(h_eta_r_trkAlgo0);
	h_eta_rp_trkAlgo0->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo0.pdf").c_str());

	h_eta_rp_trkAlgo2->Divide(h_eta_r_trkAlgo2);
	h_eta_rp_trkAlgo2->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo2.pdf").c_str());

	h_eta_rp_trkAlgo4->Divide(h_eta_r_trkAlgo4);
	h_eta_rp_trkAlgo4->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo4.pdf").c_str());

	h_eta_rp_trkAlgo5->Divide(h_eta_r_trkAlgo5);
	h_eta_rp_trkAlgo5->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo5.pdf").c_str());

	h_eta_rp_trkAlgo6->Divide(h_eta_r_trkAlgo6);
	h_eta_rp_trkAlgo6->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo6.pdf").c_str());

	h_eta_rp_trkAlgo7->Divide(h_eta_r_trkAlgo7);
	h_eta_rp_trkAlgo7->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo7.pdf").c_str());

	h_eta_rp_trkAlgo8->Divide(h_eta_r_trkAlgo8);
	h_eta_rp_trkAlgo8->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo8.pdf").c_str());

	h_eta_rp_trkAlgo9->Divide(h_eta_r_trkAlgo9);
	h_eta_rp_trkAlgo9->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo9.pdf").c_str());

	h_eta_rp_trkAlgo10->Divide(h_eta_r_trkAlgo10);
	h_eta_rp_trkAlgo10->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo10.pdf").c_str());

	h_eta_rp_trkAlgo13->Divide(h_eta_r_trkAlgo13);
	h_eta_rp_trkAlgo13->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo13.pdf").c_str());

	h_eta_rp_trkAlgo22->Divide(h_eta_r_trkAlgo22);
	h_eta_rp_trkAlgo22->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo22.pdf").c_str());

	h_eta_rp_trkAlgo23->Divide(h_eta_r_trkAlgo23);
	h_eta_rp_trkAlgo23->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo23.pdf").c_str());

	h_eta_rp_trkAlgo24->Divide(h_eta_r_trkAlgo24);
	h_eta_rp_trkAlgo24->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo24.pdf").c_str());

	delete canvSingle;

	return 0;
}

