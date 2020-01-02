#ifndef __BGN__MYDEBUG_H
#define __BGN__MYDEBUG_H

void error_handler__();

BEGIN_COMMON_NAMESPACE__

#ifdef dbg_assert
  #undef dbg_assert
  #undef dbg_assertm
#endif

#ifndef NDEBUG //DBG_ON
  #define dbg_assert assert
  #define dbg_assertm assertm
  #define DEBUG__(e) { e }
#else
  #define dbg_assert(e)
  #define dbg_assertm(e,t)
  #define DEBUG__(e)
#endif

#ifdef assert
#undef assert
#endif
#ifdef assertm
#undef assertm
#endif

//#define ASSERT_STR(s)
// "Error. " s  " :" __FILE__ ":" /*__LINE__ ":" #__DATE__*/
#define assert(x) \
{ if (!(x)) \
  its_error("Assertion failed: " #x " :" __FILE__ " :", __LINE__, \
  __DATE__); }
#define assertm(x,emsg) \
{ if (!(x)) its_error(emsg,"",""); }

#define its_error(msg,m2,m3) \
 { PRINT_ERROR(msg<<m2<<':'<<m3<<flush); error_handler__(); }


#define OUTP_LEV__ \
  FMin(bg::outputLevel,bg::__GlobalOutputLevel())
#define DEF_OUTP_LEVEL 4
extern double outputLevel; // in a class, an internal variable can be used
double __GlobalOutputLevel();

#if (defined(DBG_OUTPUT) || !defined(NDEBUG))
#define dbg_out(str) \
{ mylog << str << endl; }  // Redirection ?
//    setw(2/*int(FunctionEnterer::rdep/sp_fct____)*/) << " "
#define dbg_out_(str) \
{ mylog << str << flush; }  // Redirection ?
#define dbg_outn(level,str) \
{ if (OUTP_LEV__>=level) dbg_out(str); } // no else after ?
#define dbg_outn_(level,str) \
{ if (OUTP_LEV__>=level) dbg_out_(str); }
#define ENTER_FN(fn,dep) bg::FunctionEnterer fefefefe(fn,dep)
#else
#define dbg_out(str)
#define dbg_out_(str)
#define dbg_outn(level,str)
#define dbg_outn_(level,str)
#define ENTER_FN(fn,dep)
#endif

#define BEGIN_METHOD(typ,cls,Name) typ cls::Name() {
  // + ENTER_FN ...
#define END_METHOD }

#define LOGSTREAM__ cout //can be redefined


#define PRINT_LOG__(e) { LOGSTREAM__ << e << flush; }
#define PRINT_LOGLN(e) PRINT_LOG__(e << '\n')
#define PRINT_LOG(e) PRINT_LOGLN(e)



//#ifndef NDEBUG //DBG_ON
//  #define PRINT_ERROR(str)
//{ dbg_out( str ); PRINT_LOGLN(str); }
//#else
  #define PRINT_ERROR(str) { PRINT_LOGLN(str); \
    ofstream("errata.txt",ios::out|ios::app) << \
    "DateTime, InputFile, InstNum: " << bg::GetDateAndTime() \
    << bg::__glb_file << ' ' << bg::__glb_inumber << ": " << str << \
    '\n'; }
//#endif


//extern ofstream dbg_os__;
extern int dbg_level__;
extern double fne_level__;
extern double sp_fct____;
extern const char * __glb_file; // can be defined
extern int __glb_inumber;

/*
struct FunctionEnterer {
  char *nm;
  double dep;
  static int rdep; //Function stack depth. Single process!
  FunctionEnterer(char*fn, double dp) :nm(fn), dep(dp) {
    if (fne_level__ >= dp)
      dbg_out("--> Fn="<<fn<<" depth="<<dp);
    ++rdep;
  }
  ~FunctionEnterer() {
    --rdep;
    if (fne_level__ >= dep)
      dbg_out("<-- Fn="<<nm<<" depth="<<dep);
  }
}; // --------------------- 07.08.01 -------------------
*/

END_COMMON_NAMESPACE__
#endif // __BGN__MYDEBUG_H

