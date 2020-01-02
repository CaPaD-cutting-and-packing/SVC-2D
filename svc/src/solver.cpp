#include "firsthdr.h"
#include "solver.h"
#include "2dspp_svc.h"
#include "rcpsp_svc.h"
#include "beasley.h"

int main(int argc, const char * const * argv) {
  try {
    bg::Solver slv(argc, argv);
    slv.Go();
  }
  catch(const exception & e) {
    cout << e.what() << endl;
    return 1;
  }
  catch (...) {
    cout << "Unknown error." << endl;
    return 2;
  }
  return 0;
}

namespace bg {

ClassIntParam(Solver, instFirst, 1, "The first instance in a data file (count from 1)");
ClassIntParam(Solver, instLast, 1000000, "The last instance in a data file");
ClassIntParam(Solver, statPeriod, 10, "GROUPING: for how many instances, averages are calculated and reset and printed to !!stat.txt");
//ClassIntParam(Solver, fWriteMILP, 0, "Write MILP models to files data_n.lp");
ClassIntParam(Solver, fWriteSolution, 0, "Write solution of the last instance to a file");
//ClassIntParam(Solver, fWriteGrpPrm, 0,
  //"Whether the values of rnd params for the best solutions for each group in an extra file");
ClassIntParam(Solver, nPType, 0, "Problem type: 0 - 2D SPP, 1 - RC PSP");


Solver::Solver(int argc__, const char * const * argv__)
  : argc(argc__), argv(argv__) { }

void Solver::Go() {
  Init();
  ProcessArguments();
}

void Solver::Init() {
#ifndef NDEBUG
  cout << "Warning: NDEBUG _not_ defined!!!\n"
   "sizeofs(int,long,void*,list<int>::iter): "
   <<sizeof(int)<<' '<<sizeof(long)<<' '
   <<sizeof(void*)<<' '<<sizeof(list<int>::iterator)<<endl;
#endif
  bg::param::ReadParams(); // only after init. all static objects
//  bg::param::ParamRoot::fileName = "prm1.cfg";
  bg::param::WriteParams();
//  bg::param::ParamRoot::fileName = "prm2.cfg";
//  bg::param::WriteParamsShort();
  if (argc<2) {
    PRINT("SVC+BLR for 2D-SPP and SVC for RC PSP, Gleb Belov. The main file "__FILE__" was compiled on " __DATE__ " at " __TIME__ "\n"
    "Usage: " << argv[0] << " [options] datafile [[options] datafile [...]]\n"
    "Options, also available in the param file:\n"
    "  -i<first instance in a file>\n"
    "  -l<last instance>\n"
    "  -I<only instance>\n"
    "  -g<stat grouping period for !!stat.txt>\n"
    "  -w<0/1> write the best solution, no/yes\n"
    "  -t<time limit>\n"
    "  -n<iterations limit>"
//    "  -p<0/1> write the values of random params for the best solutions for each stat group to an extra file, no/yes\n"
  //  "  \n"
    );
    //PRINT(manual);
    throw logic_error("... see you.");
  }
#ifndef WIN32
  AlarmHandler();
  if (-1.34 == Alarm(1))
    log_ln("Timer interrupt probably not installed. Time limit would not work then.");
#endif
}

void Solver::ProcessArguments() {
  double num;
  int n;
  for (int i=1;i<argc;i++)
  try
  {
    if (('-'!=argv[i][0]) and ('/'!=argv[i][0])) // file name?
      ProcessFile(argv[i]);
    else {
      switch (argv[i][1]) {
      case 'i': // first instance
        if (GetNumArg(num, i, argv, argc)) {
          log_ln("Starting from instance "<<(int)num);
          instFirst.val = (int)num;
          instLast.val = IMax(instLast, (int)num);
        }
        break;
      case 'I': // A single instance
        if (GetNumArg(num, i, argv, argc)) {
          log_ln("Processing only instance "<<num);
          instFirst.val = instLast.val = (int)num;
        }
        break;
      case 'l': //  Last instance
        if (GetNumArg(num, i, argv, argc)) {
          log_ln("Last instance "<<num);
          instFirst.val = IMin(instFirst, (int)num);
          instLast.val = (int)num;
        }
        break;
      case 'g':
//        param::WriteParams();  // Save options
        if (GetNumArg(num, i, argv, argc)) {
          log_ln("Stat period "<<(int)num);
          statPeriod.val = (int)num;
        }
        break;
      case 'w':
        n = true;
        if (GetNumArg(num, i, argv, argc))
          n=(int)num;
        log_ln("The best found solution will be saved on exit: " << (n?"true":"false"));
        fWriteSolution.val = n;
        break;
      case 't':
        if (GetNumArg(num, i, argv, argc)) {
          log_ln("Time limit "<<num);
          SVC_2DSPP::MaxTime.val = bg_rcpsp::SVC_RCPSP::MaxTime.val = num;
        }
        break;
      case 'n':
        if (GetNumArg(num, i, argv, argc)) {
          log_ln("Iterations limit "<<num);
          SVC_2DSPP::MaxIter.val = bg_rcpsp::SVC_RCPSP::MaxIter.val = num;
        }
        break;
/*      case 'p':
        n = true;
        if (GetNumArg(num, i, argv, argc))
          n=(int)num;
        log_ln("Writing values of rnd params for good solutions in groups: " << (n?"true":"false"));
        fWriteGrpPrm.val = n;
        break; */
      }
    }
  }
  catch (const exception & e) {
    PRINT_LOGLN(e.what()<<flush);
  }
  catch (const UserBreak__&) {
    PRINT_LOGLN("User break.");
	return;
  }
  catch (...)
  {
    PRINT_LOGLN("Unknown exception.");
    return;
  }
}

bool Solver::GetNumArg(double &res, int &iarg,
    const char * const * argv, int argc) {
        const char * ts;
        ts = argv[iarg]; ++++ts;
        if (0 == *ts) ts =argv[++iarg];
        if (iarg<argc) {
          istringstream iss(ts);
          iss >> res;
          if (!iss) {
            log_ln("Error reading numerical parameter from command-line argument "<<iarg);
            return false;
          }
          return true;
        }
        log_ln("Numerical parameter needed as the last command-line argument!");
        return false;
}

void Solver::ProcessFile(const char * fln) {
  __glb_file = fln;
  nInst = 0;
  sumObj = 0;
  __stat.Reset();
  __stat_all.Reset(); // !!!!!
//  __stat_prm.Reset();
  //__stat_prm_all.Reset(); // !!!!!
  ifstream is(fln);
  if (!is)
  { PRINT("Error read opening "<<fln); return; }
  for (inst=1; is && (inst<=instLast); ++inst) {
    __glb_inumber = inst; // for error reporting
    if (!ProcessInstance(is, fln)) break;
  }
  if (nInst > 1 and 0 < nInst % int(statPeriod))
    PrintStat(fln);
  PrintStatAll();
}

    const char OUT_LINE[] =
         "inst  m   W   SVC: UB  it_best/all tm_best/all"
         " gap%  TmRow: 10 60 inf ave_best^"
#ifdef __LPBOUNDS
          " LP:tmh tmv cg% lbh lbv max"
#endif
#ifdef __BEASLEY
          " Bea:lb tm"
#endif
      ;

bool Solver::ProcessInstance
(istream & is, const char * fln) {
  if (0 == nPType)
    return Process2DSPP(is, fln);
  else
    return ProcessRCPSP(is, fln);
}

bool Solver::Process2DSPP
(istream & is, const char * fln) {
  __2DSPP_input pr = Read_2DSPP(is);
  if (pr.w.empty()) return 0;
  SVC_2DSPP svc(pr );//, &__stat_prm, &__stat_prm_all);
  if (inst >= instFirst) {
    ++ nInst;
    log__(fln<<':'<<inst<<' ');
    const char * const fln_local = Max(fln,
      1 + Max(strrchr(fln,'/'), strrchr(fln,'\\')));
    string fln1(fln_local);
    string fln2 = fln1;
    fln1 += ".txt";
    fln2 += "_bounds";
    ofstream logfile(fln1.c_str(), ios::app|ios::out);
    if (instFirst == inst) {
      logfile << '\n'<< GetDateAndTime() << '\n';
      param::WriteParamsShort(logfile);
      logfile << '\n' << OUT_LINE << '\n';
    }
    __stat.NextRun()
      << "%g" << inst
      << svc.l.size()
      << svc.W;
    svc.Run();
    { /// WRITE THE INSTANCE WITH BOUNDS:
      ofstream ofb(fln2.c_str(),
        (instFirst == inst) ? ios::out : ios::app|ios::out);
      svc.Write(ofb, svc.L_best, fln, inst);
    }
    sumObj += svc.L_best;
    __stat  << svc.L_best
      << "%.0f" << svc.iter_best
      << svc.iter
      << "%.3g" << svc.tmrow.back().first//double(svc.iter_best)/svc.MaxIter * svc.tm_svc
      << "%.0f" << svc.tm_svc;
      int lb0 = svc.MaterialBound(); // to be improved by LP?
    __stat
      << "%.2g" << int(double(svc.L_best-lb0)/lb0*1000)/10.0;
     /// TimeRow:
    SVC_2DSPP::TimeRow::iterator itt = svc.tmrow.begin(); //itt1,
    static const double tm_pt[] = {10, 60, // 600, //3600, 36000,
      1e100};
    int objVal = INT_MAX;
    for (unsigned itp = 0; // need output for each time point
       itp < (sizeof(tm_pt)/sizeof(tm_pt[0])); ++itp) {
         for ( ; svc.tmrow.end() != itt; ++itt)
           if (itt->first <= tm_pt[itp])
             objVal = itt->second;
           else break;
           __stat << "%g" << objVal;
    }
//    if (fWriteMILP)
  //    Create2DSPPModel(svc, fln, inst, svc.L_best);
  if (fWriteSolution) {
    ofstream ofs("solution.txt");
    if (ofs)
      svc.WriteSolution(ofs);
  }
#ifdef __LPBOUNDS
    __2DSPP_1DLP lp(svc, svc.L_best);
    lp.Solve();
    __stat << "%.1f" << lp.tmh << lp.tmv
      << "%.1f" << lp.cg
      << "%g" << lp.lbh << lp.lbv << FMax(lp.lbh, lp.lbv);
#endif
#ifdef __BEASLEY
    Beasley2DLP bs(svc.W,svc.w,svc.l,lb0,svc.L_best);
    bs.Solve2DSPP();
    __stat << bs.lbRes << bs.tm;
#endif
    __stat.print_diff(logfile, ' ');
    __stat.copy_diff_into(__stat_all);
//    __stat_prm.copy_diff_into(__stat_prm_all); NO, done for each input
    logfile << sumObj / nInst;
    logfile << '\n';
    if (0 == inst % int(statPeriod)) {
      PrintStat(fln);
      __stat.Reset();
    }
    log__(endl);
  }
  return 1;
}

bool Solver::ProcessRCPSP
(istream & is, const char * fln) {
  __2DSPP_input pr = Read_2DSPP(is);
  if (pr.w.empty()) return 0;
  bg_rcpsp::SVC_RCPSP svc(pr );//, &__stat_prm, &__stat_prm_all);
  if (inst >= instFirst) {
    ++ nInst;
    log__(fln<<':'<<inst<<' ');
    const char * const fln_local = Max(fln,
      1 + Max(strrchr(fln,'/'), strrchr(fln,'\\')));
    string fln1(fln_local);
    string fln2 = fln1;
    fln1 += "_psp.txt";
    fln2 += "_psp_bounds";
    ofstream logfile(fln1.c_str(), ios::app|ios::out);
    if (instFirst == inst) {
      logfile << '\n'<< GetDateAndTime() << '\n';
      param::WriteParamsShort(logfile);
      logfile << '\n' << OUT_LINE << '\n';
    }
    __stat.NextRun()
      << "%g" << inst
      << svc.l.size()
      << svc.W;
    svc.Run();
    { /// WRITE THE INSTANCE WITH BOUNDS:
      ofstream ofb(fln2.c_str(),
        (instFirst == inst) ? ios::out : ios::app|ios::out);
      svc.Write(ofb, svc.L_best, fln, inst);
    }
    sumObj += svc.L_best;
    __stat  << svc.L_best
      << "%.0f" << svc.iter_best
      << svc.iter
      << "%.3g" << svc.tmrow.back().first//double(svc.iter_best)/svc.MaxIter * svc.tm_svc
      << "%.0f" << svc.tm_svc;
      int lb0 = svc.MaterialBound(); // to be improved by LP?
    __stat
      << "%.2g" << int(double(svc.L_best-lb0)/lb0*1000)/10.0;
     /// TimeRow:
    bg_rcpsp::SVC_RCPSP::TimeRow::iterator itt = svc.tmrow.begin(); //itt1,
    static const double tm_pt[] = {10, 60, // 600, //3600, 36000,
      1e100};
    int objVal = INT_MAX;
    for (unsigned itp = 0; // need output for each time point
       itp < (sizeof(tm_pt)/sizeof(tm_pt[0])); ++itp) {
         for ( ; svc.tmrow.end() != itt; ++itt)
           if (itt->first <= tm_pt[itp])
             objVal = itt->second;
           else break;
           __stat << "%g" << objVal;
    }
//    if (fWriteMILP)
  //    Create2DSPPModel(svc, fln, inst, svc.L_best);
  if (fWriteSolution) {
    ofstream ofs("solution.txt");
    if (ofs)
      svc.WriteSolution(ofs);
  }
#ifdef __LPBOUNDS
    __2DSPP_1DLP lp(svc, svc.L_best);
    lp.Solve();
    __stat << "%.1f" << lp.tmh << lp.tmv
      << "%.1f" << lp.cg
      << "%g" << lp.lbh << lp.lbv << FMax(lp.lbh, lp.lbv);
#endif
#ifdef __BEASLEY
    Beasley2DLP bs(svc.W,svc.w,svc.l,lb0,svc.L_best);
    bs.Solve2DSPP();
    __stat << bs.lbRes << bs.tm;
#endif
    __stat.print_diff(logfile, ' ');
    __stat.copy_diff_into(__stat_all);
//    __stat_prm.copy_diff_into(__stat_prm_all); NO, done for each input
    logfile << sumObj / nInst;
    logfile << '\n';
    if (0 == inst % int(statPeriod)) {
      PrintStat(fln);
      __stat.Reset();
    }
    log__(endl);
  }
  return 1;
}

#define __STAT_FLN "!!stat.txt"
#define __PRM_FLN "!!prm_grp.txt"

void Solver::PrintStat(const char * fln) {
   {
      ofstream statfile(__STAT_FLN, ios::app);
      if (!statfile) return;
      if (nInst <= statPeriod) { // for the first subgroup
        statfile << '\n' << '\n' << fln << ' ' << GetDateAndTime() << '\n';
        param::WriteParamsShort(statfile);
        statfile << '\n';
        statfile << OUT_LINE << '\n';
      }
      __stat.print_ave(statfile, ' ');
      statfile << '\n';
   }
/*   if (fWriteGrpPrm) {
      ofstream statfile(__PRM_FLN, ios::app);
      if (!statfile) return;
      if (nInst <= statPeriod) { // for the first subgroup
        statfile << '\n' << '\n' << fln << ' ' << GetDateAndTime() << '\n';
        param::WriteParamsShort(statfile);
        statfile << '\n';
        statfile << "STAT: AVE & DEV for: " << SVC_2DSPP::StatStr << ";;; See " __STAT_FLN " for overall.";
        statfile << '\n';
      }
      __stat_prm.print_ave(statfile, ' ');
      __stat_prm.print_stddev(statfile, ' ');
      statfile << '\n';
   } */
}

void Solver::PrintStatAll() {
      ofstream statfile(__STAT_FLN, ios::app);
      if (!statfile) return;
      statfile << "------------------ ave & max --------------------\n";
      __stat_all.print_ave(statfile, ' ');
      statfile << '\n';
      __stat_all.print_max(statfile, ' ');
      statfile << '\n';
/*      statfile << "------------------ param values: ave & stddev --------------------\n";
      __stat_prm_all.print_ave(statfile, ' ');
      statfile << '\n';
      __stat_prm_all.print_stddev(statfile, ' ');
      statfile << '\n'; */
}

} // namespace bg

