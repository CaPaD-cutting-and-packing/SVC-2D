#include "firsthdr.h"
#include "bottomleft.h"

// INITIALIZE EVERYTHING IN APPROPRIATE PHASE _____
// -- this should have helped so often!
namespace bg {

ClassDblParam(BL, fImproved, 1, "Choose left/right justification acc. to area");
double pInvDef[] = {0, 0.03}; // 0.0x !!
ClassVectorParam(BL, pInvertShove, pInvDef, "Probability min/max to invert this rule");

void BL::SinglePass(PL& pl) {
  PL::iterator ii;
  InitSinglePass();
  for_each_in(pl, ii, )
    PutItem(ii);
  DoneSinglePass();
}

void BL::InitSinglePass() {
/// INIT DUMMY BLOCK:
  blocks.Reset(W);
  res.Init(*this); // set coords to -1
  the_pInvertShove = rndParam.InRange
    (pInvertShove.val[0], pInvertShove.val[1]);
}

void BL::PutItem(PL::iterator ii) {
  BlockStruc::iterator ib;
  Block::iterator ie;
  HoleList::iterator il, il2;
  lh1.clear(); // new list
/// Searching for a hole:
  for_each_in(blocks, ib, ) {
    lh0.clear(); // prev. list
    lh0.splice(lh0.end(), lh1); // taking over the last list, lh1 is empty
    for_each_in(ib->GetList(), ie, )
      if (ie->IsHole())
        if (ie->W() >= w[*ii]) { // only if the item fits
          bool fStartNewHole = 1;
          int rightMax = -1;
          for_each_in(lh0, il, ) { // the simplest: the whole hole list
            DEBUG__(if (il != lh0.begin())
              assert(il->x >= (--(il2=il))->x);) // non-falling x
            if (il->x + il->w < ie->X()) // if strictly to the left
              continue;
            if (il->x > ie->X() + ie->W())
              break; // BREAK if x >= ie->x + w[i] <= lh0 sorted by x
            if (il->x <= ie->X())
              fStartNewHole = 0;
              // = 0 if <=: could still cover. WHAT about the last part?
            EnsureMax(rightMax, il->x + il->w);
            if (rightMax < ie->X() + ie->W()) {
              if (lh0.end() == ++(il2=il))
                { fStartNewHole = 1; }
              else
                if (il2->x > rightMax) // !!! not +1
                  fStartNewHole = 1;
            }
            /// pos of the new hole:
            int xL = IMax(il->x, ie->X());
            int xR = IMin(il->x + il->w, ie->X() + ie->W());
            if (xR-xL >= w[*ii])
              if (NewHole_or_PutItem(xL,xR-xL,il->ib,ib,ii)) return;
            /// + if new item fits? if (NewHole()) return; // with fitting
          }
          if (fStartNewHole)
            if (NewHole_or_PutItem(ie->X(),ie->W(),
              ib,ib,ii)) return;
        }
  }
  assertm(0, "BL: Item "<<(*ii)<<" could not be packed.");
}

/// true if item put
bool BL::NewHole_or_PutItem(int x, int w_,
  BlockStruc::iterator ib_origin, // contains also the start height
  BlockStruc::iterator ib_now, PL::iterator ii) {
  assert(ib_now != blocks.end());
  if (ib_now->y + ib_now->len - ib_origin->y
      < l[*ii]) { // w_ >= w[i] assumed
    /// Searching for the position to insert:
    HoleList::iterator ih;
    for_each_in(lh1, ih, )
      if (ih->x > x) break;
    lh1.insert(ih, Hole(x,w_,ib_origin));
    DEBUG__(assert(x+w_ <= W);)
    return false;
  } // ELSE:
/// Inserting the item: transform the block(s).
/// Should there always be the last "dummy" block?
  int y_top = ib_origin->y + l[*ii];
  BlockStruc::iterator ib_last = ib_now;
/// Splitting the last block if necessary
  if (y_top < ib_now->y + ib_now->len) {
    ib_last = blocks.insert(ib_now, *ib_now);
    ib_last->len = y_top - ib_last->y;
    ib_now->y = y_top;
    ib_now->len -= ib_last->len;
    if (ib_origin == ib_now)
      ib_origin = ib_last;
  }
  ++ ib_last; // including the last block!!
  if (fImproved) {
    double areaL=0, areaR=0;
    for (BlockStruc::iterator ib = ib_origin; ib != ib_last; ++ib)
      ComputeAreaLR(ib,x,w_,areaL,areaR);
    bool fShoveLeft = (areaR > areaL);
    if (rndParam < the_pInvertShove)
      fShoveLeft = not fShoveLeft;
    if (not fShoveLeft)
      x += (w_-w[*ii]);
  }
#ifndef NDEBUG
    for (size_t i = 0;i<l.size(); ++i)
      if (res.x[i] >= 0) {
        bool relpos[] = {
          res.x[i] + w[i] <= x, x + w[*ii] <= res.x[i],
          res.y[i] + l[i] <= ib_origin->y,
            ib_origin->y  + l[*ii] <= res.y[i]
        };
        assert(relpos[0] + relpos[1] + relpos[2] + relpos[3] >= 1);
      }
#endif
  res.x[*ii] = x;
  res.y[*ii] = ib_origin->y;
  for (BlockStruc::iterator ib = ib_origin; ib != ib_last; ++ib) {
    InsertItem(ib,x,*ii);
  }
  return true;
}

void BL::ComputeAreaLR(BlockStruc::iterator ib, int x, int w_, //w_ is the _hole_ width!
  double& aL, double& aR) {
  Block::iterator ie;
  for_each_in(ib->GetList(), ie, )
    if (ie->X() > x) break;
  -- ie;
//  assert(ib->IsHole(ie));
  aL += double(ib->len) * (x-ie->X());
  aR += double(ib->len) * (ie->X()+ie->W()-x-w_);
}

void BL::InsertItem(BlockStruc::iterator ib, int x, int i) {
  assert(x+w[i] <= W);
  Block::iterator ie, ie_next;
  for_each_in(ib->GetList(), ie, )
    if (ie->X() > x) break;
  ie_next = ie--;
  assert(ie->IsHole());
  if (x > ie->X()) // A smaller hole to the left?
    ib->GetList().insert(ie, ib->MakeHole(ie->X(), x-ie->X()));
  if (x+w[i] < ie->X() + ie->W()) // and to the right?
    ib->GetList().insert(ie_next,
      ib->MakeHole(x+w[i], ie->X()+ie->W()-x-w[i]));
  ie->SetX(x);
  ie->SetI(i);
}

void BL::DoneSinglePass() {
  res.L = blocks.back().y;
  DEBUG__(blocks.SelfCheck(*this, res);)
  DEBUG__(res.SelfCheck(*this);)
  if (res.L < res_best.L)
  { res_best=res; }
}


} // bg
