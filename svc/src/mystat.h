#ifndef __MYSTAT_H
#define __MYSTAT_H

//BEGIN_COMMON_NAMESPACE__

namespace bg {

template <class D>
class VectorStat { // each time fixes the next accumulated
  // value and tracks the differences. their ave. and dev.
  unsigned n;
  D div;
  unsigned addingAt;
  vec<D> prev;
  vec<D> accu, max_;
  vec<D> accu_sqr, accu_log, accu_log_sqr;
  vec<const char*> fmt;
  char sbuf[1024];
public:
  VectorStat() : n(0), div(1), addingAt(0) { }
  int getN() const { return n; }
  void FirstTimeSetDivisorForEach(const D d) {
    if (!n) {div=d; assert(d);}
  }
  void Reset() {
    n= addingAt = 0;
    prev.clear(); accu.clear(); max_.clear(); accu_sqr.clear(); accu_log.clear(); accu_log_sqr.clear();
  }
  VectorStat<D>& NextRun() { ++n; addingAt=0; return *this; }
  VectorStat<D>& operator<<(D d); // d is the accumulated value!!!
  VectorStat<D>& operator<<(const char*);
  void Print(ostream&os, unsigned i,const D d,const char dv) {
    if (fmt.size() <=i)
      fmt.resize(IMax(addingAt, i), fmt.size() ? fmt.back() : "%g");
    sprintf(sbuf, (fmt[i] ? fmt[i] : "%g"), d);
//    sprintf(sbuf + strlen(sbuf), "%c", dv);
    os << sbuf << dv;
  }
  void print_ave(ostream&,const char);
  void print_ave_log(ostream&,const char);
  void print_stddev(ostream&,const char);
  void print_stddev_log(ostream&,const char);
  void print_diff(ostream&,const char);
  void copy_diff_into(VectorStat<D> &);
  void print_max(ostream&,const char);
};

typedef VectorStat<double> MyStat;

template <class D>  //inline // !!!
 VectorStat<D>&
  VectorStat<D>::operator<<(D d) {
    d /= div;
    if (addingAt==accu.size()) {
// No more:      if (n>1) { assert(0); }
//      else { // could use the other case only too
        // assert(
 //       cout << " Ad1: "<<d<<" at:"<<addingAt;
        ++ addingAt;
        prev.push_back(0); accu.push_back(d); accu_sqr.push_back(d*d);
        accu_log.push_back(log(d));
        accu_log_sqr.push_back(log(FMax(0.01, d*d)));
        max_.push_back(d);

        if (fmt.size() < accu.size())
          fmt.resize(accu.size(), fmt.size() ? fmt.back() : "%g");
  //    }
    }
    else {
//      cout << " Ad2: "<<d<<" at:"<<addingAt;
      assert(addingAt < accu.size());
      prev[addingAt] = accu[addingAt];
      accu[addingAt] += d;
      accu_sqr[addingAt] += d*d;
      accu_log[addingAt] += log(FMax(0, d)+1);
      accu_log_sqr[addingAt] += pow(log(FMax(0, d)+1),2);
      if (max_[addingAt] < d)
        max_[addingAt] = d;
      ++ addingAt;
    }
    return *this;
  }

template <class D>  //inline // !!!
 VectorStat<D>&
  VectorStat<D>::operator<<(const char * fms) {
  if (addingAt >= fmt.size())
    fmt.resize(addingAt+1, fmt.size() ? fmt.back() : fms);
// Wrong:  assert(addingAt < accu.size());
  fmt[addingAt] = fms;
  return *this;
}
//// LOGARITMIC VALUES GIVEN

template <class D>  //inline // !!!
  void VectorStat<D>::print_ave(ostream& os, const char dv) {
  assert(n);
  for (unsigned i=0;i<accu.size();++i)
    Print(os,i,accu[i]/n,dv);
}
template <class D>  //inline // !!!
  void VectorStat<D>::print_stddev(ostream& os, const char dv) {
//  assert(n>=2);
  for (unsigned i=0;i<accu.size();++i)
    Print(os,i,n>1?sqrt((accu_sqr[i]-pow(accu[i],2)/n) / (n-1)):0,dv);
}
template <class D>  //inline // !!!
  void VectorStat<D>::print_diff(ostream& os, const char dv) {
  for (unsigned i=0;i<accu.size();++i)
    Print(os,i,(accu[i] - prev[i]),dv);
}
template <class D>  //inline // !!!
  void VectorStat<D>::copy_diff_into(VectorStat<D> &vs) {
  vs.NextRun();
  assert(fmt.size() >= accu.size());
  for (unsigned i=0;i<accu.size();++i)
    vs << fmt[i] << (accu[i] - prev[i]);
}
template <class D>  //inline // !!!
  void VectorStat<D>::print_ave_log(ostream& os, const char dv) {
  assert(n);
  for (unsigned i=0;i<accu.size();++i)
    Print(os,i,exp(accu_log[i]/n)-1,dv);
}
template <class D>  //inline // !!!
  void VectorStat<D>::print_stddev_log(ostream& os, const char dv) {
  assert(n>=2);
  for (unsigned i=0;i<accu.size();++i)
    Print(os,i,exp(sqrt((accu_log_sqr[i]-pow(accu_log[i],2)/n) / (n-1)))-1,dv);
}
template <class D>  //inline // !!!
  void VectorStat<D>::print_max(ostream& os, const char dv) {
  for (unsigned i=0;i<accu.size();++i)
    Print(os,i,max_[i],dv);
}


} // namespace bg

//END_COMMON_NAMESPACE__

#endif // MYSTAT_H
