#include "firsthdr.h"
#include "rcpsp_svc.h"
//#include "1dlp.h"

// INITIALIZE EVERYTHING IN APPROPRIATE PHASE _____
// -- this should have helped so often!
namespace bg_rcpsp {

// PARAMS: Read them in MyApp()
ClassDblParam(SVC_RCPSP, MaxTime, 60, 0); // for Bortfeldt on Opteron 250
ClassDblParam(SVC_RCPSP, MaxIter, 1e100, 0);

double valresdef[] = {1000, 19000};
ClassVectorParam(SVC_RCPSP, ValReset, valresdef,
  "How often the values are reset to item areas: min/max of the waiting interval");
double blockw_pow_def[] = {2.1, 0.1};
ClassVectorParam(SVC_RCPSP, blockw_pow, blockw_pow_def,
  "MEAN, STDDEV: overprop-ty (power) of the local item area in each block in correction");
ClassDblParam(SVC_RCPSP, p_bwpnew, 0.1,
  "Probability to regenerate blockw_pow in an iteration");
double baseOrdDef[] = {2,1,1,1};
ClassVectorParam(SVC_RCPSP, BaseOrdering, baseOrdDef,
  "When constructing a random priority list, take a perturbation of falling:"
  " (1. height, 2. area, 3. width, 4. perim.) with these rel."
  " frequencies, corresp.-ly");
double p_choose_def[] = {0.6, 0.6};
ClassVectorParam(SVC_RCPSP, p_choose, p_choose_def,
  "min, max of the probability to choose a swap when perturbating an ordering;"
  " generated once for every ordering");
//ClassDblParam(SVC_RCPSP, fContValues, 1,
//  "Keep pseudo-values between iterations ");

const char tickSymb[] = {'\\','|','/','-'};

void SVC_RCPSP::Run() {
  Timer tm; tm.start();
  InitRun(); // just accepting the ordering of the data
  for (iter=0;iter<MaxIter && L_best > matBound;) {
    ++iter;
    SinglePass();
#ifndef WIN32
    if (fOutputTimer) {
      fOutputTimer = 0;
#endif
//      log__(tickSymb[nTimerTicks%(sizeof(tickSymb)/sizeof(tickSymb[0]))]<<'\b');
//      if (0 == nTimerTicks % 20)
  //      log__('('<<iter<<':'<<blocks.back().y<<") ");
    /// Xhecking time:
      tm_last = tm.totalCPUTime();
      fTimeLimit = (tm_last >= MaxTime);
#ifndef WIN32
    }
#endif
    assert(blocks.back().y >= matBound);
    if (L_best > blocks.back().y) { // is there a whole in the end ?
      L_best = blocks.back().y;
      iter_best = iter;
      log__(iter<<':'<<L_best<<' ');
      nTimerTicks = 1;
      tmrow.push_back(make_pair(tm.totalCPUTime(), L_best));
    }
//    if (L_best == blocks.back().y)  // An equally good solution!
  //    SavePrmVals();
    if (fTimeLimit) break;
  }
  tm_svc = tm.totalCPUTime();
//  log__("virt/total times: "<<tm.userTime()<<','<<tm_svc);
}

void SVC_RCPSP::InitRun() {
  L_best = INT_MAX;
  matBound = MaterialBound();
  log__("MatBnd="<<matBound<<' ');
  fTimeLimit = 0;
  valreset_next = 1;
  nTickCnt = 0;
//  valreset_nextinc = ValReset.val[0];
  assertm(l.size()>0 && w.size() && l.size()==w.size() && W>0,
    l.size() <<  ' '<<w.size()<<' '<<W);
  pl.clear();
  d.resize(l.size());
  dr.resize(l.size());
  for (size_t i=0;i<l.size();++i) { // PRIORITY LIST:
    assert(w[i]<=W && w[i]>0 && l[i]>0);
    pl.push_back((int)i); // initial fill
///    dr[i] = l[i] + rndDr; // To sort by falling heights
  }
  /// pl.sort(ReverseCmpByArray<double>(&dr[0])); NOT needed, InitPL first
/// Base Orderings:
  sum_p_plm = accumulate(BaseOrdering.val.begin(), BaseOrdering.val.end(), 0.0);
  vec<vec<double> > d_pl(4, vec<double>(l.size()));
  for (size_t i=0;i<l.size(); ++i) {
    d_pl[0][i] = l[i]; // height
    d_pl[1][i] = double(w[i]) * l[i]; // area
    d_pl[2][i] = w[i]; // width
    d_pl[3][i] = w[i] + l[i]; // perim.
  }
  plBase = vec<PL>(4);
  for (int k=0;k<4;++k) {
    plBase[k] = pl;
    plBase[k].sort(ReverseCmpByArray<double>(&d_pl[k][0]));
  }
  plSub = plBase[2]; // by falling wi
  plSub.reverse(); // for Sub
}

void SVC_RCPSP::SinglePass() {
  InitIteration();
  NewPriorityList();
  blocks.clear();
/*  if (fSub_BL) {
    bl.SinglePass(pl);
    blocks.splice(blocks.end(), bl.GetBlockStruc());
  } else */
  {
    sub.SinglePass(plSub, pl, d); // plSub sorted by growing w's!
    blocks.splice(blocks.end(), sub.GetBlockStruc());
  }
  /// ????
}

void SVC_RCPSP::InitIteration() {
 // Init pseudo-values before running the decoder
  /// PARAMS
//  if (1 == iter or rndParam < p_fLocalW_new)
  //  fLocalWaste = (rndParam < corr2D_fLocalWaste);
  fSub_BL = 0; //(fmod(iter-1, FMax(1,(NIterRandom + NIterSVC))) < NIterRandom); // <! And iter - 1
  //(rndParam < Decoder_Sub_BL);
  if (rndParam < p_bwpnew or 1 == iter) // !!!!!
    blockw_pow__ = rndNormal(blockw_pow.val[0], blockw_pow.val[1], rndParam);

//  if ((iter>1 and (!int(ValuesResetPeriod)) or//fContValues &&
//    iter % int(ValuesResetPeriod) != 1)) return;
  if ((valreset_next) != iter) return;
  valreset_next
    += ceil(rndParam.InRange(ValReset.val[0], ValReset.val[1]));//valreset_nextinc;
  log__(tickSymb[++nTickCnt%(sizeof(tickSymb)/sizeof(tickSymb[0]))]<<'\b');
//  log__(log__(iter << ":VR ");
//  valreset_nextinc += ValReset.val[1]; // the second element
    /// i.e. INIT values only in the beginning or at reset
  for (size_t i=0;i<l.size();++i)
    d[i] = l[i]*w[i]; // Init to areas, 2D scheme
}

void SVC_RCPSP::NewPriorityList() {
  assert(pl.size() == l.size()); // The Derived did not change it
  dr.resize(l.size());
  d_sub.resize(l.size());
//  DrMax = // ??? exp( - log(10.0) * (5.0 * rndDrMax + 1) );
  //  MaxPerturb_RelVal * rndDrMax;
  if (true //NIterSVC // if needed!
    && blocks.size()) {
    BlockStruc::iterator ib=blocks.begin(), ib_last=blocks.end();
    -- ib_last; // the last block is a hole!
    for ( ; ib != ib_last; ++ib )
      CorrectValuesFor(*ib);
  }
//  bool fThisRnd = (fmod(iter, (NIterRandom + NIterSVC)) < NIterRandom);
//  if (fThisRnd) ALWAYS! Need for sub
  plMethod = GetPLMethod(); // if <4, sets pl already
      // + perturb. method: also + rnd*eps?
/*  if (!fThisRnd or plMethod > 3) {
    for (size_t i=0;i<l.size();++i) {
      if (fThisRnd) {
        dr[i] = rndDr; // just random
        d_sub[i] = dr[i] * w[i] * l[i];
        assert(d_sub[i] < 1e90 and d_sub[i] > 0);
      /// IF KNAPSACK: WHAT ABOUT RANDOMNESS? NOTHING YET
      } else { // perturbed pseudo-values: -- not by DrMax:
//        dr[i] = d[i]/l[i]/w[i] + MaxPerturb_RelVal * rndDr;
        d_sub[i] = d[i]; // just equal
  //      assert(d_sub[i] < 1e90 and d_sub[i] > 0);
      }
    }
    if (fThisRnd)
      pl.sort(ReverseCmpByArray<double>(&dr[0])); // -- No subFFD
  }
*/
}

double glbm, glbs;

void SVC_RCPSP::CorrectValuesFor(Block &b) {
  Block::iterator ie;
  int waste = b.waste; // 1D waste!
  assert(waste < W && waste >= 0);
  double wratio = double(waste) / (W-waste);
  for_each_in(b, ie, )
  {
      d[ie->i] +=
//            double(l[ie->I()])*w[ie->I()] * - a re-coding oversight
             pow(double(b.len) * w[ie->i], blockw_pow__)
            * ( //fLocalWaste ? ComputeLWRatio(b, ie) :
              wratio);
/*      assertm(d[ie->I()] < 1e90 and d[ie->I()] > 0,
        "d["<<ie->I()<<"] = "<<d[ie->I()]
        <<" pm, ps = "<<glbm<<' '<<glbs
        <<" p="<<blockw_pow__
        <<" wratio="<<wratio);
*/    }
}

int SVC_RCPSP::GetPLMethod() { // inits pl already
  int plm;
/// Generating plm:
  assert(sum_p_plm > 0);
  double rsum = rndParam * sum_p_plm, sum=0;
  size_t i;
  for (i=0; i<BaseOrdering.val.size(); ++i)
    if ((sum += BaseOrdering.val[i]) > rsum) break;
  plm = IMin((int)i, (int)BaseOrdering.val.size()-1); // 5 should not happen
  if (plm > 3) return plm; // pure random
  PerturbBaseOrdering(plBase[plm], pl, d_sub);
  return plm;
}

/// fills pl and d_kp
void SVC_RCPSP::PerturbBaseOrdering(const PL& plb, PL& pl, vec<double>& d_kp) {
  PL::iterator ii, ii1, ii_last;
  pl = plb;
  p_choose__ = rndParam*(p_choose.val[1]-p_choose.val[0]) + p_choose.val[0];
  BubbleList(pl, p_choose__, rndParam);
  /// Constructing corresp. pseudo-values for knapsack:
//  d_kp.resize(pl.size());
//  int i=pl.size()-1;
//  for_each_in(pl, ii, ) {
//    d_kp[*ii] = double(w[*ii]) // not * l[*ii] because kp values based on w
//      * ( 1.0 + double(i)/pl.size() ); // ?????????
//    assert(d_kp[*ii] < 1e90 and d_kp[*ii] > 0);
//    -- i;
//  }
}

void SVC_RCPSP::WriteSolution(ostream & os) {
    sub.WriteSolution(os);
}

const char SVC_RCPSP::StatStr[] = "fSub_BL plMethod blockw_pow__ "
     "sub.the_vscale ";

void SVC_RCPSP::SavePrmVals() {
  ofstream ofs("paramval.txt", ios::app|ios::out);
  if (!ofs) return;
  if (!ofs.tellp())
    ofs << "L_best " << StatStr;
  ofs <<
     L_best << ' ' << fSub_BL << ' ' << plMethod << ' '
    << blockw_pow__  << ' '
     << sub.the_vscale  << '\n';

  if (!_prm_stat) return;
  _prm_stat->NextRun()
     << fSub_BL << plMethod;
  if (!fSub_BL)
    (*_prm_stat) << blockw_pow__
     << sub.the_vscale;

  else
   (*_prm_stat) << 0.0<< 0.0<< 0.0<< 0.0;
  _prm_stat->copy_diff_into(*_prm_stat_all);
}

} // bg

