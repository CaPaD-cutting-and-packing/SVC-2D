#include "firsthdr.h"
// #include "myparam.hpp"

namespace bg { namespace param {

const char * ParamRoot::fileName = __CFG_FILE;

ParamRoot * GetParamRoot() {
  static auto_ptr<ParamRoot> PR(new ParamRoot());
    // replace in application
  return PR.get();
}

void ParamRoot::AddParam(VirtParam *pm) {
  // check if already here; if yes, produce error
  assertm(allp.insert(pm).second, "Such a param already exists: ["
    <<pm->section<< ']'<<pm->name);
  // check if the section already
  secs[pm->section].push_back(pm);
}

void ParamRoot::ReadParams() {
  SetDefault();
  ifstream is(fileName);
  if (!is) {
    PRINT_LOGLN("Trying to create param file \""<<fileName << "\" with default values...");
    WriteParams();
    return;
  }
//  is.setf(ios::skipws,ios::skipws);
  char buf1[1024] = "",
    buf2[1024];
  while (is) {
    ReadSectionName(is,buf1); // omitting comments
    if (!is) break;
    ReadParamName(is,buf2);
    if (!is) break;
    Param<int> tmp(buf1, buf2, 0, 0, 0);
    AllParams::iterator ip = allp.find(&tmp);
    if (allp.end() != ip) {
        (*ip)->read(is);
        if (!is) {
          PRINT_ERROR(fileName<<": error reading the value of [paramsection].param ["
            <<buf1<<']'<<buf2);
          is.clear(); // IMPORTANT !!! To read other params.
          is.ignore(INT_MAX,'\n');
        }
    } else {
      PRINT_ERROR(fileName
        <<": param \""<<buf2<<"\" in section \""
        <<buf1<<"\" not found!");
      is.ignore(INT_MAX,'\n');
    }

//    is.ignore(INT_MAX,'\n'); // no line skipping!
  }
  if (!is.eof())
    PRINT_ERROR(fileName<<": could not read")
} //____________________________________________________

void ParamRoot::ReadSectionName(istream &is, char*buf) {
  SkipSpaceAndComments(is);
  if ('[' == is.peek()) {
    is.get(); // that '['
    SkipSpaceAndComments(is);
    GetTillEndOfName(is,buf,1023);
    SkipSpaceAndComments(is);
    if (']' == is.peek()) // but if not, forget
      is.get(); // that ']' also
  }
}
void ParamRoot::ReadParamName(istream &is, char*buf) {
  SkipSpaceAndComments(is);
  is.width(1023);
  is >> buf; // until space
} //____________________________________________________

void ParamRoot::SkipSpaceAndComments(istream& is) {
  for (;;) {
    char c;
    for (;;) {
      c = is.peek();
      if (isspace(c)) {
        is.get();
        continue;
      } else
        break;
    };
    if (('\''==c) or ('#'==c) or ('/'==c)) {
      is.ignore(INT_MAX,'\n');
      continue;
    } else
      break;
  };
}

void ParamRoot::SetDefault() {
  for_each_in(allp, ip, AllParams::iterator)
      (*ip)->SetDefault();
} //____________________________________________________

void ParamRoot::WriteParams(bool fShort, ostream * __os) {
  if (__os)
    WriteParams__(fShort, *__os);
  else {
    ofstream os(fileName);
    WriteParams__(fShort, os);
  }
}

void ParamRoot::WriteParams__(bool fShort, ostream & os) {
 os.precision(12);
#ifdef _WIN32
  os << left;
#endif
//  os << "'Options. Delete a line and run BCP without params to restore default\n\n";
  for_each_in(secs,iSec,
    Sections::iterator)
  if (os) {
    ParamSection::iterator ip;
    int wMax=0;
    for_each_in((iSec->second),ip,)
      wMax = IMax(wMax,strlen((*ip)->name));
    os << '[';
    if (!fShort) os.width(wMax-2);
    os << iSec->first << ']' << ' ';
    if (!fShort)
      os << '\n'; // "    // SECTION NAME\n";
    for_each_in((iSec->second),ip,)
    if (os) {
      if (!fShort) os.width(wMax);
      os << (*ip)->name << ' ';
      if (!fShort) os.width(6);
      (*ip)->write(os);
      os << ' ';
      if (!fShort && (*ip)->comment)
        os << "  // " << (*ip)->comment;
      if (!fShort) os << '\n';
      // here could be deleted.
    }
    if (!fShort) os << '\n'; // between sections.
  }
//  os.close();
  if (!os)
     PRINT_ERROR("Could not write params.")
} //____________________________________________________

//ClassParam(zzzParamRoot, writeParams, 1,
//  "whether to re-write params before execution (if used by the app.)");
//ClassParam(zzzParamRoot, outputLevel, 1,
//  "global output level (if used by the app.)");
//ClassParam(zzzParamRoot, paramPrecision, 12,
//  "precision for rewriting the param file");

}} // namespaces param, bg

using namespace COMMON_NAMESPACE__;

#ifdef _TST_MYTOOLS
double xx,xp;
static bg::opt::OptContainer MyToolsOptions2() {
  using namespace bg::opt;
  OptContainer oc;
  oc
    << MakeOpt(&xx,2.55,
    "xx","test1")
    << MakeOpt(&xp,3.28,
    "xp","test2");
  return oc;
} //____________________________________________________

static bg::opt::OptSection mytoolsopt2
  ("tst","test options section",MyToolsOptions2(),
  bg::opt::SolverCfg(), 10000);

int main() {
  try {
    COMMON_NAMESPACE__::MyApp myApp;
    for (char i=32;i<=126;i++)
      cout << i;
    cin.get();
//    cin.ignore(INT_MAX,'\n');
    return 0;
  }
  catch (const UserBreak__&)
  { PRINT_LOGLN("User break."); }
  catch (const exception & e)
  { PRINT_LOGLN(e.what()); }
  catch (...)
  { PRINT_LOGLN("Unknown exception."); }
  return 1;
}
#endif

