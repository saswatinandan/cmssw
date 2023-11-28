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
#include "TBranch.h"
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
    float y_title_offset  = 2.1;

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

	h->GetZaxis()->SetTitleOffset  (1.2);
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
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_run375007_run375055_HIPhysicsRawPrime0.txt";
			f2name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run374345_run374347_run374354_run375007_run375055_HIMinimumBias0.txt";
			matchtxt = "lumi_run_event_txt/matching_rows_check_run374345_run374347_run374354_run375007_run375055.txt";
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
		} else if (in_expTag=="LHCC_run375790_run375820_run375823") 
		{
			cout << "run375790, run375820, run375823" << endl;
			f1name = "/eos/cms/store/group/phys_heavyions/yuchenc/TrackingEffTables2022PbPbRun/run375790_run375820_run375823_HIPhysicsRawPrime0.txt";
			f2name = "/afs/cern.ch/user/y/yuchenc/tracking/CMSSW_13_2_5_patch1/src/HITrackingStudies/HITrackingStudies/test/tracking_ntuple/LHCC_run375790_run375820_run375823_HIMinimumBias0.root";
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

int main(int argc, char const *argv[])
{
	string expTag(argv[1]);
	test thisTest(expTag);
	bool faster = (argc<=2)? true: atoi(argv[2]);

	/* *******************************
	 * 0. Loading matched events info & track trees
	 * *******************************/
	// Reading matched events
	printf("Reading: %s\n", thisTest.matchtxt.c_str());
	std::ifstream inputFile(thisTest.matchtxt.c_str());

	// Check if the file is successfully opened
	if (!inputFile.is_open()) {
		std::cerr << "Error opening the file." << std::endl;
		return 1; // Exit with an error code
	}

	// Read the file line by line
	map< int, map< int, map<int, bool> > > evtMatchedMap;
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

	////// for rawprime
	int rp_event;
	int rp_run;
	int rp_lumi;
	int rp_nTrk;

	////// for raw
	int r_event;
	int r_run;
	int r_lumi;
	int r_nTrk;

	trackTree_rp->SetBranchAddress("nEv", 	&rp_event);
	trackTree_rp->SetBranchAddress("nRun",  &rp_run);
	trackTree_rp->SetBranchAddress("nLumi", &rp_lumi);
	trackTree_rp->SetBranchAddress("nTrk",  &rp_nTrk);

	trackTree_r->SetBranchAddress("nEv", 	&r_event);
	trackTree_r->SetBranchAddress("nRun",   &r_run);
	trackTree_r->SetBranchAddress("nLumi",  &r_lumi);
	trackTree_r->SetBranchAddress("nTrk",  	&r_nTrk);

	// Create a new ROOT file for the selected entries
	TFile *outputFile_rp = thisTest.f1name.find(".txt")!=string::npos? new TFile(thisTest.f1name.replace(thisTest.f1name.find(".txt"),4,"_selected.root").c_str(), "RECREATE") :
									   new TFile(thisTest.f1name.replace(thisTest.f1name.find(".root"),5,"_selected.root").c_str(), "RECREATE");

	TDirectory *_d_rp = outputFile_rp->mkdir("anaTrack");
	_d_rp->cd();

	TTree *selectedTree_rp = trackTree_rp->CloneTree(0);

	// Loop over entries in the original tree and copy all entries to the new tree
	for (Long64_t entry = 0; entry < trackTree_rp->GetEntries(); ++entry) 
	{
		trackTree_rp->GetEntry(entry);
		if (rp_nTrk==0) continue;
		if (!evtMatchedMap[rp_run][rp_lumi][rp_event]) continue;
		selectedTree_rp->Fill();
	}

	outputFile_rp->Write();
	outputFile_rp->Close();

	// Create a new ROOT file for the selected entries
	TFile *outputFile_r = thisTest.f2name.find(".txt")!=string::npos? new TFile(thisTest.f2name.replace(thisTest.f2name.find(".txt"),4,"_selected.root").c_str(), "RECREATE") :
									  new TFile(thisTest.f2name.replace(thisTest.f2name.find(".root"),5,"_selected.root").c_str(), "RECREATE");

	TDirectory *_d_r = outputFile_r->mkdir("anaTrack");
	_d_r->cd();

	TTree *selectedTree_r = trackTree_r->CloneTree(0);

	// Loop over entries in the original tree and copy all entries to the new tree
	for (Long64_t entry = 0; entry < trackTree_r->GetEntries(); ++entry) 
	{
		trackTree_r->GetEntry(entry);
		if (r_nTrk==0) continue;
		if (!evtMatchedMap[r_run][r_lumi][r_event]) continue;
		selectedTree_r->Fill();
	}

	outputFile_r->Write();
	outputFile_r->Close();

	return 0;
}

