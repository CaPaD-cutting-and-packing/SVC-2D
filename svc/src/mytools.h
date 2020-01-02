#ifndef __BGN__MYTOOLS_H
#define __BGN__MYTOOLS_H

#include "mydebug.h"
//#include "myopt.h" should be later!

// #include "myautoptr.h" use simple delete for now

//typedef double Real; // for SoPlex

//extern char __glb_file[]; // not good
//extern int __glb_inumber;


namespace bg {


inline const char * GetDateAndTime() {
    time_t time0=time(0);
    return ctime(&time0);
}

//using namespace std;

template <class Iter>
inline Iter prev(const Iter it)
  { Iter ip = it; --ip; return ip; }
template <class Iter>
inline Iter next(const Iter it)
  { Iter in = it; ++in; return in; }

template<class D>
class vec : public std::vector<D> {
  public:
    typedef typename std::vector<D>::size_type size_type;
    typedef typename std::vector<D>::reference reference;
    typedef typename std::vector<D>::const_reference const_reference;
    vec() : std::vector<D>() { }
    explicit vec(size_type m)
       : std::vector<D>(m) { }
    vec(size_type m, const_reference d) : std::vector<D>(m,d) { }
    template <class I>
      vec(I b, I e) : std::vector<D>(b,e) { }
#ifndef NDEBUG
    reference operator [](size_type i)
    {
      assert(i>=0&&i<this->size());
      return std::vector<D>::operator[](i);
    }
    const_reference operator [](size_type i) const
    {
      assert(i>=0&&i<this->size());
      return std::vector<D>::operator[](i);
    }
    reference front()
    {
      assert(this->size());
      return std::vector<D>::front();
    }
    const_reference front() const
    {
      assert(this->size());
      return std::vector<D>::front();
    }
    reference back()
    {
      assert(this->size());
      return std::vector<D>::back();
    }
    const_reference back() const
    {
      assert(this->size());
      return std::vector<D>::back();
    }
#endif
}; // also with resize() ??

typedef vec<double> VecDbl;
typedef vec<int> VecInt;
typedef vec<double> vecd;
typedef vec<int> veci;

template <class E>
  ostream & operator<<(ostream&os, vec<E> &v) {
  for (size_t i=0;i<v.size();++i)
    os << v[i] << ' ';
  return os;
}
template <class E>
  istream & operator>>(istream&is, vec<E> &v) {
  for (size_t i=0;i<v.size()&&is;++i)
    is >> v[i];
  return is;
}

template <class D>
class List : public std::list<D> {
  public:
    typedef typename std::list<D>::reference reference;
    typedef typename std::list<D>::const_reference const_reference;
    List() : std::list<D>() { }
#ifndef NDEBUG
    reference front()
    {
      assert(this->size());
      return std::list<D>::front();
    }
    const_reference front() const
    {
      assert(this->size());
      return std::list<D>::front();
    }
    reference back()
    {
      assert(this->size());
      return std::list<D>::back();
    }
    const_reference back() const
    {
      assert(this->size());
      return std::list<D>::back();
    }
#endif
}; // also with resize() ??
//typedef Lst List;

/// Alternatively, for any container:
// template<cnt,d> ?????

//#define FOR(v,s,e) for(v=s;v<=e;++v)
//#define FORDN(v,s,e) for(v=s;v>=e;--v)
#define FOR_EACH_IN(cont,it,it_type) \
 for(it_type it=(cont).begin();it!=(cont).end();++(it))
#define for_each_in FOR_EACH_IN
#define for_each_r_in(cnt,rit,rit_type) \
 for(rit_type rit=(cnt).rbegin();rit!=(cnt).rend();++(rit))

#define FORless(i,v1,v2_) for ((i)=(v1);(i)<(v2_);++(i))


class VirtDestr {
public:
  virtual ~VirtDestr() { }
};


// rammelvoll

//typedef vec<double> vec_d;
//typedef vec<int> vec_i;


/*
template <class M> inline
void MemSet(M*p,int v,int n) { memset(p,v,n*sizeof(M)); }
template <class M> inline
void MemCpy(M*d,M*s,int n) { memcpy(d,s,n*sizeof(M)); }
*/
/* // THIS IS DOOF!!:
template <class D,class E>// inline
D IMax(D d,E e) { return (d>(D)e) ? d : (D)e; }
template <class D,class E>// inline
D Min(D d,E e) { return (d<(D)e) ? d : (D)e; }
template <class D,class E>// inline
void Swap(D &d,E &e) { D t=d; d=(D)e; e=(E)t; }
*/
inline int IMax(const int a,const int b) { return a>b ? a: b; }
inline long LMax(const long a,const long b) { return a>b ? a: b; }
inline double FMax(const double a,const double b) { return a>b ? a: b; }
inline int IMin(const int a,const int b) { return a<b ? a: b; }
inline long LMin(const long a,const long b) { return a<b ? a: b; }
inline double FMin(const double a,const double b) { return a<b ? a: b; }
template <class D,class E>// inline // care!!
inline D Max(D d,E e) { return (d>e)?d:(D)e; }
template <class D,class E>// inline // care!!
inline D Min(D d,E e) { return (d<e)?d:(D)e; }
template <class D>
inline void EnsureMax(D & var, const D val) { if (val > var) var = val; }
template <class D>
inline void EnsureMin(D & var, const D val) { if (val < var) var = val; }
template <class D>// inline
void Swap(D &d,D &e) { D t=d; d=e; e=t; }
template <class D,class E>// inline // care!!
void Swap2Types(D &d,E &e) { D t=d; d=(D)e; e=(E)t; }
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
template <class D,class E> //inline
D Del0(D a,E b) { return b==0 ? 0 : D(a/(D)b); }

template <class D>
void Reconstruct(D& d) {
  d.~D();
  new (&d) D();
}

//______________________________________________________
// MY STL
////////////////////////////////////////////////////////


template <class vec,class vec2>
void erase_whichnot(vec &v,vec2 &whichNot) {
  int i,j;
  assert(v.size() >= whichNot.size());
  for (i=j=0;i<whichNot.size();++i)
    if (whichNot[i]) v[j++] = v[i];
  v.erase(v.begin()+j,v.end());
}


template <class C>
void resize(C & c,int m) {
  c.resize(m); // could reconstruct
} //____________________________________________________


template <class D>
class cycle : public list<D> {
  private: int n;
  public: void push_back(const D&d) {
            if (this->size()>=n) this->pop_front();
            list<D>::push_back(d);
          }
  public: void push_front(const D&d) {
            if (this->size()>=n) this->pop_back();
            list<D>::push_front(d);
          }
          explicit cycle(int n_=1) :n(n_) { }
};
template <class V1,class V2>
inline void CopyVec(V1&d,V2&s) {
  if (d.size() < s.size()) // only then: m.b. reserving
    d.resize(s.size());
  int i; for(i=0;i<s.size();++i) d[i] = s[i];
}


template <class D>
class CmpByArray {
  const D* const d; public: CmpByArray(const D* const d_) :d(d_) { }
  bool operator()(int i1,int i2) const
  { return d[i1] < d[i2]; }
};

template <class D>
class ReverseCmpByArray {
  const D* const d; public: ReverseCmpByArray(const D* const d_) :d(d_) {}
  bool operator()(int i1,int i2) const
  { return d[i1] > d[i2]; }
};


template <class vec>
class CmpByVec {
  vec v; public: CmpByVec(vec & v_) :v(v_) { }
  bool operator()(int i1,int i2) const
  { return v[i1] < v[i2]; }
};


template <class D>
class CmpPtrByVal {
  public:
  bool operator()(const D * const p1, const D* const p2) const
  { return *p1 < *p2; }
};


template < class Iter >
class VectorPrinter {
public:
  typedef Iter iterator_type;
  Iter st, en;
  int w; char del;
  VectorPrinter(Iter i1, Iter i2, int ww, char d)
    : st(i1), en(i2), w(ww), del(d) { }
}; // VectorPrinter<Iter>
template< class Iter >
VectorPrinter<Iter>
  MakeVecPrinter(Iter i1, Iter i2, int w, char d) {
  return VectorPrinter<Iter>(i1,i2,w,d);
}
template <class Iter>
ostream & operator << (ostream & os, const VectorPrinter<Iter> & vp) {
  for (Iter it = vp.st; it != vp.en; ++it)
    os << setw(vp.w) << (*it) << vp.del;
  return os;
}

#if (defined(__GNUC__))
  #define bool int
  #define true 1
  #define false 0
#endif
#if (defined(_MSC_VER)/* || defined(__GNUC__)*/)
  #define and &&
  #define not !
  #define or ||
#endif


// IN/OUTPUT:
//ofstream & GetMyLog__();

void error2_handler__(); // non critical errors
extern int nErr2__, maxnErr2__;;

#define assert2(expr) if (!(expr)) {\
 if (nErr2__ < maxnErr2__) PRINT_ERROR("Error2: "<<#expr<< __FILE__ " :" __LINE__   __DATE__);  error2_handler__(); }

// errors in GNU when having the macros!!!
// to use in before else only in brackets

#define __asErtm(x,emsg) if (!(x)) { if (nErr2__ < maxnErr2__)  PRINT_ERROR("Error2: "<<emsg);  error2_handler__(); }
#define assert2m __asErtm
#define assertm2 __asErtm

#define PRINT__(e) PRINT_LOG__(e) // { LOGSTREAM__ << e; }
  // LOGSTREAM__ can be redefined
#define PRINTLN(e) PRINT__(e << '\n')
#define PRINT(e) PRINTLN(e)


#define log_out__ PRINT_LOG__
#define log_ln(e) PRINTLN(e)
#define log_out PRINT__


#define log__ log_out__
#define log_ log_out__

#define log_n(n,expr) { if (OUTP_LEV__ >= n) log_ln(expr); }
#define log_n_(n,expr) { if (OUTP_LEV__ >= n) log__(expr); }


template <class vec>
inline void PrintVec(ostream &os,const vec &v) {
  int i;
  for (i=0;i<v.size();++i)
    os << v[i] << ' ';
}


extern bool fShouldExit;
class UserBreak__ { };
inline bool CheckForUserBreak__() {
  if (fShouldExit)
    throw UserBreak__();
  // Maybe check if keypressed .. and some menu
  return false;
} //____________________________________________________


template <class A>
class Incrementor {
  A & a;
public:
  Incrementor(A& aa) :a(aa) { ++a; }
  ~Incrementor() { --a; }
};//____________________________________________________


template <class T>
inline auto_ptr<T> make_auto_ptr(T* pt)
{ return auto_ptr<T>(pt); }


//______________________________________________________
// MY CONTAINERS
////////////////////////////////////////////////////////
  class StringComparator {
  public:
    bool operator()
      (const char * s1, const char * s2) const
    { assert(s1 && s2); return (0 > strcmp(s1,s2)); }
  };//__________________________________________________

inline int strucmp(const char * s1, const char * s2) {
  unsigned k=0;
  while (s1[k] && s2[k]
    && toupper(s1[k])==toupper(s2[k])) ++k;
  if (toupper(s1[k])<toupper(s2[k])) return -1;
  return (toupper(s1[k])>toupper(s2[k]));
}

  class StringUComparator {
  public:
    bool operator()
      (const char * s1, const char * s2) const
    { assert(s1&&s2); return (0 > strucmp(s1,s2)); }
  };//__________________________________________________


  template <class IE, class E>
    inline  IE  Ptr2Iter (const E * pe) {
      assertm(sizeof(IE) == sizeof (E*),
        "Cannot convert pointer to iterator !!! Recode");
    /// Assuming iterator contains a shifted ptr:
    IE it;
    *(char**)(&it) = (char*)pe + (*(char**)(&it) - (char*)(&*it));
    assert(&*it == pe);
    return it;
  }

//------------------------ My I/O ------------------------------
inline void GetTillEndOfName(istream &is, char * buf, int w) {
  char c;
  int i;
  for (i=0;i<w;++i) {
    c = is.peek();
    if (!isalpha(c) && !isdigit(c) && '_' != c)
      break;
    is.get();
    buf[i] = c;
  }
  buf[i] = 0; // end of str
}

//////////////// My application ////////////////////////
class MyApp {
public:
  MyApp();
  ~MyApp();
  void Tests();
  void InitHandlers();
};//____________________________________________________


struct MyTime {
  double t;
  MyTime &operator = (const double _) { t= _; return *this; }
  operator double() const { return t; }
  MyTime(const double _=0) : t(_) { }
};
inline ostream & operator <<
  (ostream &ofs,const MyTime & m) {
/* //ios::fmtflags f=os.flags();
  char buf[20];
  ostrstream os(buf,sizeof(buf));
//os << right;
  if (m.t>3600) os << long(m.t)/3600 << 'h';
  if (m.t>60) os << int(m.t)%3600/60 << 'm';
  os << int(m.t)%60;
  if (m.t < 60) os << '.' << int((m.t-floor(m.t))*100);
  os << ends;
  //os.flags(f);


  ofs << buf;*/
  double tm = m;
  if (tm>=100) tm=ceil(tm);
  else if (tm>=10) tm= ceil(tm*10.0)/10.0;
  else tm = ceil(tm*100.0)/100.0;
  ofs << tm;
  return ofs;
}

#ifndef WIN32
double Alarm (double seconds);
void AlarmHandler(int sig = SIGVTALRM);
#endif
extern volatile bool fOutputTimer;
extern volatile long nTimerTicks;

} // namespace bg

#endif // __BGN__MYTOOLS_H

