#ifndef __1DLP_H
#define __1DLP_H

//#include <ilcplex/ilocplex.h>

namespace bg {

class __1DLP { // Binary patterns!!!!!
public:
  const int L;
  const vec<int> l,b;
  double lpv, tm_all, tm_cg;
  __1DLP(const int _L,const vec<int> &_l,const vec<int> &_b)
    : L(_L), l(_l), b(_b) { }
  void SolveLP();
  double LPVal() const { return lpv; }
  int LPBnd() const { return (int)ceil (lpv-1e-6); }
}; // clöass __1DLP

double SolveBKP_cplex(double W, vec<int> &w, vec<double>&p, vec<bool> &x);
/*
class BKP_cplex {
  auto_ptr<VirtDestr> model;
  void ConstructModel();
public:
  void SetBasicInfo(double W,vec<int> &w);
  void SetPrices(vec<double> &p);
  void Solve(vec<int> &x, double &val);
}*/



} // namespace bg

#endif // __1DLP_H

