#include "firsthdr.h"
#include "beasley.h"
#include "raster.h"

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

namespace bg {

void Beasley2DLP::Solve2DSPP() { // + cplex log to a file?
  Timer tmr; tmr.start();
  int midb=0, lb, ub;
  log__(" Beas..");
    lb = hlb;
    ub = hub;
    midb = lb;
    for (int ci=0; lb != ub; ++ ci) {
      log__(' '<<ci<<".BS("<<midb<<")..");
      H=midb;
      SolveFixedContainer();
      log__('='<<slv.resVal-w.size()); // can be negative
      if (size_t(floor(slv.resVal+1e-6)) < w.size())
        lb = midb+1; // !!!
      else
        ub = midb;
      midb = (lb+ub)/2;
      lbRes = midb;
    }
  tm = tmr.totalCPUTime();
}

void Beasley2DLP::SolveFixedContainer() { // + cplex log to a file?
  const char* modelfile="beasley.lp";
  WriteModel(modelfile);
  slv.SolveLP(modelfile);
}

void Beasley2DLP::WriteModel(const char * modelfile) {
  unsigned i,j1,j2,ji1,ji2;
//////////// RASTER POINTS: (here we understand them as positions where items can start)
  vec<int> ub(w.size(), 1), rpw, rph;  // placeable points (0..W-1, 0..H-1)
  ConstructRP(w,ub,double(W),rpw); // + modify W, H? if rpw.back() < w ...
  ConstructRP(h,ub,double(H),rph);
  int minw = *min_element(w.begin(),w.end());
  int minh = *min_element(h.begin(),h.end());
  log__(" w");
//////////// LP MODEL:
  ofstream of(modelfile);
  of << "Maximize\n";
  for (i=0;i<w.size();++i) // Location points
    of << " + f" <<i;
  of << "\nSubject To\n";
  for (i=0;i<w.size();++i) {// ... - fi = 0
    for (j1=0;j1<rpw.size();++j1)
      if (rpw[j1] <= W-w[i]) // feasible w-region for i
      for (j2=0;j2<rph.size();++j2)
        if (rph[j2] <= H-h[i])
        of << " + z"<<i<<'_'<<rpw[j1]<<'_'<<rph[j2];
    of << " - f"<<i<<" = 0\n";
  }
  for (j1=0;j1<rpw.size();++j1) // for each RP where items can intersect
    if (rpw[j1] < W //and rpw[j1] >= minw-1 -- Wrong!
       ) // -- it is enough to check this only in RPs
    for (j2=0;j2<rph.size();++j2)
    if (rph[j2] < H //and rph[j2] >= minh-1
       ) {
      for (i=0;i<w.size();++i) // and now the region of starting points for item i covering RP j1,j2:
        for (ji1=0;ji1<=j1;++ji1)
          if (rpw[j1]-rpw[ji1] < w[i])
          for (ji2=0;ji2<=j2;++ji2)
            if (rph[j2]-rph[ji2] < h[i])
            of << " + z"<<i<<'_'<<rpw[ji1]<<'_'<<rph[ji2];
      of << " <= 1\n";
    }
  of << "Bounds\n";
  for (i=0;i<w.size();++i)
    of << " f" <<i<< "<= 1\n";
  ////////// NO BINARIES! Or?
  of << "End\n";
}

void CplexModelSolver::SolveLP(const char * modelfile) {
   log__(" r");
   IloEnv env;
   try {
      IloModel model(env);
      IloCplex cplex(env);

      ofstream ofout("beasley.out", ios::out|ios::app);
      ofout << "\n\n\n------------- "<<GetDateAndTime()<<" ------------\n\n";
      cplex.setOut(ofout);

      IloObjective   obj;
      IloNumVarArray var(env);
      IloRangeArray  rng(env);
      cplex.importModel(model, modelfile, obj, var, rng);

      log__(" s");
      cplex.extract(model);
      if ( !cplex.solve() ) {
         env.error() << "Failed to optimize LP" << endl;
         throw(-1);
      }

      IloNumArray vals(env);
      cplex.getValues(vals, var);
      env.out() << "Solution status = " << cplex.getStatus() << endl;
      env.out() << "Solution value  = " << ceil(cplex.getObjValue()) << " - " << (ceil(cplex.getObjValue()) - cplex.getObjValue()) << endl;
      env.out() << "Solution vector = " << vals << endl;

      resOpt = (IloAlgorithm::Optimal == cplex.getStatus());
      resVal = cplex.getObjValue();
/*      try {     // basis may not exist
         IloCplex::BasisStatusArray cstat(env);
         cplex.getBasisStatuses(cstat, var);
         env.out() << "Basis statuses  = " << cstat << endl;
      } catch (...) {
      }*/

      env.out() << "Maximum bound violation = "
                << cplex.getQuality(IloCplex::MaxPrimalInfeas) << endl;

      cplex.setOut(env.getNullStream());

   }
   catch (IloException& e) {
      cerr << "Concert exception caught: " << e << endl;
   }
   catch (...) {
      cerr << "Unknown exception caught" << endl;
   }

   env.end();
}

//public void importModel(IloModel & m, const char * filename, IloObjective & obj, IloNumVarArray vars, IloRangeArray rngs, IloRangeArray lazy, IloRangeArray cuts)

//This method is an simplification of the importModel method that does not provide arrays to return SOS's. This method is more easy to use in the case that you are dealing with continuous models, because in this case you already know that no SOS's will be present.


} // bg


