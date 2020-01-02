#ifndef __2DSPP__H
#define __2DSPP__H

//#include "mystat.h"

// File: Heuristics for 2D SPP
namespace bg {

// Every proc s.b. independetn OR clearly define assertions

class Problem {
public:
//  ofstream __log, __log_stat;
//  VectorStat<double> * __pstat;
//  Problem() : __pstat(0) { }
  virtual ~Problem() { }
}; // class Problem

class __2DSPP_input : public Problem {
public:
  __2DSPP_input(int W_, const vec<int> &w_, const vec<int> &l_,
      const char* prn=0,int lb_=0, int ub_=INT_MAX)
    : W(W_), w(w_), l(l_),
    minw(w.empty()? 0: *min_element(w.begin(), w.end())),
    pr_name(prn), lb(lb_), ub(ub_)
    { assert(w.size() == l.size()); }
  const int W;  // INPUT
  const vec<int> w, l;
  const int minw;
  const char * pr_name;
  mutable int lb, ub; // bounds
}; //

class __2DSPP_output { // cuase it is written
  __2DSPP_output() { }
public:
  int L;
  vec<int> x, y;
  __2DSPP_output(const __2DSPP_input& inp) { Init(inp); L = INT_MAX; }
  void Init(const __2DSPP_input& inp) {
    L = 0;
    x.clear(); x.resize(inp.l.size(), -1);
    y.clear(); y.resize(inp.l.size(), -1);
  }
  void SelfCheck(const __2DSPP_input&, int =0) const;
};

class __2DSPP : public __2DSPP_input {
public:
  __2DSPP(const __2DSPP_input & id)
    :__2DSPP_input(id), res(id), res_best(id) { }
  __2DSPP_output res, res_best;
  int MaterialBound();
  void Write(ostream& ,int ub,const char*fln,int inst);
  void WriteSolution(ostream&);
}; // class __2DSPP

__2DSPP_input Read_2DSPP(istream&);

//void Check2DPacking(int W, int L, vec<int> &w, vec<int> &l, vec<int> &x, vec<int> &y);
void Create2DSPPModel(__2DSPP &, const char * fln=0, int inst=0, int ub0 = INT_MAX);

class __2DSPP_1DLP {
  __2DSPP pr;
  int __ub;
public:
  __2DSPP_1DLP(__2DSPP &prob, int ub) : pr(prob), __ub(ub) { }
  void Solve();
/// OUTPUT:
  int lbv, lbh;
  double cg, tmh, tmv;
};

} // namespace bg

#endif // __2DSPP__H

