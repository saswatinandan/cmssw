from ROOT import TLegend, TLatex

def formatLegend(leg, textsize=27):

    leg.SetBorderSize(0);
    leg.SetTextFont(43);
    leg.SetTextSize(textsize);
    leg.SetFillStyle(0);
    leg.SetFillColor(0);
    leg.SetLineColor(0);

def createLatex():

    latex = TLatex()
    latex.SetTextFont(63);
    latex.SetTextSize(31);
    latex.DrawLatexNDC(0.22,0.84,"CMS");
    latex.SetTextFont(53);
    latex.SetTextSize(22);
    latex.DrawLatexNDC(0.32,0.84,"Preliminary");
    latex.SetTextFont(43);
    latex.SetTextSize(24);
    return latex
