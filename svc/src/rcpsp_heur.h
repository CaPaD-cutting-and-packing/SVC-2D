#ifndef __HEUR_RCPSP__H
#define __HEUR_RCPSP__H

#include "2dspp.h"

// File: Heuristics for 2D SPP
namespace bg_rcpsp {

using namespace bg;
// Every proc s.b. independetn OR clearly define assertions

typedef List<int> PL; // priority list

class BlockElement {
public:
  int i; // item index
  BlockElement(int i_) : i(i_) { }
};
typedef List<BlockElement> BlockListType;

class Block
  : public BlockListType {
public:
  int y; // start height
  int len; // block height
  int waste; // remaining width
  Block(int w_, int y_=0, int l_=INT_MAX) : y(y_), len(l_), waste(w_) { }
  typedef BlockElement ElemType;
  typedef BlockListType ListType;
  BlockListType & GetList() { return *this; }
  const BlockListType & GetList() const { return *this; }
  void SelfCheck(const __2DSPP_input& inp,
      const __2DSPP_output& outp) const {
    long sw = 0;
    for_each_in(GetList(), ie, const_iterator) {
      sw += inp.w[ie->i];
    }
    assert(sw <= inp.W);
  }
}; // class Block

class BlockStruc
  :public List<Block> {
public:
  void Reset(int W) {
    clear();
    push_back(Block(W, 0, INT_MAX));    // Init first block
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



} // namespace bg_rcpsp

#endif // __HEUR_RCPSP__H

