#include "firsthdr.h"

//#include "mystat.h"

//char __glb_file[1024];
//int __glb_inumber;

namespace bg { //  BEGIN_COMMON_NAMESPACE__

/// MATHEMATICS

//template <class Random>
double rndNormal (double m, double s,
    Random& rnd01 // reference
) {
  double res = 0;
  for (int i=0;i<12;++i)
    res += (double)rnd01;
//  log__(" m,s:"<<m<<' '<<s<<" rN:"<<res<<' ');
  res -= 6;
  res *= s;
  res += m;
  return res;
}


int nErr2__=0, maxnErr2__=7; // max non-critical errors reported

void error2_handler__() {
  ++ nErr2__;
//  if (signal_error__)
//    throw logic_error("Some error.");
}


//ofstream *pmylog__ = &thelog__;
//ostream & mylog = thelog__;
//ofstream & GetMyLog__()
// { return *(COMMON_NAMESPACE__::pmylog__); }

MyApp myApp;

MyApp::MyApp() {
//  opt::ReadOptions(); // done in the algorithm
  InitHandlers();
  Tests();
} //____________________________________________________
MyApp::~MyApp() {
//  opt::WriteOptions();// could be changed when executing
} //____________________________________________________
void MyApp::Tests() {
  double v=0;
  double v1=v;
  assertm(not NOT_ZERO(v1),
    "0!=0 !!! Modify NOT_ZERO macro.");
} //____________________________________________________

bool fShouldExit=false; // Can be modified by the appl.

volatile bool fOutputTimer=0;
volatile long nTimerTicks=0;

#ifndef WIN32
void AlarmHandler(int sig) {
  fOutputTimer=1;
  ++ nTimerTicks;
  signal (sig, AlarmHandler);
}
double  Alarm (double seconds)
     {
       struct itimerval old, new_;
//       new.it_interval.tv_sec = 0;
       new_.it_value.tv_usec = (long int)((seconds-floor(seconds))*(1000000.0));
       new_.it_value.tv_sec = (long int) seconds;
       new_.it_interval = new_.it_value;
       if (setitimer (ITIMER_VIRTUAL, &new_, &old) < 0)
         return -1.34;
       else
         return old.it_value.tv_sec;
// not here       AlarmHandler();
     }
#endif



void SigTermHandler(int signum) {
  throw UserBreak__();
}
void SigErrHandler(int signum) {
  const char *s=0;
  switch (signum) {
  case SIGFPE: s="Floating point exception";
    break;
  case SIGSEGV: s="Segment violation";
    break;
  case SIGILL: s="Illegal instruction";
    break;
  }
  throw runtime_error(s);
} //____________________________________________________
void MyApp::InitHandlers() {
//#ifndef DBG_ON
  signal(SIGTERM,SigTermHandler);
  signal(SIGINT,SigTermHandler);
#ifdef WIN32
  signal(SIGBREAK,SigTermHandler); // Ctrl-C
#else // UNIX
  signal(SIGHUP,SigTermHandler);
  signal(SIGQUIT,SigTermHandler);
#endif
  signal(SIGFPE,SigErrHandler);
  signal(SIGSEGV,SigErrHandler);
  signal(SIGILL,SigErrHandler);
//#endif
} //____________________________________________________



/*
int mystat::Accumulator::ww=20;
void mystat::Accumulator::Do(int wh,ostream &os) {
  switch (wh) {
  case 0: reset();
    break;
  case 1:
    if (flags|bPrintAbs) {
      if (flags|bPrintTotal)
        PrintVar(os,ww,nTimes);
      PrintVar(os,ww,accu);
    }
    if (flags|bPrintRatio)
      PrintVar(os,ww,Del0(accu,nTimes));
    break;
  case 2:
    char buf[1024];
    strncpy(buf,name,1020);
    int L=strlen(buf);
    if (flags|bPrintAbs) {
      if (flags|bPrintTotal) {
        strcpy(buf+L,"_N");
        PrintVar(os,ww,buf);
      }
      strcpy(buf+L,"_A");
      PrintVar(os,ww,buf);
    }
    if (flags|bPrintRatio) {
      strcpy(buf+L,"_R");
      PrintVar(os,ww,buf);
    }
    break;
  }
}
*/

//ofstream thelog__("log.txt");
//ofstream
//int floatWidth = 10;

} // END_COMMON_NAMESPACE__

// int FUCK;
//____________________________________________________

