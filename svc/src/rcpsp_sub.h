#ifndef __SUB_RCPSP__H
#define __SUB_RCPSP__H

#include "rcpsp_heur.h"
//namespace combo {
extern "C" {
  //long minknap(int n, int *p, int *w, int *x, int c);
  #include "combo.h"
}
//}

// File: Heuristics for 2D SPP
namespace bg_rcpsp {

using namespace bg;

// Every proc s.b. independetn OR clearly define assertions

class Sub_RCPSP : public __2DSPP {
public:
  Sub_RCPSP(const __2DSPP_input& inp)
    :__2DSPP(inp), iter(0),
    suml(accumulate(inp.l.begin(), inp.l.end(), 0))
    //pl (GetBasePL())
    { }
  virtual void SinglePass(const PL&, const PL&, const vec<double>&);
  virtual BlockStruc & GetBlockStruc() { return blocks; } // not const

  DeclClassIntParam nScaleW;
  DeclClassVectorParam vsp;
  Random rndParam;
  double the_vscale;
private:
//  virtual void InitBeforeManyRuns() { }
  int iter;
  BlockStruc blocks;
  const int suml;
  int suml_; // sum of l's - finish control,
//  PL pl; // the source pl sorted by non-incr. w's
  PL pl_; // working priority list, contains unpacked items
  const PL * pplKP; // ptr to a priority list for KP
  const vec<double> * pd; // pseudo-values (leading to pl?)
  vec<int> l_; // remaining lengths
  vec<double> dw;
  vec<item> items;
  vec<bool> fX;
  void InitSinglePass(const PL&, const PL&, const vec<double>& );
  void
    FillHole(Block& ),  // ie will point to the last el. ???
    SelectFilling_KP(int, PL&),
    SelectFilling_Greedy(int, PL&),
    PutFilling(Block &, PL &),
//    SolveKP_cplex(int W, list<int> & sol, vec<double> &w),
    SolveKP_combo(int W, list<int> & sol, vec<double> &w),
    IntegrateNewHole(Block&, Block::iterator), // the same but ie const
    DoneSinglePass();
  PL GetBasePL();
}; // class Sub2D

} // namespace bg_rcpsp

#endif // __HEUR_RCPSP__H

