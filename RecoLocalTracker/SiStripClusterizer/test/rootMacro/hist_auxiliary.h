#ifndef HIST_AUXILIARY_H
#define HIST_AUXILIARY_H

#include "TH1F.h"

template <typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value>>
T
constrainValue(T value,
                 T lowerBound,              
                 T upperBound)
  {
    assert(lowerBound <= upperBound);
    value = std::max(value, lowerBound);    
    value = std::min(value, upperBound);
    return value;
}

int fillWithOverFlow(TH1 * histogram,
                 double x,
                 double evtWeight=1,
                 double evtWeightErr=0.)
{
  if(!histogram) assert(0);
  const TAxis * const xAxis = histogram->GetXaxis();
  const int bin = constrainValue(xAxis->FindBin(x), 1, xAxis->GetNbins());
  const double binContent = histogram->GetBinContent(bin);
  const double binError   = histogram->GetBinError(bin);
  histogram->SetBinContent(bin, binContent + evtWeight);
//  histogram->SetBinError(bin, std::sqrt(pow(binError,2) + 1));
  return ((bin == xAxis->GetNbins()) || (bin == 1)) ? 1 : 0;
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

#endif //HIST_AUXILIAY_H
