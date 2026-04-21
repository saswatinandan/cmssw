#include "TH1F.h"
#include "TH2F.h"

inline TH1F* create_hist(edm::Service<TFileService>& fs, const std::string& name, const std::vector<double>& ptbins) {
   const double* arry = ptbins.data();
   TH1F* h = fs->make<TH1F>(name.c_str(), name.c_str(), ptbins.size()-1, arry);
   return h;
}

inline TH1F* create_hist(edm::Service<TFileService>& fs, const std::string& name, const int& nbins, const double& xlow, const double& xhigh) {
       return fs->make<TH1F>(name.c_str(), name.c_str(), nbins, xlow, xhigh);
}

inline TH1F* create_hist(TFileDirectory& dir, const std::string name, const std::string title, const int nbins, const double xlow, const double xup) {
     return dir.make<TH1F>(name.c_str(), title.c_str(), nbins, xlow, xup);
}

inline TH2F* create_hist(TFileDirectory& dir, const std::string name, const std::string title, const std::vector<double>& ptbins, const int& nybins,const float ylow, const float& yhigh, const std::string xtitle, const std::string ytitle) {
        const double* arry = ptbins.data();
        TH2F* h = dir.make<TH2F>(name.c_str(), title.c_str(), ptbins.size()-1, arry, nybins, ylow, yhigh);
	h->GetXaxis()->SetTitle(xtitle.c_str());
	h->GetYaxis()->SetTitle(ytitle.c_str());
	return h;
}

template<typename std::size_t S>
inline TH2F* create_hist(TFileDirectory& dir, const std::string name, const std::string title, const std::vector<double>& ptbins, const double (&arry)[S], const std::string xtitle, const std::string ytitle) {
      TH2F* h = dir.make<TH2F>(name.c_str(), title.c_str(), ptbins.size()-1, ptbins.data(), sizeof(arry)/sizeof(arry[0])-1, arry);
      h->GetXaxis()->SetTitle(xtitle.c_str());
      h->GetYaxis()->SetTitle(ytitle.c_str());
      return h;
}

inline TH2F* create_hist(edm::Service<TFileService>& fs, const std::string name, const std::string title, const std::vector<double>& pbins, const std::vector<double>& ptbins, const std::string xtitle, const std::string ytitle) {
    TH2F* h = fs->make<TH2F>(name.c_str(), title.c_str(), pbins.size()-1, pbins.data(), ptbins.size()-1, ptbins.data());
    h->GetXaxis()->SetTitle(xtitle.c_str());
    h->GetYaxis()->SetTitle(ytitle.c_str());
    return h;
}

inline TH2F* create_hist(TFileDirectory& dir, const std::string name, const std::string title, const int nxbins, const float xlow, const float xhigh, const int& nybins,const float ylow, const float& yhigh, const std::string xtitle, const std::string ytitle) {
      TH2F*h =  dir.make<TH2F>(name.c_str(), title.c_str(), nxbins, xlow, xhigh, nybins, ylow, yhigh);
      h->GetXaxis()->SetTitle(xtitle.c_str());
      h->GetYaxis()->SetTitle(ytitle.c_str());
      return h;
}

template <typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value>>
T
inline constrainValue(T value,
               T lowerBound,
               T upperBound)
  {
    assert(lowerBound <= upperBound);
    value = std::max(value, lowerBound);
    value = std::min(value, upperBound);
    return value;
}

inline void fillWithOverFlow(TH2 * histogram,
                 double x,
                 double y,
                 double evtWeight=1.,
                 double evtWeightErr=0.)
{
  if(!histogram) assert(0);
  const TAxis * const xAxis = histogram->GetXaxis();
  const TAxis * const yAxis = histogram->GetYaxis();
  const int binx = constrainValue(xAxis->FindBin(x), 1, xAxis->GetNbins());
  const int biny = constrainValue(yAxis->FindBin(y), 1, yAxis->GetNbins());
  const double binContent = histogram->GetBinContent(binx, biny);
  const double binError   = histogram->GetBinError(binx, biny);
  histogram->SetBinContent(binx, biny, binContent + evtWeight);
  histogram->SetBinError(binx, biny, std::sqrt(pow(binError,2) + 1));
}

inline void fillWithOverFlow(TH1 * histogram,
      double x,
      double evtWeight=1.,
      double evtWeightErr=0.)
{
  if(!histogram) assert(0);
  const TAxis * const xAxis = histogram->GetXaxis();
  const int bin = constrainValue(xAxis->FindBin(x), 1, xAxis->GetNbins());
  const double binContent = histogram->GetBinContent(bin);
  const double binError   = histogram->GetBinError(bin);
  histogram->SetBinContent(bin, binContent + evtWeight);
  histogram->SetBinError(bin, std::sqrt(pow(binError,2) + 1));
}

