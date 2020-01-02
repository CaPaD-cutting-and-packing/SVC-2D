#ifndef __SOLVER_H
#define __SOLVER_H

#include "mystat.h"

namespace bg {

class Solver {
  int argc; const char* const * argv;
  int inst, nInst;
  double sumObj;
  MyStat __stat, __stat_all, __stat_prm, __stat_prm_all;
  void Init(), ProcessArguments(), ProcessFile(const char *),
    PrintStat(const char * = ""), PrintStatAll();
  bool ProcessInstance(istream &, const char * fln);
  bool Process2DSPP(istream &, const char * fln);
  bool ProcessRCPSP(istream &, const char * fln);
public:
  Solver(int argc__, const char * const * argv);
  void Go();
  static bool GetNumArg(double &res, int &iarg,
    const char * const * argv, int argc);
  DeclClassIntParam instFirst, instLast, statPeriod, fWriteSolution, nPType;
   // fWriteGrpPrm; //fWriteMILP;
}; // class Solver

} // namespace bg

#endif // __SOLVER_H
