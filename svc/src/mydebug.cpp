#include "firsthdr.h"
#include "mydebug.h"
//#include "lasthdr.h"

int signal_error__=1;

void error_handler__() {
  if (signal_error__)
    throw logic_error("Runtime error.");
}

namespace bg {

//ofstream dbg_os__("dbg_out.txt");

//int FunctionEnterer::rdep=0; // Function stack depth
int dbg_level__=5; // 0: Release; 5: All possible output
double fne_level__=5; // 0: Release; 5: All possible fns.
  // The functions which call none are at level 5.
double sp_fct____=1; // To contract indentation
const char * __glb_file="no file set.";
int __glb_inumber=0;

double outputLevel=2;
double __GlobalOutputLevel() { return outputLevel; }

} // namespace bg
