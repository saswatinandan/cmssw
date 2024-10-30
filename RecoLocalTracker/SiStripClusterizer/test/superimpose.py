import ROOT as r
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument("-i", dest="input_file", default='', help="name of input file")
parser.add_argument("-v", dest="vars", nargs='+', default=['width', 'charge', 'avg_charge'], help="list of variables")

options = parser.parse_args()

f = r.TFile(options.input_file)

for hist in options.vars:#['cluster_width', 'cluster_charge', 'cluster_avgcharge']:
    c = r.TCanvas('','',600,600)
    h1 = f.Get(f'raw_all_cluster_{hist}')
    h2 = f.Get(f'raw_unmatched_cluster_{hist}')
    h3 = f.Get(f'raw_low_pt_cluster_{hist}')
    h4 = f.Get(f'raw_high_pt_cluster_{hist}')
    h1.Scale(1/h1.Integral())
    h2.Scale(1/h2.Integral())
    h3.Scale(1/h3.Integral())
    h4.Scale(1/h4.Integral())
    h1.SetLineColor(2)
    h1.GetYaxis().SetRangeUser(0,1.2*max(h1.GetMaximum(), h2.GetMaximum(), h3.GetMaximum(), h4.GetMaximum()))
    h1.Draw('hist')
    
    c.Update()
    stat1 = h1.GetListOfFunctions().FindObject("stats")
    stat1.SetX1NDC(0.4);
    stat1.SetX2NDC(0.6);
    stat1.SetY1NDC(0.2);
    stat1.SetY2NDC(0.4);
    stat1.SetTextColor(2);
    c.Modified()

    c.cd()
    
    h2.Draw('hist sames')
    c.Update()
    stat2 = h2.GetListOfFunctions().FindObject("stats")
    stat2.SetX1NDC(0.4);
    stat2.SetX2NDC(0.6);
    stat2.SetY1NDC(0.4);
    stat2.SetY2NDC(0.6);
    
    h3.SetLineColor(3)
    h3.Draw(' sames hist')
    c.Update()
    stat3 = h3.GetListOfFunctions().FindObject("stats")
    stat3.SetX1NDC(0.4);
    stat3.SetX2NDC(0.6);
    stat3.SetY1NDC(0.6);
    stat3.SetY2NDC(0.8);
    stat3.SetTextColor(3);

    h4.SetLineColor(4)
    h4.Draw(' sames hist')
    c.Update()
    stat4 = h4.GetListOfFunctions().FindObject("stats")
    stat4.SetX1NDC(0.4);
    stat4.SetX2NDC(0.6);
    stat4.SetY1NDC(0.8);
    stat4.SetY2NDC(1.0);
    stat4.SetTextColor(4);
    
    leg = r.TLegend(.62, .6, .87, .8);
    leg.AddEntry(h1, 'matched_cluster', "f");
    leg.AddEntry(h2, 'unmatched_cluster', "f");
    leg.AddEntry(h3, 'low_pt', "f");
    leg.AddEntry(h4, 'high_pt', "f");
    leg.Draw('same')
    c.SaveAs(f'{hist}.png')
