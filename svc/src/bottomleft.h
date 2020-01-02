#ifndef __BL__H
#define __BL__H

#include "2dspp_heur.h"

// File: Heuristics for 2D SPP
namespace bg {

// Every proc s.b. independetn OR clearly define assertions

class Hole {
public:
  int x,w;
  BlockStruc::iterator ib; // origin: contains also the base y
  Hole(int x_,int w_,BlockStruc::iterator i_)
    : x(x_), w(w_), ib(i_) { }
}; // class Hole

typedef List<Hole> HoleList;

class BL : public __2DSPP {
public:
  BL(const __2DSPP_input & inp)
    : __2DSPP(inp) { }
  virtual void SinglePass(PL & );
  virtual BlockStruc & GetBlockStruc() { return blocks; }
  DeclClassDblParam fImproved;
  DeclClassVectorParam pInvertShove;
  double the_pInvertShove; // for each solution
private:
  BlockStruc blocks;
  HoleList lh0, lh1;
  Random rndParam;
  void InitSinglePass();
  void PutItem(PL::iterator ii),
    InsertItem(BlockStruc::iterator ib, int x, int i),
    DoneSinglePass(),
    ComputeAreaLR(BlockStruc::iterator ib, int x, int w_, //w_ is the _hole_ width!
  double& aL, double& aR);
  bool NewHole_or_PutItem(int x, int w_,
    BlockStruc::iterator ib_origin, // contains also the start height
    BlockStruc::iterator ib_now, PL::iterator ii);
}; // class SVC_BL

} // namespace bg

#endif // __BL__H

