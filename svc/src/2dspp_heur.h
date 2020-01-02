#ifndef __HEUR_2DSPP__H
#define __HEUR_2DSPP__H

#include "2dspp.h"

// File: Heuristics for 2D SPP
namespace bg {

// Every proc s.b. independetn OR clearly define assertions

typedef List<int> PL; // priority list

class BlockElement {
  int x,i;
public:
  BlockElement(int x_,int i_) :x(x_), i(i_) { }
  bool IsHole() const { return (i<0); }
  int GetX() const { return x; }
  int GetW() const { DEBUG__(assert(i<0);) return -i; }
  int GetI() const { DEBUG__(assert(i>=0);) return i; }
  int X() const { return GetX(); }
  int W() const { return GetW(); }
  int I() const { return GetI(); }
  void SetX(int x_) { x=x_; }
  void SetW(int w_) { i=-w_; }
  void SetI(int i_) { i=i_; }
};
typedef List<BlockElement> BlockListType;

class Block
  : public BlockListType {
public:
  int y; // start height
  int len;
  Block(int y_=0, int l_=INT_MAX) : y(y_), len(l_) { }
  typedef BlockElement ElemType;
  typedef BlockListType ListType;
  BlockListType & GetList() { return *this; }
  const BlockListType & GetList() const { return *this; }
  static ElemType MakeHole(int p1, int w) {
    return ElemType(p1,-w); // coding hole width by neg. i
  }
  static bool IsHole(const ListType::iterator ie) {
    return ie->IsHole();
  }
  static int HoleWidth(const ListType::iterator ie) {
    return ie->GetW();
  }
  static int GetX(const ListType::iterator ie) {
    return ie->GetX();
  }
  void SelfCheck(const __2DSPP_input& inp,
      const __2DSPP_output& outp) const {
    for_each_in(GetList(), ie, const_iterator) {
      if (ie != begin()) {
        const_iterator ie_ = ie; --ie_;
        int w_ = ie_->IsHole() ? ie_->W() : inp.w[ie_->I()];
        assert(ie_->X() + w_ == ie->X());
      }
      if (not ie->IsHole())
        assert(ie->X() == outp.x[ie->I()]);
    }
  }
}; // class Block

class BlockStruc
  :public List<Block> {
public:
  void Reset(int W) {
    clear();
    push_back(Block(0, INT_MAX));    // Init first block
    back().push_back(Block::MakeHole(0,W));
  }
  void SelfCheck(const __2DSPP_input& inp, // not checking total height
      const __2DSPP_output& outp) const {
    assert(size());
    assert(0 == front().y);
    for_each_in(*this, ib, const_iterator) {
      if (ib != begin()) {
        const_iterator ib_ = ib; --ib_;
        assert(ib_->y + ib_->len == ib->y);
      }
      ib->SelfCheck(inp, outp);
    }
  }
}; // class BlockStruc


template <class L>
void BubbleList(L& pl, const double pp, Random & // & !!!
  rnd) {
  typename L::iterator ii, ii1, ii_last;
  dbg_assert(pl.size());
  ii = pl.begin();
//  -- (ii_last = pl.end());
  for ( ; ii != pl.end(); ++ii)
    for (ii1 = ii; ii1 != pl.end(); ++ ii1)
      if (rnd < pp) {
        if (ii1 != ii)
          pl.splice(ii, pl, ii1); // FROM ITSELF!
        break;
      }
}


} // namespace bg

#endif // __HEUR_2DSPP__H

