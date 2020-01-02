#ifndef __BEASLEY__H
#define __BEASLEY__H

// File: Heuristics for 2D SPP
namespace bg {

// Every proc s.b. independetn OR clearly define assertions

class CplexModelSolver {
//  const char * modelfile;
public:
  double resOpt, resVal;
//  CplexModelSolver(const char * f) :modelfile(f) { }
  void SolveLP(const char *);
}; // class

class Beasley2DLP {
  const int W, hlb, hub;
  const veci w,h;
  int H; // var
  CplexModelSolver slv;
  void WriteModel(const char * modelfile);
public:
  Beasley2DLP(int W_, const veci & w_, const veci& h_, int hlb_, int hub_)
    : W(W_),  hlb(hlb_), hub(hub_), w(w_), h(h_) { }
  void Solve2DSPP();
  void SolveFixedContainer();
  int lbRes;
  double tm;
}; // class

} // namespace bg

#endif // __BEASLEY__H

