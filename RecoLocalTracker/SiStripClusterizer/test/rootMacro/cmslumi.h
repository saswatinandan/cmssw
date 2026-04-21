#include "TPad.h"
#include "TLatex.h"
#include "TROOT.h"

void CMS_lumi(TPad* pad, const char* lumiText = "#it{pp} collision (13.6 TeV)") {
    // -----------------------------
    // Standard CMS text settings
    // -----------------------------
    const char* cmsText = "CMS";
    int cmsTextFont = 61;       // bold
    const char* extraText = "Preliminary";
    int extraTextFont = 52;     // italic

    double cmsTextSize_ndc   = 0.05;  // matches axis title size
    double extraTextSize_ndc = 0.04;  // matches axis label size
    double lumiTextSize_ndc  = 0.04;

    // -----------------------------
    // TLatex for drawing
    // -----------------------------
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);

    // Calculate top-left and top-right positions
    double xLeft  = pad->GetLeftMargin();
    double xRight = 1 - pad->GetRightMargin();
    double yTop   = 1 - pad->GetTopMargin() + 0.2 * pad->GetTopMargin();

    // -----------------------------
    // "CMS" - bold, top-left
    // -----------------------------
    latex.SetTextFont(cmsTextFont);
    latex.SetTextAlign(11); // left-top alignment
    latex.SetTextSize(cmsTextSize_ndc);
    latex.DrawLatex(xLeft, yTop, cmsText);

    // "Preliminary" - italic, right of "CMS"
    latex.SetTextFont(extraTextFont);
    latex.SetTextAlign(11); // left-top alignment
    latex.SetTextSize(extraTextSize_ndc);
    latex.DrawLatex(xLeft + 0.10, yTop, extraText);

    // -----------------------------
    // Lumi / energy text - top-right
    // -----------------------------
    latex.SetTextFont(42);
    latex.SetTextAlign(31);  // right-top alignment
    latex.SetTextSize(lumiTextSize_ndc);
    latex.DrawLatex(xRight, yTop, lumiText);

    pad->Update();
}
