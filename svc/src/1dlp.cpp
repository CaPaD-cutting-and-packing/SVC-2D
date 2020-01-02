#include "firsthdr.h"
#include "1dlp.h"

// INITIALIZE EVERYTHING IN APPROPRIATE PHASE _____

//#define __USE_CPLEX // when compiled at all

#ifndef __USE_CPLEX

namespace bg {
double
SolveBKP_cplex(double W, vec<int> &w, vec<double>&p, vec<bool> &x)
{ return 0;}
void __1DLP::SolveLP() { }
} // bg

#else


#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

#define RC_EPS 1.0e-6

typedef IloArray<IloNumArray> IloNumArray2;

//  Copyright (C) 1999-2004 by ILOG.

namespace bg {

/*
class BKP_cplex_model : public VirtDestr {
public:
  IloEnv env;
  ~BKP_cplex_model() { env.end(); }
  IloModel patGen;
  IloObjective ReducedCost;
  IloNumVarArray Use;
  IloNumArray price(env, nWdth);
  BKP_cplex_model(double W, vec<int> &w)
    : patGen(env), Use(env, w.size(), 0, 1, ILOINT)
    {
      ReducedCost = IloAdd(patGen, IloMinimize(env, 1));
      patGen.add(IloScalProd(size, Use) <= W);
    }
      IloNumArray newPatt(env, nWdth);
         for (i = 0; i < nWdth; i++) {
           price[i] = -cutSolver.getDual(Fill[i]);
         }
         ReducedCost.setLinearCoefs(Use, price);

         patSolver.solve();
         patSolver.getValues(newPatt, Use);

}*/

/*  void ConstructModel();
public:
  void SetBasicInfo(double W,vec<int> &w);
  void SetPrices(vec<double> &p);
  void Solve(vec<int> &x, double &val);
*/
double
SolveBKP_cplex(double W, vec<int> &w, vec<double>&p, vec<bool> &x)
{
   double res;
   IloEnv env;
   unsigned i;
   try {
      IloModel patGen (env);

      IloObjective ReducedCost = IloAdd(patGen, IloMaximize(env, 1));
      IloNumVarArray Use(env, w.size(), 0, 1, ILOINT);

      IloNumArray size(env, w.size());
      IloNumArray newPatt(env, w.size());

      FORless(i,0,w.size()) size[i] = w[i];
      patGen.add(IloScalProd(size, Use) <= W);

      IloCplex patSolver(patGen);
      patSolver.setOut(env.getNullStream());

      FORless(i,0,w.size())
        ReducedCost.setLinearCoef(Use[i], p[i]);
      patSolver.solve();
      res = patSolver.getValue(ReducedCost);

      x.clear(); x.resize(w.size()); // ZEROs
      patSolver.getValues(newPatt, Use);
      FORless(i,0,w.size())
        if (newPatt[i] > 1e-6)
          x[i] = true;
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Non-Cplex Error." << endl;
   }

   env.end();
   return res;

}


static void readData (__1DLP &, IloNum& rollWidth,
                      IloNumArray& size, IloNumArray& amount);
static void report1 (IloCplex& cutSolver, IloNumVarArray Cut, IloRangeArray Fill);
static void report2 (ostream &, IloAlgorithm& patSolver,
                     IloNumVarArray Use,
                     IloObjective obj);
static void report3 (IloCplex& cutSolver, IloNumVarArray Cut);

void __1DLP::SolveLP() {
   IloEnv env;
   try {
      IloInt  i, j;

      IloNum      rollWidth;
      IloNumArray amount(env, l.size());
      IloNumArray size(env, l.size());

      readData(*this, rollWidth, size, amount);

      /// CUTTING-OPTIMIZATION PROBLEM ///

      IloModel cutOpt (env);

      IloObjective   RollsUsed = IloAdd(cutOpt, IloMinimize(env));
      IloRangeArray  Fill = IloAdd(cutOpt, IloRangeArray(env, amount, IloInfinity));
      IloNumVarArray Cut(env);

      IloInt nWdth = size.getSize();
      for (j = 0; j < nWdth; j++) {
         Cut.add(IloNumVar(RollsUsed(1) + Fill[j](1
           /*int(rollWidth / size[j])*/))); // BINARY PATTERNS !!!
      }

      IloCplex cutSolver(cutOpt);
      ofstream of1("cut.log"/*, ios::app*/);
      cutSolver.setOut(of1);

      /// PATTERN-GENERATION PROBLEM ///

      IloModel patGen (env);

      IloObjective ReducedCost = IloAdd(patGen, IloMinimize(env, 1));
      IloNumVarArray Use(env, nWdth, 0, 1, ILOINT);
        // BINARY PATTERNS!!!
      patGen.add(IloScalProd(size, Use) <= rollWidth);

      IloCplex patSolver(patGen);
      ofstream of2("pat.log"/*, ios::app*/);
      patSolver.setOut(of2);

      ofstream os_patterns("patt.txt"/*, ios::app*/);

      /// COLUMN-GENERATION PROCEDURE ///

      IloNumArray price(env, nWdth);
      IloNumArray newPatt(env, nWdth);

      /// COLUMN-GENERATION PROCEDURE ///

      Timer tm; tm.start();
      tm_cg = 0;
      double tm_last;

      for (;;) {
         /// OPTIMIZE OVER CURRENT PATTERNS ///

//         tm_last = tm.userTime();
         cutSolver.solve();
//         tm_lp += tm.userTime() - tm_last;
         report1 (cutSolver, Cut, Fill);

         /// FIND AND ADD A NEW PATTERN ///

         for (i = 0; i < nWdth; i++) {
           price[i] = -cutSolver.getDual(Fill[i]);
         }
         ReducedCost.setLinearCoefs(Use, price);

         tm_last = tm.userTime();
         patSolver.solve();
         tm_cg += tm.userTime() - tm_last;
         report2 (os_patterns, patSolver, Use, ReducedCost);

         if (patSolver.getValue(ReducedCost) > -RC_EPS) break;

         patSolver.getValues(newPatt, Use);
         Cut.add( IloNumVar(RollsUsed(1) + Fill(newPatt)) );
      }

    lpv = cutSolver.getObjValue();
    tm_all = tm.userTime();

 /*
      cutOpt.add(IloConversion(env, Cut, ILOINT));

      cutSolver.solve();
 */
      report3 (cutSolver, Cut);
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Non-Cplex Error." << endl;
   }

   env.end();

} // __1DLP::SolveLP()


static void readData (__1DLP & __1d, IloNum& rollWidth,
                      IloNumArray& size, IloNumArray& amount)
{
   {
      rollWidth = __1d.L;
      for (unsigned i=0;i<__1d.l.size();++i) {
        size[i] = __1d.l[i];
        amount[i] = __1d.b[i];
      }
   }
}

static void report1 (IloCplex& cutSolver, IloNumVarArray Cut, IloRangeArray Fill)
{/*
   cout << endl;
   cout << "Using " << cutSolver.getObjValue() << " rolls" << endl;
   cout << endl;
   for (IloInt j = 0; j < Cut.getSize(); j++) {
      cout << "  Cut" << j << " = " << cutSolver.getValue(Cut[j]) << endl;
   }
   cout << endl;
   for (IloInt i = 0; i < Fill.getSize(); i++) {
      cout << "  Fill" << i << " = " << cutSolver.getDual(Fill[i]) << endl;
   }
   cout << endl;*/
}

static void report2 (ostream & os, IloAlgorithm& patSolver, IloNumVarArray Use, IloObjective obj)
{
   os << endl;
   os << "Reduced cost is " << patSolver.getValue(obj) << endl;
   os << endl;
   if (patSolver.getValue(obj) <= -RC_EPS) {
      for (IloInt i = 0; i < Use.getSize(); i++)  {
         os << "  Use" << i << " = " << patSolver.getValue(Use[i]) << endl;
      }
      os << endl;
   }
}

static void report3 (IloCplex& cutSolver, IloNumVarArray Cut)
{/*
   cout << endl;
   cout << "Best integer solution uses "
        << cutSolver.getObjValue() << " rolls" << endl;
   cout << endl;
   for (IloInt j = 0; j < Cut.getSize(); j++) {
      cout << "  Cut" << j << " = " << cutSolver.getValue(Cut[j]) << endl;
   }*/
}

} // namespace bg

#endif // ifndef USECPLEX

