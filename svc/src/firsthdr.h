#ifdef  __FIRSTHDR_H
#error should be included only once
#endif
#define __FIRSTHDR_H


#define COMMON_NAMESPACE__ bg
#define BEGIN_COMMON_NAMESPACE__ namespace bg {
#define END_COMMON_NAMESPACE__   }


BEGIN_COMMON_NAMESPACE__
//using namespace std;
END_COMMON_NAMESPACE__

#include "stdafx.h"

//#define DBG_ON


//#include "mydebug.h"
#include "mytools.h"
#include "myparam.h"
//#include "mystat.h"


//#include "spxdefines.h"
#include "timer.h"
#include "random.h"
BEGIN_COMMON_NAMESPACE__
typedef soplex::Timer Timer;
typedef soplex::Random Random;
END_COMMON_NAMESPACE__


//#include "sshdrs.h"
//#include "alg.h"
#include "mymath.h"


