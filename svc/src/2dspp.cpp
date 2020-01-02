#include "firsthdr.h"
#include "2dspp.h"
#include "1dlp.h"

// Statistics & Solver separately

namespace bg {

bool ReadAssignment(char*s,const char*name,double& v) {
      char *p=strstr(s,name), *pend;
      if (p) { // READ "name=value" OR "name value"
        p += strlen(name);
        while (' '==*p or '\t'==*p) ++p;
        if ('=' == *p) ++p;
        v = strtod(p, &pend);
        return (p!=pend);
      }
      return 0;
}

__2DSPP_input Read_2DSPP(istream & is) {
  int W=0;
  vec<int> w,l,w0;
  int m, n2=0;
  int lb=0, ub = INT_MAX;
  static char pr_name[1024] = "";
  char buf[1024];
  is.clear(); is >> m;
  while (not is and not is.eof() and n2<10) {
    is.clear(); // is.ignore(30000,'\n'); }
    is >> setw(sizeof(pr_name)) >> pr_name;
    is.getline(buf, sizeof(buf));
    if (strlen(buf)) {
      double v;
      if (ReadAssignment(buf,"UB",v)) ub=int(v);
      if (ReadAssignment(buf,"LB",v)) lb=int(v);
    }
    is >> m; ++n2;
  } //while (!is && !is.eof() && n2<10);
  if (is.eof())
    return __2DSPP_input(0,w0,w0);
  if (n2>=10)
  { PRINT_ERROR("Reading error."); return __2DSPP_input(0,w0,w0); }
  is >> W;
  log__("m="<<m<<", W="<<W<<' ');
  if (!is)
  { PRINT_ERROR("Reading error."); return __2DSPP_input(0,w0,w0); }
  if (m<1 || W<1)
  { PRINT_ERROR("Bad m or W."); return __2DSPP_input(0,w0,w0); }
  l.resize(m); w.resize(m);
  int i;
  for (i=0;i<m;i++) {
    is >> w[i] >> l[i];
    if ((l[i]<=0) or (w[i]<=0) or (w[i] > W)) {
      PRINT_ERROR("Bad item data.");
      return __2DSPP_input(0,w0,w0);
    }
  }
  if (is || (!is && is.eof()))
    return __2DSPP_input(W,w,l,strlen(pr_name)?pr_name:0,lb,ub);
  return __2DSPP_input(0,w0,w0);
}

void __2DSPP::Write(ostream& os,int ub_,const char*fln,int inst) {
  if (pr_name)
    os << pr_name;
  else
    if (fln)
      os << fln << '_' << inst;
    else
      os << "inst_name?";
  os << " LB="<<lb<<" UB="<<IMin(ub, ub_);
  os << '\n' << l.size()
    << '\n' << W << '\n';
  for (unsigned i=0;i<l.size();++i)
    os << w[i] << '\t' << l[i] << '\n';
  os << '\n';
}

int __2DSPP::MaterialBound() {
  double area=0;
  for (unsigned i=0;i<l.size();++i)
    area += double(l[i]) * w[i];
  return int(ceil(area/W));
}

void __2DSPP_output::SelfCheck(const __2DSPP_input& inp, int fPSP) const {
  assert(inp.l.size() == x.size());
  assert(x.size() == y.size());
  for (unsigned i=0;i<x.size();++i) { // Remember: was init to -1
    if (!fPSP)
      assert(x[i]>=0 && x[i]+inp.w[i] <= inp.W);
    assert(y[i]>=0 && y[i]+inp.l[i] <= L);
    if (!fPSP)
      for (unsigned j=i+1;j<x.size(); ++j) {
        assert( (x[i]+inp.w[i]<=x[j]) || (x[j]+inp.w[j]<=x[i]) ||
            (y[i]+inp.l[i]<=y[j]) || (y[j]+inp.l[j]<=y[i]) );
    }
  }
}

void __2DSPP::WriteSolution(ostream &os) {
  os << "\n\\begin{figure}[htbp]\n";
  os << "\\setlength{\\unitlength}{1mm} % adjust here"
    "\n\\begin{picture}("<<W<<','<<res_best.L<<")\n";
  for (unsigned i = 0; i< l.size();++i)
    os << "\\put("<<res_best.x[i]<<','<<res_best.y[i]
      <<"){\\framebox("<<w[i] << ',' << l[i]<<"){\\tiny "
      << w[i] << 'x' << l[i] << "}}\n";
//    os << i+1 << ": " << w[i] << 'x' << l[i]
  //    <<" at ("<<res_best.x[i]<<','<<res_best.y[i]<<")\n";
  os << "\\end{picture}\n";
  os << "\\caption{\\label{figgg}Instance ???: m=" << l.size() << " W="<<W << '\n';
  os << "Dolution with H=" << res_best.L << "}\n";
  os << "\\end{figure}\n";
}

void Create2DSPPModel
(__2DSPP &prob, const char * fln, int inst, int ub0) {
//  PRINT(" ");
#ifdef __2SPPModel
  int lb0 = prob.MaterialBound();
//  int ub0 = dynamic_cast<SVC_Sub&>(prob).L__;
  int x,y,z,i,k, m=prob.l.size();
  ostringstream oss;
  oss << fln << '_' << inst << ".lp" << ends;
  ofstream os(oss.str().c_str());

  log_ln("Model: LB="<<lb0<<", writing to "<<oss.str().c_str());
  os <<    // CHECK LIMITS
    "Minimize\n"
    "  H\n"
  // ============== CONSTRAINTS
    "Subject To\n  ";
  for (y=lb0+1; y <= ub0; ++y)  // ZF of MODEL 1Dch
    os << " +s"<<y;
  os << " - H <= "<<-lb0<<'\n';
  for (y=lb0+1; y < ub0; ++y) // constr1
    os << "  s"<<y<<" - s"<<y+1<<" >= 0\n";
  for (i=0;i<m;++i) // constr2
    for (y=lb0+1; y <= ub0; ++y)
      os << "  s"<<y<<" - t"<<i<<'_'<<y<<" >= 0\n";
  for (i=0;i<m;++i) { // constr3
    for (y=prob.l[i]; y <= ub0; ++y)
      os << " +t"<<i<<'_'<<y;
    os << " = 1\n";
  }
  for (y=1; y <= ub0; ++y) { // constr4
    for (i=0;i<m;++i)
      for (z=IMax(y, prob.l[i]); z < y+prob.l[i]; ++z)
        os << " +"<<prob.w[i]<<'t'<<i<<'_'<<z;
    os << " <= "<<prob.W<<'\n';
  }
  // t0y = 0 in Bounds
// ============================
  for (i=0;i<m;++i) { // constr1 of MODEL 1Dcv'
    for (x=prob.w[i]; x <= prob.W; ++x)
      os << " +u"<<i<<'_'<<x;
    os << " = 1\n";
  }
  for (x=1; x <= prob.W; ++x) { // constr2
    for (i=0;i<m;++i)
      for (z=IMax(x, prob.w[i]); z < x+prob.w[i]; ++z)
        os << " +"<<prob.l[i]<<'u'<<i<<'_'<<z;
    os << " - H <= 0\n";
  }
  // u0x = 0 in Bounds
// ============================
  for (i=0;i<m;++i) { // constr1 of the Padberg MODEL
    for (y=prob.l[i]; y <= ub0; ++y)
      os << " +"<<(y-prob.l[i])<<'t'<<i<<'_'<<y;
    os << " - y"<<i<<" =0\n";
    for (x=prob.w[i]; x <= prob.W; ++x)
      os << " +"<<(x-prob.w[i])<<'u'<<i<<'_'<<x;
    os << " - x"<<i<<" =0\n";
  }
  for (i=0;i<m;++i)
    for (k=i+1;k<m;++k)
      os
        << "  x"<< i <<" - x"<<k<<" +"<<prob.W<<"dx"<<i<<'_'<<k
          <<" <= "<< prob.W-prob.w[i] <<'\n'
        << "  x"<< k <<" - x"<<i<<" +"<<prob.W<<"dx"<<k<<'_'<<i
          <<" <= "<< prob.W-prob.w[k] <<'\n'
        << "  y"<< i <<" - y"<<k<<" +"<<ub0<<"dy"<<i<<'_'<<k
          <<" <= "<< ub0-prob.l[i] <<'\n'
        << "  y"<< k <<" - y"<<i<<" +"<<ub0<<"dy"<<k<<'_'<<i
          <<" <= "<< ub0-prob.l[k] <<'\n'
        // + delta - relations:
        << " dx"<<i<<'_'<<k<< " +dx"<<k<<'_'<<i
          << " +dy"<<i<<'_'<<k<< " +dy"<<k<<'_'<<i
          << " >= 1\n"
        << " dx"<<i<<'_'<<k<< " +dx"<<k<<'_'<<i
          << " <= 1\n"
        << " dy"<<i<<'_'<<k<< " +dy"<<k<<'_'<<i
          << " <= 1\n";
  // ============== VARIABLES
  os << "Bounds\n";
  for (i=0;i<m;++i)
    os <<"  0<= x"<<i<<" <="<<prob.W-prob.w[i]
      <<"\n  0<= y"<<i<<" <=" <<ub0-prob.l[i]<<'\n';
  for (y=lb0+1; y <= ub0; ++y) // MODEL 1Dch
    os << " 0 <= s"<<y<<" <= 1\n";
  for (i=0;i<m;++i)
    for (y=prob.l[i]; y <= ub0; ++y)
      os << " 0 <= t"<<i<<'_'<<y<<" <= 1\n";
  for (i=0;i<m;++i)    // MODEL 1Dcv'
    for (x=prob.w[i]; x <= prob.W; ++x)
      os << " 0 <= u"<<i<<'_'<<x<<" <= 1\n";
  for (y=prob.l[0]; y < lb0/2+prob.l[0]/2; ++y) // constr5
    os << "  t0_"<<y<<" = 0\n";
  for (x=prob.w[0]; x < prob.W/2+prob.w[0]/2; ++x) // constr3
    os << "  u0_"<<x<<" = 0\n";
  os<<"Binaries\n";
  for (y=lb0+1; y <= ub0; ++y) // MODEL 1Dch
    os << " s"<<y<<'\n';
  for (i=0;i<m;++i)
    for (y=prob.l[i]; y <= ub0; ++y)
      os << " t"<<i<<'_'<<y<<'\n';
  for (i=0;i<m;++i)    // MODEL 1Dcv'
    for (x=prob.w[i]; x <= prob.W; ++x)
      os << " u"<<i<<'_'<<x<<'\n';
  for (i=0;i<m;++i)
    for (k=i+1;k<m;++k)
      os
        << " dx"<<i<<'_'<<k<< " dx"<<k<<'_'<<i
          << " dy"<<i<<'_'<<k<< " dy"<<k<<'_'<<i<<'\n';
  os << "End\n";
#else
  log__("MILP Model writing not compiled! ");
#endif
}

void __2DSPP_1DLP::Solve() {
  Timer tm; tm.start(); double tm_all=0, tm_cg=0;
  int midb=0, lb, ub;
  log__(" 1dh..");
  __1DLP __1dh(pr.W, pr.w, pr.l); // horizontal;
  __1dh.SolveLP();
  tm_all += __1dh.tm_all; tm_cg += __1dh.tm_cg;
  tmh = tm.userTime();
  log__('='<<__1dh.LPVal());
  assert (__1dh.LPBnd() <= __ub);
  lbv = lbh = __1dh.LPBnd();
  if (__1dh.LPBnd() == __ub)
    { log__("=UB!!"); }
  else {
    lb = lbh;
    ub = __ub;
    midb = lb;
    for (int ci=0; lb != ub; ++ ci) {
      log__(' '<<ci<<".1dv("<<midb<<")..");
      __1DLP __1dv(midb, pr.l, pr.w);
      __1dv.SolveLP();
      tm_all += __1dv.tm_all; tm_cg += __1dv.tm_cg;
      log__('='<<__1dv.LPVal());
      if (__1dv.LPBnd() > pr.W)
        lb = midb+1; // !!!
      else
        ub = midb;
      midb = (lb+ub)/2;
      lbv = midb;
    }
  }
  tmv = tm.userTime()-tmh;
  cg = int(Del0(tm_cg, tm_all)*100.0)/100.0;
  log_ln(" //1D_times "<<tmh<<','<<tmv
    <<','<< cg
    <<" => UB="<<__ub<<", LBh="<<lbh<<", LBv="<<lbv);
//      << setw(4) << __ub << ' '
}

} // bg

