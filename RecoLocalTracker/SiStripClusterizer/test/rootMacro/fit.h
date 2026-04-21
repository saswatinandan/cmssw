#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"
#include "TPaveText.h"
#include "cmslumi.h"

// J/psi + exponential background function
double jpsiExp(double *x, double *p)
{
   double t = (x[0] - p[1]) / p[2];
    double alpha = std::abs(p[3]);
    double n_cb  = std::abs(p[4]);

    double signal;

    if (t > -alpha) {
        signal = p[0] * std::exp(-0.5 * t * t);
    } else {
        double A_cb = std::pow(n_cb / alpha, n_cb) * std::exp(-0.5 * alpha * alpha);
        double B_cb = n_cb / alpha - alpha;
        double val  = B_cb - t;

        signal = p[0] * A_cb * std::pow((val > 0 ? val : 1e-30), -n_cb);
    }

    return signal + p[5] + p[6] * x[0];
}

// Function to fit two histograms, plot them, and draw ratio
void FitTwoHistsCB(TH1F* hist1, TH1F* hist2,
	       	const std::string& t1, const std::string& t2,
		const std::string name
		)
{
    // -------- Fit functions --------
    TF1 *fit1 = new TF1("fit1", jpsiExp, 2.6, 3.5, 7);
    TF1 *fit2 = new TF1("fit2", jpsiExp, 2.6, 3.5, 7);

    hist1->SetStats(0);
    hist2->SetStats(0);
    fit1->SetParameters(hist1->GetMaximum(), 3.096, 0.03, 1.5, 3, hist1->GetMinimum(), 1.0);
    fit2->SetParameters(hist2->GetMaximum(), 3.096, 0.03, 1.5, 3, hist1->GetMinimum(), 1.0);
    //fit1->SetParameters(hist1->GetMaximum(), 91.18, 2.49, 1.5, 3, hist1->GetMinimum(), 1.0);
    //fit2->SetParameters(hist2->GetMaximum(), 91.18, 2.49, 1.5, 3, hist1->GetMinimum(), 1.0);
    for (TF1* fit : {fit1, fit2}) {
        fit->SetParNames("N_{sig}", "Mean", "Sigma", "alpha", "beta", "N_{bkg}", "slope");
    }

    // -------- Fit histograms --------
    hist1->Fit(fit1, "R0");
    hist2->Fit(fit2, "R0");

    // -------- Extract signal yields --------
    double nsig1 = fit1->GetParameter(0);
    double err1  = fit1->GetParError(0);
    double nsig2 = fit2->GetParameter(0);
    double err2  = fit2->GetParError(0);
    double nbkg1 = fit1->GetParameter(3);
    double bkg_err1  = fit1->GetParError(3);
    double nbkg2 = fit2->GetParameter(3);
    double bkg_err2  = fit2->GetParError(3);

    // -------- Canvas and pads --------
    TCanvas *c = new TCanvas("c", "Two histograms with ratio", 800, 800);

    TPad *pad1 = new TPad("pad1", "Top Pad", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();

    hist1->SetMarkerStyle(20);
    hist1->GetXaxis()->SetLabelOffset(0.02);
    hist1->GetYaxis()->SetLabelOffset(0.01);
    hist1->SetMarkerColor(kBlack);
    hist1->SetLineColor(kBlack);
    hist1->SetTitle("Hist1 vs Hist2 with J/psi Fit; m_{#mu#mu} [GeV]; Events");
    hist1->Draw("E");

    CMS_lumi(pad1);
    pad1->Update();
    c->Update();

    hist2->SetMarkerStyle(21);
    hist2->SetMarkerColor(kBlue);
    hist2->SetLineColor(kBlue);
    hist2->Draw("E SAME");
    
    fit1->SetLineColor(kBlack);
    fit1->Draw("SAME");
    fit2->SetLineColor(kBlue);
    fit2->Draw("SAME");

    // Legend
    TLegend *leg = new TLegend(0.65,0.65,0.85,0.75);
    leg->AddEntry(hist1, t1.c_str(), "lep");
    leg->AddEntry(hist2, t2.c_str(), "lep");
    
    /*TPaveText pave1(0.12, 0.55, 0.50, 0.82, "NDC");
    pave1.SetTextAlign(0.3);
    pave1.SetFillStyle(0);
    pave1.SetBorderSize(0);
    pave1.SetTextFont(42);
    pave1.SetTextSize(0.048);
    pave1.SetTextAlign(12);
    pave1.AddText(Form("%s: ", t1.c_str()));
    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(Form(" Mean = %.0f #pm %.0f MeV", fit1->GetParameter(1)*1000, fit1->GetParError(1)*1000));
    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(Form("  #sigma = %.0f #pm %.0f MeV\n", fit1->GetParameter(2)*1000, fit1->GetParError(2)*1000));
    pave1.AddText("");
    pave1.AddText("");
    pave1.AddText(Form(" N_{sig} = %.0f #pm %.0f\n", nsig1, err1));

    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(Form("%s: ", t2.c_str()));
    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(Form(" Mean = %.0f #pm %.0f MeV\n", fit2->GetParameter(1)*1000, fit2->GetParError(1)*1000));
    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(Form("  #sigma = %.0f #pm %.0f MeV\n", fit2->GetParameter(2)*1000, fit2->GetParError(2)*1000));
    pave1.AddText(" ");
    pave1.AddText(" ");
    pave1.AddText(Form(" N_{sig} = %.0f #pm %.0f\n", nsig2, err2));

    pave1.Draw("same");*/
    leg->Draw("same");


    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextSize(0.048);

    float y = 0.78;
    latex.DrawLatex(0.15, y, Form("%s: ", t1.c_str()));
    y -= 0.06;
    latex.DrawLatex(0.15, y, Form("Mean = %.0f #pm %.0f MeV", fit1->GetParameter(1)*1000, fit1->GetParError(1)*1000));
    y -= 0.06;
    latex.DrawLatex(0.15, y, Form("#sigma = %.0f #pm %.0f MeV", fit1->GetParameter(2)*1000, fit1->GetParError(2)*1000));
    y -= 0.06;
    latex.DrawLatex(0.15, y, Form("N_{sig} = %.0f #pm %.0f", nsig1, err1));

    y -=0.1;
    latex.DrawLatex(0.15, y, Form("%s: ", t2.c_str()));
    y -= 0.06;
    latex.DrawLatex(0.15, y, Form("Mean = %.0f #pm %.0f MeV", fit2->GetParameter(1)*1000, fit2->GetParError(1)*1000));
    y -= 0.06;
    latex.DrawLatex(0.15, y, Form("#sigma = %.0f #pm %.0f MeV", fit2->GetParameter(2)*1000, fit2->GetParError(2)*1000));
    y -= 0.06;
    latex.DrawLatex(0.15, y, Form("N_{sig} = %.0f #pm %.0f", nsig2, err2));
    
    // -------- Bottom pad: ratio --------
    c->cd();
    TPad *pad2 = new TPad("pad2", "Bottom Pad", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.3);
    pad2->Draw();
    pad2->cd();

    TH1D *hRatio = (TH1D*)hist1->Clone("hRatio");
    hRatio->SetMinimum(0.5);
    hRatio->SetMaximum(1.5);
    hRatio->Divide(hist2);
    hRatio->GetYaxis()->SetNdivisions(507);
    hRatio->GetYaxis()->SetLabelOffset(0.01);
    hRatio->SetMarkerStyle(20);
    hRatio->SetMarkerColor(kBlack);
    hRatio->SetLineColor(kBlack);
    hRatio->SetTitle("");
    hRatio->GetYaxis()->SetTitle("Ratio");
    hRatio->GetYaxis()->SetTitleSize(0.12);
    hRatio->GetYaxis()->SetTitleOffset(0.4);
    hRatio->GetYaxis()->SetLabelSize(0.1);
    hRatio->GetXaxis()->SetTitle("m_{#mu#mu} [GeV]");
    hRatio->GetXaxis()->SetTitleSize(0.12);
    hRatio->GetXaxis()->SetLabelSize(0.1);
    TLine* line = new TLine(hist1->GetXaxis()->GetXmin(), 1, hist1->GetXaxis()->GetXmax(), 1);
    hRatio->Draw("E");
    line->SetLineColor(kBlack);
    line->SetLineWidth(2);
    line->Draw("same");

    // -------- Save --------
    c->SaveAs(Form("jpsi_%s.png", name.c_str()));
    c->SaveAs(Form("jpsi_%s.pdf", name.c_str()));
}
