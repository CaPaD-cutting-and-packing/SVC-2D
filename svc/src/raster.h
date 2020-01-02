#ifndef __RASTER_H__32
#define __RASTER_H__32


//#include <Vector>
using namespace std;


namespace bg {


void ConstructRP
(const vec<int> &sz,double SZ,vec<int> &rps);
void ConstructRP
(const vec<int> &sz,const vec<int> &ub, // with upper bounds
 double SZ,vec<int> &rps);
void ConstructReducedRP
(const vec<int> &sz,double SZ,vec<int> &rps);
void ConstructReducedRP
(const vec<int> &sz,const vec<int> &ub, // with upper bounds
 double SZ,vec<int> &rps);

// RETURNS number, not value
int FindRPUnder(int l,vec<int> &rpl);


} // namespace bg


#endif // __RASTER_H__32

