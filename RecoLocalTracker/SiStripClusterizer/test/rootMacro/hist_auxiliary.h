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
                 double evtWeight,
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

#endif //HIST_AUXILIAY_H
