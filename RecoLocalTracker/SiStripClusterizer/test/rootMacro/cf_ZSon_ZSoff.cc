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


using namespace std;

template<class T>
void PlotStyle(T* h)
{
	//fonts
    int defaultFont       = 43;
    float x_title_size    = 20;
    float y_title_size    = 20;

    float x_title_offset  = 1.5;
    float y_title_offset  = 2.1;

    float label_size      = 20;
    float label_offset    = 0.013;

	h->GetXaxis()->SetLabelFont(defaultFont);
	h->GetXaxis()->SetTitleFont(defaultFont);
	h->GetYaxis()->SetLabelFont(defaultFont);
	h->GetYaxis()->SetTitleFont(defaultFont);

	// gStyle->SetTitleFontSize(16);
	h->SetTitleOffset  (0);


	h->GetYaxis()->SetTitleOffset  (y_title_offset);
	h->GetYaxis()->CenterTitle();
	h->GetYaxis()->SetTitleSize    (x_title_size);
	h->GetYaxis()->SetLabelOffset  (label_offset);
	h->GetYaxis()->SetLabelSize    (label_size);
	h->GetYaxis()->SetNdivisions(508);

	h->GetXaxis()->SetTitleOffset  (x_title_offset);
	h->GetXaxis()->CenterTitle();
	h->GetXaxis()->SetTitleSize    (x_title_size);
	h->GetXaxis()->SetLabelOffset  (label_offset);
	h->GetXaxis()->SetLabelSize    (label_size);
	h->GetXaxis()->SetNdivisions(508);

	h->GetZaxis()->SetTitleOffset  (x_title_offset);
	h->GetZaxis()->CenterTitle();
	h->GetZaxis()->SetTitleSize    (x_title_size);
	h->GetZaxis()->SetLabelOffset  (label_offset);
	h->GetZaxis()->SetLabelSize    (label_size);

	h->SetLineWidth(2);
	// h->SetMinimum(-0.001);

}


void formatLegend(TLegend* leg, double textsize=17)
{
        leg->SetBorderSize(0);
        leg->SetTextFont(43);
        leg->SetTextSize(textsize);
        leg->SetFillStyle(0);
        leg->SetFillColor(0);
        leg->SetLineColor(0);
}

struct cluster{
	int idx;
	unsigned int event;
	int run;
	int lumi;

	// for approxCluster
	uint32_t    detId;
	uint16_t    firstStrip;
	uint16_t    endStrip;
	float       barycenter;
	uint16_t    size;
	int         charge;

	int      	matchLevel; // 1: exact 
							// 2: having overlap

	cluster(): idx(0), event(0), run(0), lumi(0), detId(0), 
		  firstStrip(0), endStrip(0), barycenter(0.), size(0), charge(0),
		  matchLevel(-1) {};
	cluster(int in_idx,
			unsigned int in_event,
			int in_run,
			int in_lumi,
			// for approxCluster
			uint32_t    in_detId,
			uint16_t    in_firstStrip,
			uint16_t    in_endStrip,
			float       in_barycenter,
			uint16_t    in_size,
			int in_charge) :
			idx(in_idx),
			event(in_event),
			run(in_run),
			lumi(in_lumi),
			  // for approxCluster
			detId(in_detId),
			firstStrip(in_firstStrip),
			endStrip(in_endStrip),
			barycenter(in_barycenter),
			size(in_size),
			charge(in_charge),
			matchLevel(-1) {};

	void print()
	{
		cout << "idx: " << idx <<
				", event: " << event <<
				", run: " << run <<
				", lumi: " << lumi <<

				", detId: " << detId <<
				", firstStrip: " << firstStrip <<
				", endStrip: " << endStrip <<
				", barycenter: " << barycenter <<
				", size: " << size <<
				", charge: " << charge << 
				", matchLevel: " << matchLevel << endl;
	}

};



int main(int argc, char const *argv[])
{
	int chooseEventId = atoi(argv[1]);

	TFile* f1   		= TFile::Open("../output/ZSon_sep19_2_2_dump_raw.root", "read");
	TDirectoryFile* _1  	= (TDirectoryFile*) f1->Get("sep19_2_2_dump_raw");
	TTree* offlineClusterTree= (TTree*) _1->Get("offlineClusterTree");

	TFile* f2   		= TFile::Open("../output/ZSoff_sep19_2_2_dump_raw.root", "read");
	TDirectoryFile* _2  	= (TDirectoryFile*) f2->Get("sep19_2_2_dump_raw");
	TTree* offlineClusterTree_2= (TTree*) _2->Get("offlineClusterTree");

	const static int nMax = 300000;


	////// for ZS on
	unsigned int r_event;
	int r_run;
	int r_lumi;

	// for stripCluster
	uint32_t    r_detId;
	uint16_t    r_firstStrip;
	uint16_t    r_endStrip;
	float       r_barycenter;
	uint16_t    r_size;
	int         r_charge;

	float       r_hitX[nMax];
	float       r_hitY[nMax];
	uint16_t    r_channel[nMax];
	uint16_t    r_adc[nMax];

	offlineClusterTree->SetBranchAddress("event", &r_event);
	offlineClusterTree->SetBranchAddress("run",   &r_run);
	offlineClusterTree->SetBranchAddress("lumi",  &r_lumi);

	offlineClusterTree->SetBranchAddress("detId", &r_detId);
	offlineClusterTree->SetBranchAddress("firstStrip", &r_firstStrip);
	offlineClusterTree->SetBranchAddress("endStrip", &r_endStrip);
	offlineClusterTree->SetBranchAddress("barycenter", &r_barycenter);
	offlineClusterTree->SetBranchAddress("size", &r_size);
	offlineClusterTree->SetBranchAddress("charge", &r_charge);

	offlineClusterTree->SetBranchAddress("x", r_hitX);
	offlineClusterTree->SetBranchAddress("y", r_hitY);
	offlineClusterTree->SetBranchAddress("channel", r_channel);
	offlineClusterTree->SetBranchAddress("adc", r_adc);


	////// for ZS off
	unsigned int r_event_2;
	int r_run_2;
	int r_lumi_2;

	// for stripCluster
	uint32_t    r_detId_2;
	uint16_t    r_firstStrip_2;
	uint16_t    r_endStrip_2;
	float       r_barycenter_2;
	uint16_t    r_size_2;
	int         r_charge_2;

	float       r_hitX_2[nMax];
	float       r_hitY_2[nMax];
	uint16_t    r_channel_2[nMax];
	uint16_t    r_adc_2[nMax];

	offlineClusterTree_2->SetBranchAddress("event", &r_event_2);
	offlineClusterTree_2->SetBranchAddress("run",   &r_run_2);
	offlineClusterTree_2->SetBranchAddress("lumi",  &r_lumi_2);

	offlineClusterTree_2->SetBranchAddress("detId", &r_detId_2);
	offlineClusterTree_2->SetBranchAddress("firstStrip", &r_firstStrip_2);
	offlineClusterTree_2->SetBranchAddress("endStrip", &r_endStrip_2);
	offlineClusterTree_2->SetBranchAddress("barycenter", &r_barycenter_2);
	offlineClusterTree_2->SetBranchAddress("size", &r_size_2);
	offlineClusterTree_2->SetBranchAddress("charge", &r_charge_2);

	offlineClusterTree_2->SetBranchAddress("x", r_hitX_2);
	offlineClusterTree_2->SetBranchAddress("y", r_hitY_2);
	offlineClusterTree_2->SetBranchAddress("channel", r_channel_2);
	offlineClusterTree_2->SetBranchAddress("adc", r_adc_2);



	TCanvas *canvall = new TCanvas("canvall", "canvall", 600*3, 600*1);
	gStyle->SetOptTitle(0);
	gErrorIgnoreLevel = kWarning;
	canvall->Divide(3,1,0.001,0.001);

	TH1F * h_width_tot_on_all      = new TH1F( "ZS on", 
	                                    "; width; yield",  
	                                    50, 0., 50. );
	TH1F * h_charge_tot_on_all     = new TH1F( "charge_tot_on_all", 
	                                    "; charge; yield",  
	                                    100, 0., 700. );
	TH1F * h_barycenter_tot_on_all = new TH1F( "barycenter_tot_on_all", 
	                                    "; barycenter; yield",  
	                                    100, 0., 950. );

	TH1F * h_width_tot_off_all      = new TH1F( "ZS off", 
	                                    "; width; yield",  
	                                    50, 0., 50. );
	TH1F * h_charge_tot_off_all     = new TH1F( "charge_tot_off_all", 
	                                    "; charge; yield",  
	                                    100, 0., 700. );
	TH1F * h_barycenter_tot_off_all = new TH1F( "barycenter_tot_off_all", 
	                                    "; barycenter; yield",  
	                                    100, 0., 950. );

	map< int, map< int, map<int, cluster> > > r_dict; 	// event, detId, idx
	const Int_t r_nEntries = offlineClusterTree->GetEntries();
	for (int sc_idx = 0; sc_idx < r_nEntries; ++sc_idx)
	{
		if(sc_idx%1000000 == 0) std::cout << "Scanning ZS on clusters: " << sc_idx << "/" << r_nEntries << std::endl;
		offlineClusterTree->GetEntry(sc_idx);

		r_dict[ r_event ][ r_detId ][ sc_idx ] = cluster( sc_idx, r_event, r_run, r_lumi,
												r_detId, r_firstStrip, r_endStrip, r_barycenter,
												r_size, r_charge );
		h_width_tot_on_all->Fill( r_size );
		h_charge_tot_on_all->Fill( r_charge );
		h_barycenter_tot_on_all->Fill( r_barycenter );
	}

	map< int, map< int, map<int, cluster> > > r_dict_2; 	// event, detId, idx
	const Int_t r_nEntries_2 = offlineClusterTree_2->GetEntries();
	for (int sc_idx = 0; sc_idx < r_nEntries_2; ++sc_idx)
	{
		if(sc_idx%1000000 == 0) std::cout << "Scanning ZS off clusters: " << sc_idx << "/" << r_nEntries_2 << std::endl;
		offlineClusterTree_2->GetEntry(sc_idx);

		r_dict_2[ r_event_2 ][ r_detId_2 ][ sc_idx ] = cluster( sc_idx, r_event_2, r_run_2, r_lumi_2,
												r_detId_2, r_firstStrip_2, r_endStrip_2, r_barycenter_2,
												r_size_2, r_charge_2 );

		h_width_tot_off_all->Fill( r_size_2 );
		h_charge_tot_off_all->Fill( r_charge_2 );
		h_barycenter_tot_off_all->Fill( r_barycenter_2 );
	}

	canvall->cd(1);	
	canvall->GetPad(1)->SetMargin (0.18, 0.05, 0.15, 0.05);
	PlotStyle(h_width_tot_off_all); h_width_tot_off_all->SetLineColor(kBlue); 	h_width_tot_off_all->Draw("");
	PlotStyle(h_width_tot_on_all); h_width_tot_on_all->SetLineWidth(0); h_width_tot_on_all->SetFillColorAlpha(kBlack, 0.7); h_width_tot_on_all->SetLineColorAlpha(kBlack, 0.7);  	h_width_tot_on_all->Draw("same");
	TLegend* leg0 = canvall->GetPad(1)->BuildLegend(.4, .6, .85, .8);
	formatLegend(leg0);
	canvall->cd(2);	
	canvall->GetPad(2)->SetMargin (0.18, 0.05, 0.15, 0.05);
	PlotStyle(h_charge_tot_off_all); h_charge_tot_off_all->SetLineColor(kBlue); 	h_charge_tot_off_all->Draw("");
	PlotStyle(h_charge_tot_on_all); h_charge_tot_on_all->SetLineWidth(0); h_charge_tot_on_all->SetFillColorAlpha(kBlack, 0.7); h_charge_tot_on_all->SetLineColorAlpha(kBlack, 0.7); 	h_charge_tot_on_all->Draw("same");
	canvall->cd(3);	
	canvall->GetPad(3)->SetMargin (0.18, 0.05, 0.15, 0.05);
	PlotStyle(h_barycenter_tot_off_all); h_barycenter_tot_off_all->SetLineColor(kBlue); 	h_barycenter_tot_off_all->Draw("");
	PlotStyle(h_barycenter_tot_on_all); h_barycenter_tot_on_all->SetLineWidth(0); h_barycenter_tot_on_all->SetFillColorAlpha(kBlack, 0.7); h_barycenter_tot_on_all->SetLineColorAlpha(kBlack, 0.7); 	h_barycenter_tot_on_all->Draw("same");
	
	canvall->SaveAs("../img/cf_ZSon_ZSoff_TotalClusters_all.png");
	system("dropbox_uploader.sh upload ../img/cf_ZSon_ZSoff_TotalClusters_all.png /tmp/");

	delete canvall;


	TCanvas *canv0 = new TCanvas("canv0", "canv0", 600*3, 600*1);
	gStyle->SetOptTitle(0);
	gErrorIgnoreLevel = kWarning;
	canv0->Divide(3,1,0.001,0.001);

	TH1F * h_width_tot_on      = new TH1F( "ZS on", 
	                                    "; width; yield",  
	                                    50, 0., 50. );
	TH1F * h_charge_tot_on     = new TH1F( "charge_tot_on", 
	                                    "; charge; yield",  
	                                    100, 0., 700. );
	TH1F * h_barycenter_tot_on = new TH1F( "barycenter_tot_on", 
	                                    "; barycenter; yield",  
	                                    100, 0., 950. );

	TH1F * h_width_tot_off      = new TH1F( "ZS off", 
	                                    "; width; yield",  
	                                    50, 0., 50. );
	TH1F * h_charge_tot_off     = new TH1F( "charge_tot_off", 
	                                    "; charge; yield",  
	                                    100, 0., 700. );
	TH1F * h_barycenter_tot_off = new TH1F( "barycenter_tot_off", 
	                                    "; barycenter; yield",  
	                                    100, 0., 950. );


	for (auto& _scs_perEvt_perDetId: r_dict[chooseEventId]) 
	{
		TCanvas *canv2 = new TCanvas("canv2", "canv2", 600, 600);
		gStyle->SetOptStat(0);
		gStyle->SetOptTitle(0);

		bool first(true);
		for (auto& _sc: _scs_perEvt_perDetId.second) 
		{
			offlineClusterTree->GetEntry(_sc.first);

			cluster sc = _sc.second;
			// cluster( sc.idx, r_event, r_run, r_lumi,
			// 			r_detId, r_firstStrip, r_endStrip, r_barycenter,
			// 			r_size, r_charge );
			sc.print();

			TH1F * h_sc = new TH1F( Form("sc%d", sc.idx), Form("ZS on (%d-%d) rootIdx%d; strip; ADC",sc.firstStrip,sc.endStrip,_sc.first), 800,0,800 );

			for(uint16_t i=0; i < r_size; ++i) h_sc->Fill(r_channel[i], r_adc[i]);
			// PlotStyle(h_sc);  	
			h_sc->SetLineWidth(0);
			h_sc->SetFillColorAlpha(kBlack, 0.7);
			h_sc->SetLineColorAlpha(kBlack, 0.7);
			h_sc->GetYaxis()->SetRangeUser(0, h_sc->GetMaximum()*3);
			h_sc->DrawClone((first)? "hist": "hist same");

			first = false;

			h_width_tot_on->Fill( r_size );
			h_charge_tot_on->Fill( r_charge );
			h_barycenter_tot_on->Fill( r_barycenter );

			delete h_sc;
		}

		for (auto& _sc: r_dict_2[chooseEventId][_scs_perEvt_perDetId.first]) 
		{
			offlineClusterTree_2->GetEntry(_sc.first);

			cluster sc = _sc.second;
			// cluster( sc.idx, r_event_2, r_run_2, r_lumi_2,
			// 			r_detId_2, r_firstStrip_2, r_endStrip_2, r_barycenter_2,
			// 			r_size_2, r_charge_2 );
			sc.print();

			TH1F * h_sc = new TH1F( Form("sc_2%d", sc.idx), Form("ZS off (%d-%d) rootIdx%d; strip; ADC",sc.firstStrip,sc.endStrip,_sc.first), 800,0,800 );

			for(uint16_t i=0; i < r_size_2; ++i) h_sc->Fill(r_channel_2[i], r_adc_2[i]);
			// PlotStyle(h_sc);  	
			h_sc->SetLineWidth(0);
			h_sc->SetFillColorAlpha(kBlue, 0.7);
			h_sc->SetLineColorAlpha(kBlue, 0.7);
			h_sc->GetYaxis()->SetRangeUser(0, h_sc->GetMaximum()*3);
			h_sc->DrawClone("hist same");

			h_width_tot_off->Fill( r_size_2 );
			h_charge_tot_off->Fill( r_charge_2 );
			h_barycenter_tot_off->Fill( r_barycenter_2 );

			delete h_sc;
		}

		TLegend* leg = canv2->BuildLegend(.3, .6, .85, .8);
		// formatLegend(leg);
		gPad->Modified();
		gPad->Update();

		canv2->SaveAs(Form("../img/Event%d_DetId%d_clustersShape.png",
			chooseEventId, _scs_perEvt_perDetId.first));
		system(Form("dropbox_uploader.sh upload ../img/Event%d_DetId%d_clustersShape.png /tmp/",
			chooseEventId, _scs_perEvt_perDetId.first));

	}

	cout << "------------------------------- Finish scanned ZS on -------------------------------" << endl;

	for (auto& _scs_perEvt_perDetId: r_dict_2[chooseEventId]) 
	{
		if (r_dict[chooseEventId][_scs_perEvt_perDetId.first].size()!=0) continue;
		TCanvas *canv2 = new TCanvas("canv2", "canv2", 600, 600);
		gStyle->SetOptStat(0);
		gStyle->SetOptTitle(0);

		bool first(true);
		for (auto& _sc: _scs_perEvt_perDetId.second) 
		{
			offlineClusterTree_2->GetEntry(_sc.first);

			cluster sc = _sc.second;
			// cluster( sc.idx, r_event_2, r_run_2, r_lumi_2,
			// 			r_detId_2, r_firstStrip_2, r_endStrip_2, r_barycenter_2,
			// 			r_size_2, r_charge_2 );
			sc.print();

			TH1F * h_sc = new TH1F( Form("sc%d", sc.idx), Form("ZS off (%d-%d) rootIdx%d; strip; ADC",sc.firstStrip,sc.endStrip,_sc.first), 800,0,800 );

			for(uint16_t i=0; i < r_size_2; ++i) h_sc->Fill(r_channel_2[i], r_adc_2[i]);
			// PlotStyle(h_sc);  	
			h_sc->SetLineWidth(0);
			h_sc->SetFillColorAlpha(kBlue, 0.7);
			h_sc->SetLineColorAlpha(kBlue, 0.7);
			h_sc->GetYaxis()->SetRangeUser(0, h_sc->GetMaximum()*3);
			h_sc->DrawClone((first)? "hist": "hist same");

			first = false;

			h_width_tot_off->Fill( r_size_2 );
			h_charge_tot_off->Fill( r_charge_2 );
			h_barycenter_tot_off->Fill( r_barycenter_2 );

			delete h_sc;
		}

		TLegend* leg = canv2->BuildLegend(.3, .6, .85, .8);
		// formatLegend(leg);
		gPad->Modified();
		gPad->Update();

		canv2->SaveAs(Form("../img/Event%d_DetId%d_clustersShape.png",
			chooseEventId, _scs_perEvt_perDetId.first));
		system(Form("dropbox_uploader.sh upload ../img/Event%d_DetId%d_clustersShape.png /tmp/",
			chooseEventId, _scs_perEvt_perDetId.first));

	}

	cout << "------------------------------- Finish scanned ZS off -------------------------------" << endl;

	canv0->cd(1);	
	canv0->GetPad(1)->SetMargin (0.18, 0.05, 0.15, 0.05);
	PlotStyle(h_width_tot_off); h_width_tot_off->SetLineColor(kBlue); 	h_width_tot_off->Draw("");
	PlotStyle(h_width_tot_on); h_width_tot_on->SetLineWidth(0); h_width_tot_on->SetFillColorAlpha(kBlack, 0.7); h_width_tot_on->SetLineColorAlpha(kBlack, 0.7);  	h_width_tot_on->Draw("same");
	leg0 = canv0->GetPad(1)->BuildLegend(.4, .6, .85, .8);
	formatLegend(leg0);
	canv0->cd(2);	
	canv0->GetPad(2)->SetMargin (0.18, 0.05, 0.15, 0.05);
	PlotStyle(h_charge_tot_off); h_charge_tot_off->SetLineColor(kBlue); 	h_charge_tot_off->Draw("");
	PlotStyle(h_charge_tot_on); h_charge_tot_on->SetLineWidth(0); h_charge_tot_on->SetFillColorAlpha(kBlack, 0.7); h_charge_tot_on->SetLineColorAlpha(kBlack, 0.7); 	h_charge_tot_on->Draw("same");
	canv0->cd(3);	
	canv0->GetPad(3)->SetMargin (0.18, 0.05, 0.15, 0.05);
	PlotStyle(h_barycenter_tot_off); h_barycenter_tot_off->SetLineColor(kBlue); 	h_barycenter_tot_off->Draw("");
	PlotStyle(h_barycenter_tot_on); h_barycenter_tot_on->SetLineWidth(0); h_barycenter_tot_on->SetFillColorAlpha(kBlack, 0.7); h_barycenter_tot_on->SetLineColorAlpha(kBlack, 0.7); 	h_barycenter_tot_on->Draw("same");
	
	canv0->SaveAs(Form("../img/cf_ZSon_ZSoff_TotalClusters_Event%d.png", chooseEventId));
	system(Form("dropbox_uploader.sh upload ../img/cf_ZSon_ZSoff_TotalClusters_Event%d.png /tmp/", chooseEventId));

	delete canv0;

}


