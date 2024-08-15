from ROOT import TH1F, TH2F

def PlotStyle(h):

  defaultFont     = 43;
  x_title_size    = 28;
  y_title_size    = 28;

  x_title_offset  = 1.5;
  y_title_offset  = 2.1;

  label_size      = 28;
  label_offset    = 0.013;

  h.GetXaxis().SetLabelFont(defaultFont);
  h.GetXaxis().SetTitleFont(defaultFont);
  h.GetYaxis().SetLabelFont(defaultFont);
  h.GetYaxis().SetTitleFont(defaultFont);
  h.GetZaxis().SetLabelFont(defaultFont);
  h.GetZaxis().SetTitleFont(defaultFont)

  h.SetTitleOffset(0);


  h.GetYaxis().SetTitleOffset(y_title_offset);
  h.GetYaxis().CenterTitle();
  h.GetYaxis().SetTitleSize(x_title_size);
  h.GetYaxis().SetLabelOffset(label_offset);
  h.GetYaxis().SetLabelSize(label_size);
  h.GetYaxis().SetNdivisions(508);

  h.GetXaxis().SetTitleOffset  (1.2);
  h.GetXaxis().CenterTitle();
  h.GetXaxis().SetTitleSize(x_title_size);
  h.GetXaxis().SetLabelOffset(label_offset);
  h.GetXaxis().SetLabelSize(label_size);
  h.GetXaxis().SetNdivisions(508);

  h.GetZaxis().SetTitleOffset(1.2);
  h.GetZaxis().CenterTitle();
  h.GetZaxis().SetTitleSize(x_title_size);
  h.GetZaxis().SetLabelOffset(label_offset);
  h.GetZaxis().SetLabelSize(label_size);
  h.GetZaxis().SetNdivisions(508);


  h.SetLineWidth(2)

  return h


def constrainValue(value, lowerBound, upperBound):

     assert(lowerBound <= upperBound)
     value = max(value, lowerBound)
     value = min(value, upperBound)
     return value

def fillWithOverFlow(histogram,
                     x,
                     evtWeight=1,
                     evtWeightErr=0.):

  if not histogram: assert(0);
  xAxis = histogram.GetXaxis();
  bin = constrainValue(xAxis.FindBin(x), 1, xAxis.GetNbins());
  binContent = histogram.GetBinContent(bin);
  binError   = histogram.GetBinError(bin);
  histogram.SetBinContent(bin, binContent + evtWeight);
  if (bin == xAxis.GetNbins() or bin ==1):
      return 1
  else:
      return 0
'''
def Divide_w_sameDsets(num, denom, ratio):

    for _x in range(1, _x.num.GetNbinsX()+1):
    
     _r = denom.GetBinContent(_x) ? num->GetBinContent(_x)/denom->GetBinContent(_x) : 0.;
                float _n_relErr = TMath::Abs(   num->GetBinError(_x)/  num->GetBinContent(_x) );
                float _d_relErr = TMath::Abs( denom->GetBinError(_x)/denom->GetBinContent(_x) );
                float _r_err    = TMath::Abs(_r) * ( (_n_relErr > _d_relErr) ? _n_relErr : _d_relErr  );
                ratio->SetBinContent(_x, _r);
                ratio->SetBinError(_x, _r_err);
        }
}
'''
def createhist(name, title, nbins, xlow=9999, xhigh=0, nbins_y=0, ylow=0, yhigh=0, one_d=True):

    if one_d:
      if(xlow==9999):
        h = TH1F(name, title, nbins, array);
        h.Sumw2();
        return h;
      else:
        h = TH1F(name, title, nbins, xlow, xhigh);
        h.Sumw2();
        return h;
    else:
        h = TH2F(name, title, nbins, xlow, xhigh, nbins_y, ylow, yhigh);
        h.Sumw2();
        return h;
