#pragma once
#include "histManagerBase.h"
#include "TrackAlgo.h"
#include "object.h"
#include "fit.h"

class match_trackobj_histManager
      : public histManagerBase
{
        public:
          match_trackobj_histManager(const string& obj, const std::string& type1, const std::string& type2):
          histManagerBase(obj),
	  t1(type1),
	  t2(type2)
          {

             hists_2d["pt"] = createhist("pt", Form("pt %s vs %s", t1.c_str(), t2.c_str()), 100, 0, 20, 100, 0, 20);
	     hists_2d["mass"] = createhist("mass", Form("mass %s vs %s", t1.c_str(), t2.c_str()), 50, 2.6, 3.5, 50, 2.6, 3.5);
	     hists_2d["hits"] = createhist("validhits", Form("validhits %s vs %s", t1.c_str(), t2.c_str()), 50, 0.5, 50.5, 50, 0.5, 50.5);
	     hists[Form("matched_mass_%s", t1.c_str())] = createhist(Form("matched_mass_%s", t1.c_str()), t1, 50, 2.6, 3.5);
             hists[Form("matched_mass_%s", t2.c_str())] = createhist(Form("matched_mass_%s", t2.c_str()), t2, 50, 2.6, 3.5);
             hists[Form("unmatched_mass_%s", t1.c_str())] = createhist(Form("unmatched_mass_%s", t1.c_str()), t1, 50, 2.6, 3.5);
	     hists[Form("unmatched_mass_%s", t2.c_str())] = createhist(Form("unmatched_mass_%s", t2.c_str()), t2, 50, 2.6, 3.5);
           };

     void compareMatching()
     {
      TCanvas* canv = create_canvas();
      canv->SetMargin(0.18, 0.20, 0.12, 0.07);
      //canv->SetLogy(true);
      gStyle->SetOptStat(1111);
      for (const auto [k,hist] : hists_2d) {
        hist->SetStats(1);
        hist->Scale(1./hist->Integral());
        hist->Draw("colz");
        hist->GetXaxis()->SetTitle(t1.c_str());
        hist->GetYaxis()->SetTitle(t2.c_str());
        TLine* line = new TLine(hist->GetXaxis()->GetXmin(), hist->GetYaxis()->GetXmin(), hist->GetXaxis()->GetXmax(), hist->GetYaxis()->GetXmax());
        line->SetLineColor(kRed);
        line->SetLineWidth(2);
        line->Draw("SAME");
        gPad->Update();
        TPaveStats* st = (TPaveStats*)hist->FindObject("stats");
        if (st) {
          st->SetX1NDC(0.16);
          st->SetX2NDC(0.39);
          st->SetY1NDC(0.75);
          st->SetY2NDC(0.90);
        }
        gPad->Modified();
        gPad->Update();
        canv->SaveAs(Form("%s.png", k.c_str()));
        canv->Clear();
      }

      for (const auto [k,hist] : hists) {
        hist->SetStats(1);
        //hist->Scale(1./hist->Integral());
        hist->GetXaxis()->SetTitle(k.c_str());
        hist->Draw();
        canv->SaveAs(Form("%s.png", k.c_str()));
        canv->Clear();
      }
      std::cout << hists[Form("matched_mass_%s", t2.c_str())]->Integral()+hists[Form("unmatched_mass_%s", t2.c_str())]->Integral() << std::endl;
      FitTwoHistsCB(hists[Form("matched_mass_%s", t1.c_str())], hists[Form("matched_mass_%s", t2.c_str())], t1, t2, "matched");
      FitTwoHistsCB(hists[Form("unmatched_mass_%s", t1.c_str())], hists[Form("unmatched_mass_%s", t2.c_str())], t1, t2, "unmatched");
     };
     std::string t1;
     std::string t2;
};

