#ifndef __SUB_2DSPP__H
#define __SUB_2DSPP__H

#include "2dspp_heur.h"
//namespace combo {
extern "C" {
  //long minknap(int n, int *p, int *w, int *x, int c);
  #include "combo.h"
}
//}

// File: Heuristics for 2D SPP
namespace bg {

// Every proc s.b. independetn OR clearly define assertions

class Sub_2DSPP : public __2DSPP {
public:
  Sub_2DSPP(const __2DSPP_input& inp)
    :__2DSPP(inp), iter(0),
    suml(accumulate(inp.l.begin(), inp.l.end(), 0))
    //pl (GetBasePL())
    { }
  virtual void SinglePass(const PL&, const PL&, const vec<double>&);
  virtual BlockStruc & GetBlockStruc() { return blocks; } // not const

  DeclClassIntParam nScaleW;
  DeclClassDblParam //Filling_FFD_KP, p_q2Dnew,
    pChangeP_TIP; //pChangePBHole; // pOrientArea;
  DeclClassVectorParam vsp, pTower, pBubbleHole, pPermutateHole, pInvertHole;
  Random rndParam;
  int //fKP,
    fPermutate; // fOrientArea;
  double the_vscale, the_pTower, pTowerUpper, the_pBubbleHole, the_pPermHole, the_pInvertHole;
private:
//  virtual void InitBeforeManyRuns() { }
  int iter;
  BlockStruc blocks;
  const int suml;
  int suml_; // sum of l's - finish control,
//  PL pl; // the source pl sorted by non-incr. w's
  PL pl_; // working priority list
  const PL * pplKP; // ptr to a priority list for KP
  const vec<double> * pd; // pseudo-values (leading to pl?)
  vec<int> l_; // remaining lengths
  vec<double> dw;
  vec<item> items;
  vec<bool> fX;
  void InitSinglePass(const PL&, const PL&, const vec<double>& );
  void
    FillHole(Block&, Block::iterator &ie),  // ie will point to the last el. ???
    SelectFilling_KP(int, PL&),
    SelectFilling_Greedy(int, PL&),
    PutFilling(Block&, Block::iterator &ie, PL &),
//    SolveKP_cplex(int W, list<int> & sol, vec<double> &w),
    SolveKP_combo(int W, list<int> & sol, vec<double> &w),
    IntegrateNewHole(Block&, Block::iterator), // the same but ie const
    DoneSinglePass();
  PL GetBasePL();
}; // class Sub2D

} // namespace bg

#endif // __HEUR_2DSPP__H

