#ifndef __SVC_RCPSP__H
#define __SVC_RCPSP__H

#include "rcpsp_sub.h"
//#include "bottomleft.h"
#include "mystat.h"

// File: Heuristics for 2D SPP
namespace bg_rcpsp {

// Every proc s.b. independetn OR clearly define assertions

class SVC_RCPSP : public __2DSPP { // Based on BLOCK STRUCTURES
  int res, res_best; // to hide output info
public:
  SVC_RCPSP(const __2DSPP_input& inp,
    MyStat *prms=0, MyStat *prms_all=0)
    :__2DSPP(inp), sub(inp), _prm_stat(prms), _prm_stat_all(prms_all) { }
  Sub_RCPSP sub;
//  BL bl;
  MyStat * const _prm_stat, * const _prm_stat_all;
  static const char StatStr[];
  BlockStruc blocks; // after solving by sub or BL
  PL pl, plSub;
  double iter;
  double iter_best;
  int L_best, matBound;
    // best solution: to init the 2D basis?
// VARS:
  vec<double> d, // the pseudo-values, also INPUT to the knapsack-Sub
    d_sub, // INPUT to the knapsack-sub: corresp to the PL if it is random
    dr; // divided by li*wi (2D scheme) to build PL if random scheme 4
  double valreset_next;//, valreset_nextinc; // next iter & its increment to reset vals
  int nTickCnt;
  double tm_svc, tm_last;
  typedef List<pair<double, int> > TimeRow;
  TimeRow tmrow;
  bool fTimeLimit;
  Random rndDr, // for breaking ties in rel. pseudo-values
    rndDrMax, // the maximum of the factor
    rndParam, // for updating params
    qRnd; // for stoch. updating
  double DrMax; // the factor
  void
    Run(),
    WriteSolution(ostream&),
    SavePrmVals(),
    InitRun(),
    InitIteration(),
    SinglePass(),
    InstantiatePowers(bool fIterStart),
    CorrectValuesFor(Block&),  // VIRTUAL SVC
    NewPriorityList(),
    PerturbBaseOrdering(const PL& plb, PL& pl, vec<double>& d_kp);
  double ComputeLWRatio(Block& b, Block::iterator);
  int GetPLMethod();
//private:
  DeclClassDblParam MaxTime, MaxIter, //MaxPerturb_RelVal, //corr2D_fLocalWaste, p_fLocalW_new,
    //Decoder_Sub_BL,
    p_bwpnew;
//  DeclClassIntParam NIterSVC, NIterRandom; //, fWritePrm;
  DeclClassVectorParam ValReset, blockw_pow, BaseOrdering, p_choose;

  // instantated (rnadomly generated) parameters for current iteration:
  int //fLocalWaste,
    fSub_BL, plMethod;
  double blockw_pow__, p_choose__, sum_p_plm;
  vec<PL> plBase;
}; // class SVC_RCPSP

} // namespace bg_rcpsp

#endif // __SVC_RCPSP__H

