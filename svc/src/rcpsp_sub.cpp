#include "firsthdr.h"
#include "rcpsp_sub.h"
//#include "1dlp.h"

// INITIALIZE EVERYTHING IN APPROPRIATE PHASE _____
// -- this should have helped so often!
namespace bg_rcpsp {

using namespace bg;
//#define RND_SC
//double combomax;

//int f70 = 0;

//ClassDblParam(Sub_RCPSP, Filling_FFD_KP, 1,
  //"0: Filling holes by FFD, 1: by KP, between: probability");
ClassIntParam(Sub_RCPSP, nScaleW, 0,
  "vscale0 = min{wi/di | I0} : W/max{di | I_0} : W0/max{di | I_0} (nScaleW = 0,1,2)");
double q2D_def[] = {-0.8, 0.5};
ClassVectorParam(Sub_RCPSP, vsp, q2D_def,
  "multiply the 2D portion of KP values by the_vscale*di*vscale0, where the_vscale = 10^(RndUniform(vsp2-vsp1)) set in each run" //"q2d[1] - log10(rnd) * q2D[2]"
  );
//ClassDblParam(Sub_RCPSP, p_q2Dnew, 1,
  //"probability to change q2D in an iteration");

void Sub_RCPSP::SinglePass(const PL& pl, const PL& plKP, const vec<double>& d) {
  ++ iter;
  Block::iterator ie, ie1;
  InitSinglePass(pl,plKP,d);
  do {
    Block & b = blocks.back();
    FillHole(b);      // need some struct to find fitting items?
    b.len = INT_MAX;
    for_each_in(b, ie,)
      EnsureMin(b.len, l_[ie->i]); // block length
    res.L += b.len;            // the total length
    blocks.push_back(b);   // A COPY
    Block & b1 = blocks.back();
    b1.y = res.L;
    ie = b1.begin();
    do {
        ie1 = ie;
        ++ ie1; // because ie can be deleted
        l_[ie->i] -= b.len;
        suml_ -= b.len;       // stopping control
        if (l_[ie->i] <= 0) // a product is over
          IntegrateNewHole(b1, ie); // RC PSP: just delete *ie and add w
        ie = ie1;
    } while (b1.end() != ie);
    assert(suml_ >= 0);
  } while (suml_ > 0);
  DoneSinglePass();
}

void Sub_RCPSP::InitSinglePass(const PL& pl, const PL& plKP, const vec<double>& d) {
//  fKP = (rndParam < Filling_FFD_KP);
//  if (rndParam < p_q2Dnew or 1 == iter) { // !!!
//    double rnd1 = 1e-12 + rndParam*(1-1e-12);
//    the_q2D = q2D.val[0] - log10(rnd1) * q2D.val[1];
//    assertm(fabs(the_q2D) < 1e100, "the_q2D="<<the_q2D<<", rnd1="<<rnd1);
//  }
  the_vscale = pow(10, rndParam.InRange(vsp.val[0], vsp.val[1]));
//  the_vscale = vsp.val[0] - vsp.val[1] * log10(rndParam.InRange(1e-12,1));

#ifdef RND_SC
  if (rndParam < 0.1 or 1 == iter) // !!! 0.1 ??
    combomax = pow(10, rndParam.InRange(4,6));
#endif

#ifndef NDEBUG /// CHECK monotony. WHY ???
  PL::const_iterator ip = pl.begin(), ip1;
  for(;;) {
    ip1 = ip;
    ++ ip1;
    if (pl.end() == ip1)
      break;
    assert(w[*ip] <= w[*ip1]);
    ++ ip;
  }
#endif

//    the_q2D = q2D.val[0] + rndParam * (q2D.val[1]-q2D.val[0]);
  l_ = l;          // init remaining lengths;
  pl_ = pl;
  pplKP = &plKP;
  pd = &d;
  suml_ = suml;
  blocks.Reset(W);
  res.Init(*this); // set coords to -1
}


void Sub_RCPSP::FillHole(Block& b) {
//  assert( not pl_.empty()); can happen in the last block
  if (pl_.empty()) return; // necessary???
  bool fKP__ = true; //fKP;
  if (fKP__) {
    int sw=0, nfit=0;
    for_each_in(pl_, ip, list<int>::iterator)
      if (w[*ip] <= b.waste) // !!!!!! only fitting items
        { sw += w[*ip]; ++ nfit; }
      else break; // pl sorted acc. to growing w's
    if (!nfit)
      return;
    if (nfit < 2 or sw <= b.waste)
      fKP__ = false; // otherwise combo fails
  }
  PL filling;
  if (fKP__) {
//  try{
    SelectFilling_KP(b.waste,filling);
//  } catch (...) {
//    FillFFD(b,ie);
//  }
  } else
    SelectFilling_Greedy(b.waste,filling);
  PutFilling(b, filling);
//  ++ ie; NO, this is done in the for-cycle
}

void Sub_RCPSP::SelectFilling_Greedy(int W, PL& grd_l) {
  PL::iterator ip1 = pl_.begin(), ip;
  int W_ = W; // starting size
  for (; //W_ >= minw &&
      pl_.end() != ip1;) { // free space
    if (w[*ip1] > W_)
      break; // sorted pl!
    ip = ip1;
    ++ ip1; // before splice
    W_ -= w[*ip];
    assert(W_ >= 0);
    grd_l.splice(grd_l.end(), pl_, ip); // erasing item
  }
}

void Sub_RCPSP::SelectFilling_KP(int W_, PL& kp_l) {
//  List<int> kp_l;
  PL::iterator ip; // NO DELETION from pl_
  int W_all = W;
  int W=W_; // to hide
  dw = *pd; // pseudo-values
//  double maxval=0;
  double scale;
  if (!nScaleW) {
    double minwd = 1e100; // *max_element(d.begin(), d.end());
    for_each_in(pl_,ip,)
      if (w[*ip] <= W) // --- may not be
        EnsureMin(minwd, w[*ip]/dw[*ip]); // indep. from W_!
      else break; // pl sorted
    scale = the_vscale * minwd; // normalized also for witdh !
  } else {
    double maxdw = 0.0; // *max_element(d.begin(), d.end());
    for_each_in(pl_,ip,)
      if (w[*ip] <= W) // --- may not be
        EnsureMax(maxdw, dw[*ip]); // indep. from W_!
      else break; // pl sorted
    if (1==nScaleW)
      scale = the_vscale * W_all / maxdw; // normalized also for witdh !
    else
      scale = the_vscale * W_ / maxdw; // normalized also for witdh !
  }
//  assert(maxub > 0);
//  assert(maxub < 1e90);
  unsigned i;
//  for (i=0;i<dw.size();++i)
  //  dw[i] /= l[i]; // bring to 1 dimension -- not good
  for (i=0;i<dw.size();++i)
    dw[i] = double(w[i]) + dw[i] * scale; // bring to 1 dimension
//  if (1) // fKP_combo
  SolveKP_combo(W, kp_l, dw); // MINUS ie->second
}

void Sub_RCPSP::PutFilling(Block&b, PL& kp_l) {
  PL::iterator ip, ip1; // NO DELETION from pl_
  Block::iterator ie1;

  int sw=0;
  for_each_in(kp_l, ip, )
    sw += w[*ip];
  assert(sw <= b.waste);
  assert(sw > 0);

  for_each_in(kp_l, ip, ) {
    res.y[*ip] = res.L;
    b.push_back(BlockElement(*ip)); // item index
    b.waste -= w[*ip];
  }
}

/*
void Sub_RCPSP::SolveKP_cplex
  (int W_, list<int> & kp_l, vec<double> &d) {
  unsigned i;
  list<int>::iterator ip, ip1;
  vec<int> www(pl_.size());
  vec<double> ppp(pl_.size());
  vec<bool> x(pl_.size());
  i=0;
  for_each_in(pl_,ip,) {
    ppp[i] = d[*ip];
    www[i] = w[*ip];
    ++i;
  }
  SolveBKP_cplex(W_, www, ppp, x);
  i=0;
  ip1 = pl_.begin();
  for (; ip1 != pl_.end(); ++i) {
    ip = ip1++ ;
    if (x[i])
      kp_l.splice(kp_l.end(), pl_, ip);
  }
  int sw=0;
  for_each_in(kp_l, ip, )
    sw += w[*ip];
  assert(sw <= W_);
}
*/
/// taking pl_ as current items and d[] as their weights
/// pl_ is modified.
/*void Sub_RCPSP::SolveKP_minknap(int W_, list<int> & kp_l) {
  // Computing parameters to convert the profits to int:
  unsigned i;
  list<int>::iterator ip, ip1;
  double maxval = 0;
  for_each_in(pl_,ip,)
    if (d[*ip] * W_ / w[*ip] > maxval)
      maxval = d[*ip] * W_ / w[*ip]; // how big at most
  vec<int> www(pl_.size()), ppp(pl_.size()), x(pl_.size());
  typedef multimap<pair<int,int>, list<int>::iterator> ItMMap;
  ItMMap orit;
  i=0;
  for_each_in(pl_,ip,) {
    ppp[i] = int( d[*ip] * 1e6 / maxval );
    www[i] = w[*ip];
    orit.insert(make_pair(
      make_pair(ppp[i], www[i]), ip));
    ++i;
  }
  minknap(ppp.size(), &ppp[0], &www[0], &x[0], W_);
  for(i=0;i<x.size(); ++i)
    if (x[i]) {
      ItMMap::iterator ii = orit.find(
        make_pair(ppp[i], www[i]));
      kp_l.splice(kp_l.end(), pl_, ii->second);
      orit.erase(ii);
    }
//  assert(i == items.size());
  int sw=0;
  for_each_in(kp_l, ip, )
    sw += w[*ip];
  assert(sw <= W_);
}
*/

//item glb_items[200];
//stype glb_c;
//double glb_maxval;
//double glb_d[200];
//itype glb_pmax;

/// taking pl_ as current items and d[] as their weights
/// pl_ is modified.
void Sub_RCPSP::SolveKP_combo(int W_, list<int> & kp_l, vec<double> &d) {
  /// combo  fails if n <= 2
  // Computing parameters to convert the profits to int:
/*  try {
  *((int*)1256) = 5;
  } catch (...) {
    cout << "!!!!";
  }*/ // showd that the program just stops without any exception.
  PL::iterator ip, ip1, ip_0;
  PL::const_iterator ipc;
  double maxval = 0;
//  unsigned i = 0;
  fX.clear(); fX.resize(l.size());
  for_each_in(pl_,ipc,)
    if (w[*ipc] <= W_) {
      EnsureMax(maxval, d[*ipc] * W_ / w[*ipc]);
      fX[*ipc] = true;
    }
    else break; // pl sorted
  ip_0 = pl_.begin();
//#define BYLIST
#ifdef BYLIST
  PL pl1; pl1.assign(pl_.begin(), ipc);
  BubbleList(pl1, 0.5, rndParam);
  ipc = pl1.end();
  ip_0 = pl1.begin();
#endif
  assert(maxval > 0);
#ifdef RND_SC
  const double scale = combomax/maxval;
#else
  const double scale = 1e6/maxval;
#endif
//  items.clear();
  items.resize(2*l.size());
//  items.resize(i);
//DEBUG__(  typedef multiset<pair<itype,itype>> PW_MSet;
 // PW_MSet pw;)
//  itype pmax = LONG_MIN;
  vec<item>::iterator iib = items.begin() + l.size();
  vec<item>::iterator iie = items.begin() + l.size(), ii1;
#ifdef BYLIST
  do {
//    if (w[*ipc] <= W_) {
    -- ipc;
#endif
#define BLR_Bubble
#ifdef BLR_Bubble
  for_each_in(*pplKP, ipc, )
  if (fX[*ipc]) {
#endif
//#define COMBOTOWER
//#ifdef COMBOTOWER
//    if (rndParam > -1)
      ii1 = iie++; // just according to the sequence
  //  else
//#endif
    //  ii1 = --iib;
//      items.push_back(item());
    ii1->p = itype // as defined for p
        ( d[*ipc] * scale );
    ii1->w = w[*ipc];
    ii1->i0 = *ipc;
//      EnsureMax(pmax, items.back().p);
  //  }
    //else break; // pl_ sorted
//    ++i;
  }
#ifdef BYLIST
  while (ip_0 != ipc);
#endif

//  assertm(pmax > 0, "pmax="<<pmax<<", the_vscale="<<the_vscale);
//  stype res__ =
    combo(&*iib, &*(iie-1), W_, 0, //f70 ? (stype)7e5 :
            200000000, 1, 0);
//#define PRINT_COMBO
#ifdef PRINT_COMBO
  stype res2__ =
    combo(&*iib, &*iie-1, W_, 0, 2000000, 1, 0);
  log__(
    " COMBO: n="<<(iie-iib)<<" W="<< W_ <<" Z="<< res__ << " Z2="<<res2__
  );
  cin.get();
#endif

  fX.clear(); fX.resize(l.size());
  for(ii1 = iib;ii1 < iie; ++ii1)
    if (ii1->x) {
      fX[ii1->i0] = true;
      DEBUG__(assert(ii1->w==w[ii1->i0]);)
      DEBUG__(assert(ii1->p==itype(d[ii1->i0] * scale));)
    }
  kp_l.clear();
  ip = pl_.begin();
  while(ip!=pl_.end()) { // splicing the list:
    ip1 = ip ++;
    if (fX[*ip1])
      kp_l.splice(kp_l.end(), pl_, ip1); // this already gives another sorting!
  }
}

void Sub_RCPSP::IntegrateNewHole(Block& b, Block::iterator ie) {
  assert(b.size());
  assert(b.end() != ie);
  b.waste += w[ie->i];
  b.erase(ie);
}

void Sub_RCPSP::DoneSinglePass() {
  assert(blocks.back().y == res.L);
  DEBUG__(blocks.SelfCheck(*this, res);)
  DEBUG__(res.SelfCheck(*this, 1);)
  if (res.L < res_best.L)
  { res_best=res; }
}


} // bg_rcpsp

