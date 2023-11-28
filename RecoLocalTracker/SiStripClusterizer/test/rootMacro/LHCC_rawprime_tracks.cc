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
		float _r 	= num->GetBinContent(_x)/denom->GetBinContent(_x);
		float _n_relErr = TMath::Abs(   num->GetBinError(_x)/  num->GetBinContent(_x) );
		float _d_relErr = TMath::Abs( denom->GetBinError(_x)/denom->GetBinContent(_x) );
		float _r_err 	= TMath::Abs(_r) * ( (_n_relErr > _d_relErr) ? _n_relErr : _d_relErr  );
		ratio->SetBinContent(_x, _r);
		ratio->SetBinError(_x, _r_err);
	}
}

int main(int argc, char const *argv[])
{
	string expTag(argv[1]);
	test thisTest(expTag);
	bool faster = (argc<=2)? true: atoi(argv[2]);

	/* *******************************
	 * 0. Loading matched events info & track trees
	 * *******************************/
	// Reading matched events
	map< int, map< int, map<int, bool> > > evtMatchedMap;
	
	if (thisTest.matchtxt!="all")
	{
		printf("Reading: %s\n", thisTest.matchtxt.c_str());
		std::ifstream inputFile(thisTest.matchtxt.c_str());

		// Check if the file is successfully opened
		if (!inputFile.is_open()) {
			std::cerr << "Error opening the file." << std::endl;
			return 1; // Exit with an error code
		}

		// Read the file line by line
		std::string line;
		int countMatchedEvts = 0;
		while (std::getline(inputFile, line)) {
			// Create a string stream from the line
			std::istringstream iss(line);

			// Variables to store data
			int _run;
			int _event;
			int _lumi;

			// Read data from the stringstream
			if (iss >> _run >> _event >> _lumi) {
				// Print the read data
				evtMatchedMap[_run][_lumi][_event] = true;
				// std::cout << "Run: " << _run << ", Event: " << _event << ", Lumi: " << _lumi << std::endl;
				countMatchedEvts ++;
			} else {
				std::cerr << "Error reading data from line: " << line << std::endl;
			}
		}

		printf("Finished reading: %d matched events\n", countMatchedEvts);
		
		// Close the file
		inputFile.close();
	}


	TChain* trackTree_rp 	= new TChain("anaTrack/trackTree");
	std::vector<std::string> fileList1 = getFileList(thisTest.f1name.c_str());
	int fileListLen = (5000<fileList1.size())? 5000: fileList1.size();
	for (unsigned int fI=0; fI<fileListLen; fI++) {
		trackTree_rp->Add(fileList1.at(fI).c_str());
	}

	TChain* trackTree_r 	= new TChain("anaTrack/trackTree");
	std::vector<std::string> fileList2 = getFileList(thisTest.f2name.c_str());
	fileListLen = (5000<fileList2.size())? 5000: fileList2.size();
	for (unsigned int fI=0; fI<fileListLen; fI++) {
		trackTree_r->Add(fileList2.at(fI).c_str());
	}

	const static int nMax = 40000;
	////// for rawprime
	int rp_event;
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
	
	unsigned char rp_trkAlgo[nMax];
	unsigned char rp_trkNHit[nMax];
	unsigned char rp_trkNdof[nMax];
	unsigned char rp_trkNlayer[nMax];
	float       rp_trkChi2[nMax];
	float       rp_trkPtError[nMax];


	////// for raw
	int r_event;
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
	
	unsigned char r_trkAlgo[nMax];
	unsigned char r_trkNHit[nMax];
	unsigned char r_trkNdof[nMax];
	unsigned char r_trkNlayer[nMax];
	float       r_trkChi2[nMax];
	float       r_trkPtError[nMax];

	trackTree_rp->SetBranchAddress("nEv", 	&rp_event);
	trackTree_rp->SetBranchAddress("nRun",  &rp_run);
	trackTree_rp->SetBranchAddress("nLumi", &rp_lumi);
	trackTree_rp->SetBranchAddress("nTrk",  &rp_nTrk);

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

	trackTree_r->SetBranchAddress("nEv", 	&r_event);
	trackTree_r->SetBranchAddress("nRun",   &r_run);
	trackTree_r->SetBranchAddress("nLumi",  &r_lumi);
	trackTree_r->SetBranchAddress("nTrk",  	&r_nTrk);

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

	// Define custom bin edges
	const int numBins = 7;
	Double_t customBins[numBins + 1] = {0.1, 0.5, 1.0, 2.0, 6.0, 9.0, 30.0, 100.0};

	TH1F * h_pt_tot_r      = new TH1F( "RAW",
	                                    "; track p_{T} [GeV/#it{c}]; normalized distributions",  
	                                    numBins, customBins);
	TH1F * h_dcaxyres_tot_r     = new TH1F( "RAW",
	                                    "; track DCA xy/#sigma; normalized distributions", //  (0.5 < p_{T} < 0.9 [GeV/#it{c}]); normalized distributions",  
	                                    30, -5., 5. );
	TH1F * h_dcazres_tot_r = new TH1F( "RAW",
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

	TH1F * h_pt_tot_rp      = new TH1F( "RAW'",
	                                    "; track p_{T} [GeV/#it{c}]; normalized distributions",  
	                                    numBins, customBins);
	TH1F * h_dcaxyres_tot_rp     = new TH1F( "RAW'",
	                                    "; track DCA xy/#sigma; normalized distributions", //  (0.5 < p_{T} < 0.9 [GeV/#it{c}]); normalized distributions",  
	                                    30, -5., 5. );
	TH1F * h_dcazres_tot_rp = new TH1F( "RAW'",
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


	float pt_avg_r = 0;
	const Int_t r_nEntries = trackTree_r->GetEntries();
	for (int r_idx = 0; r_idx < r_nEntries; ++r_idx)
	{
		if(r_idx%1000 == 0) std::cout << "Scanning raw tracks: " << r_idx << "/" << r_nEntries << std::endl;
		trackTree_r->GetEntry(r_idx);

		if (thisTest.matchtxt!="all" && !evtMatchedMap[r_run][r_lumi][r_event]) continue;

		// trackTree_r->Show(r_idx);

		for (int r_trkIdx = 0; r_trkIdx < r_nTrk; ++r_trkIdx)
		{
			// if (r_trkAlgo[r_trkIdx]!=0) continue;
			// cout << r_trkPt[r_trkIdx] << ", " << r_trkEta[r_trkIdx] << ", " << r_trkPhi[r_trkIdx] << ", " << r_trkDxy1[r_trkIdx] << ", " << r_trkDxyError1[r_trkIdx] << ", " << r_trkDz1[r_trkIdx] << ", " << r_trkDzError1[r_trkIdx] << ", " << (int) r_trkNHit[r_trkIdx] << ", " << r_trkChi2[r_trkIdx] << ", " << r_trkPtError[r_trkIdx] << endl;

			if ( std::abs(r_trkDz1[r_trkIdx]/r_trkDzError1[r_trkIdx]) 	< cut_dzSig && 
			     // std::abs(r_trkDxy1[r_trkIdx]/r_trkDxyError1[r_trkIdx]) 	< cut_dxySig && 
			     r_trkChi2[r_trkIdx]/((int) r_trkNdof[r_trkIdx])
			     			/((int) r_trkNlayer[r_trkIdx]) 		< cut_chi2 && 
			     (int) r_trkNHit[r_trkIdx] 					>=cut_nhits && 
			     std::abs(r_trkPtError[r_trkIdx]/r_trkPt[r_trkIdx]) 	< cut_ptRes ) // &&
			     // r_trkAlgo[r_trkIdx]!=0 &&
			     // r_trkAlgo[r_trkIdx]!=4 &&
			     // r_trkAlgo[r_trkIdx]!=22
			     // ) 
			{
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
	}
	// h_pt_tot_r->Print();

	float pt_avg_rp = 0;
	const Int_t rp_nEntries = trackTree_rp->GetEntries();
	for (int rp_idx = 0; rp_idx < rp_nEntries; ++rp_idx)
	{
		if(rp_idx%1000 == 0) std::cout << "Scanning rawprime tracks: " << rp_idx << "/" << rp_nEntries << std::endl;
		trackTree_rp->GetEntry(rp_idx);

		if (thisTest.matchtxt!="all" && !evtMatchedMap[rp_run][rp_lumi][rp_event]) continue;

		for (int rp_trkIdx = 0; rp_trkIdx < rp_nTrk; ++rp_trkIdx)
		{
			// if (rp_trkAlgo[rp_trkIdx]!=0) continue;
			if ( std::abs(rp_trkDz1[rp_trkIdx]/rp_trkDzError1[rp_trkIdx]) 	< cut_dzSig && 
			     // std::abs(rp_trkDxy1[rp_trkIdx]/rp_trkDxyError1[rp_trkIdx]) < cut_dxySig && 
			     rp_trkChi2[rp_trkIdx]/((int) rp_trkNdof[rp_trkIdx])
			     			  /((int) rp_trkNlayer[rp_trkIdx]) 	< cut_chi2 && 
			     (int) rp_trkNHit[rp_trkIdx] 				>=cut_nhits && 
			     std::abs(rp_trkPtError[rp_trkIdx]/rp_trkPt[rp_trkIdx]) 	< cut_ptRes ) // &&
			     // rp_trkAlgo[rp_trkIdx]!=0 &&
			     // rp_trkAlgo[rp_trkIdx]!=4 &&
			     // rp_trkAlgo[rp_trkIdx]!=22
			     // )
			{
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

	}

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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_pt.pdf /tmp/").c_str());

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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_ptRatio.pdf /tmp/").c_str());

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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_dcaxyres.pdf /tmp/").c_str());

	canvSingle0->GetPad(0)->SetMargin (0.22, 0.16, 0.14, 0.07);
	TH1F* h_dcaxyres_tot_ratio = (TH1F*) h_dcaxyres_tot_rp->Clone("h_dcaxyres_tot_rp");
	h_dcaxyres_tot_rp->SetMarkerColor(46);
	h_dcaxyres_tot_ratio->GetYaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
	h_dcaxyres_tot_ratio->GetYaxis()->SetRangeUser(0.95,1.05);
	Divide_w_sameDsets(h_dcaxyres_tot_rp, h_dcaxyres_tot_r, h_dcaxyres_tot_ratio);
	// h_dcaxyres_tot_ratio->Divide(h_dcaxyres_tot_r);
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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_dcaxyresRatio.pdf /tmp/").c_str());

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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_dcazres.pdf /tmp/").c_str());
	
	canvSingle0->GetPad(0)->SetMargin (0.22, 0.16, 0.14, 0.07);
	TH1F* h_dcazres_tot_ratio = (TH1F*) h_dcazres_tot_rp->Clone("h_dcazres_tot_rp");
	h_dcazres_tot_rp->SetMarkerColor(46);
	h_dcazres_tot_ratio->GetYaxis()->SetTitle("#frac{(N_{track})_{RAW\'}}{(N_{track})_{RAW}}");
	h_dcazres_tot_ratio->GetYaxis()->SetRangeUser(0.95,1.05);
	Divide_w_sameDsets(h_dcazres_tot_rp, h_dcazres_tot_r, h_dcazres_tot_ratio);
	// h_dcazres_tot_ratio->Divide(h_dcazres_tot_r);
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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_dcazresRatio.pdf /tmp/").c_str());

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
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaphi.pdf /tmp/").c_str());

	h_eta_rp->Divide(h_eta_r);
	h_eta_rp->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio.pdf /tmp/").c_str());

	gStyle->SetOptTitle(1);
	h_eta_rp_trkAlgo0->Divide(h_eta_r_trkAlgo0);
	h_eta_rp_trkAlgo0->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo0.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo0.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo2->Divide(h_eta_r_trkAlgo2);
	h_eta_rp_trkAlgo2->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo2.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo2.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo4->Divide(h_eta_r_trkAlgo4);
	h_eta_rp_trkAlgo4->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo4.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo4.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo5->Divide(h_eta_r_trkAlgo5);
	h_eta_rp_trkAlgo5->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo5.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo5.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo6->Divide(h_eta_r_trkAlgo6);
	h_eta_rp_trkAlgo6->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo6.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo6.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo7->Divide(h_eta_r_trkAlgo7);
	h_eta_rp_trkAlgo7->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo7.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo7.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo8->Divide(h_eta_r_trkAlgo8);
	h_eta_rp_trkAlgo8->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo8.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo8.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo9->Divide(h_eta_r_trkAlgo9);
	h_eta_rp_trkAlgo9->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo9.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo9.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo10->Divide(h_eta_r_trkAlgo10);
	h_eta_rp_trkAlgo10->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo10.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo10.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo13->Divide(h_eta_r_trkAlgo13);
	h_eta_rp_trkAlgo13->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo13.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo13.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo22->Divide(h_eta_r_trkAlgo22);
	h_eta_rp_trkAlgo22->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo22.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo22.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo23->Divide(h_eta_r_trkAlgo23);
	h_eta_rp_trkAlgo23->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo23.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo23.pdf /tmp/").c_str());

	h_eta_rp_trkAlgo24->Divide(h_eta_r_trkAlgo24);
	h_eta_rp_trkAlgo24->Draw("e");
	gPad->SetGridy();
	canvSingle0->SaveAs(("../img/"+expTag+"_etaRatio_trkAlgo24.pdf").c_str());
	system(("dropbox_uploader.sh upload ../img/"+expTag+"_etaRatio_trkAlgo24.pdf /tmp/").c_str());

	delete canvSingle;

	return 0;
}

