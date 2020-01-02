#ifndef __MYPARAM_H
#define __MYPARAM_H

namespace bg {

namespace param {

// USER should read params himself
// and can vary the filename, especially after c.l. options

#ifndef __CFG_FILE
#define __CFG_FILE "params.cfg"
#endif

#define DeclClassParam(typ) static bg::param::Param<typ> //<typ>
#define ClassParam(typ, sec, name, defval, comment) \
  bg::param::Param<typ> sec::name(#sec, #name, defval, \
    comment) // no ";"

#define DeclClassDblParam DeclClassParam(double)
#define DeclClassIntParam DeclClassParam(int)
#define DeclClassVectorParam DeclClassParam(vec<double>)
#define DeclClassMatrixParam DeclClassParam(vec<vec<double> >)

#define ClassDblParam(sec, name, defval, comment) \
  ClassParam(double, sec, name, defval, comment)
//#define ClassDblParam(sec, name, defval) ClassParam(sec, name, defval, 0)
#define ClassIntParam(sec, name, defval, comment) \
  ClassParam(int, sec, name, defval, comment)
//#define ClassIntParam(sec, name, defval)  ClassParam(sec, name, defval, 0)
#define ClassVectorParam(sec, name, defval, comment) \
  ClassParam(vec<double>, sec, name, \
  vec<double>(&defval[0], &defval[0]+sizeof(defval)/sizeof(defval[0])), comment)
#define ClassMatrixParam(sec, name, defval, comment) \
  ClassParam(vec<vec<double> >, sec, name, \
  bg::param::VecVecInit(defval[0], sizeof(defval), sizeof(defval[0])), comment)

template <class D>
vec<vec<D> > VecVecInit(D * pD, int sAll, int sLine) {
  unsigned s1 = sAll/sLine;
  unsigned s2 = sLine/sizeof(D);
  vec<vec<D> > matr(s1, vec<D>(s2));
  for (size_t i=0;i<s1;++i)
    for (size_t j=0;j<s2;++j)
      matr[i][j] = *(pD + i*s2 + j);
  return matr;
}

// LETTER CASE-INDIFFERENT !!

class VirtParam { // no different types => not virtual
public:
    const char *section, *name;
    const char *comment;
    VirtParam(const char *sec, const char*n,
      const char *comm=0, const bool fInsert=1);
    virtual ~VirtParam() {}
    virtual void read(istream &is) =0;
    virtual void write(ostream &os) =0;
    virtual void SetDefault() =0;
    bool operator < (const VirtParam & p) const {
      if (0>strucmp(section, p.section)) return true;
      if (0<strucmp(section, p.section)) return false;
      if (0>strucmp(name, p.name)) return true;
      return false;
    }
}; // class VirtParam

template <class D>
class Param : public VirtParam {
public:
    D val;
    const D defval;
    Param(const char *sec, const char*n,
     const D defv=0, const char *comm=0, const bool fInsert=1)
       : VirtParam(sec,n,comm,fInsert), defval(defv) { }
    void read(istream &is) { is >> val; }
    void write(ostream &os) { os << val; }
    void SetDefault() { val = defval; }
    operator D() const { return val; }
    const D& operator () () const { return val; }
//    double operator = (const double d) { val = d; return d; }
};

//template <class D>
//class Param<vec<vec<D> > >::operator[]

class ParamSection : public List<VirtParam*> {
//  ParamSection() { } // forbidden
  // not so, creating new by subscription in a map<>
public:
//  ParamSection(Param *pm) { push_back(pm); }
  const char * GetSectionName()
  { assert(size()); return front()->section; }
/*  bool operator < (const ParamSection &ps) const {
    return (-1==strucmp(front()->GetSectionName(),
      ps.front()->GetSectionName()));
  } */
}; // class ParamSection

class ParamRoot {
    void WriteParams__(bool, ostream &);
  public:
    static const char * fileName;
    typedef set<VirtParam*, CmpPtrByVal<VirtParam> > AllParams;
    typedef map<const char*, ParamSection, StringUComparator>
      Sections;
    AllParams allp; Sections secs;
    void AddParam(VirtParam *p);
    void ReadParams(), WriteParams(bool fShort=0, ostream * =0);
    void SetDefault();
    void ReadParamName(istream&,char*);
    void ReadSectionName(istream&,char*);
    void SkipSpaceAndComments(istream&);
//  double & GlobalOutputLevel();
}; // class ParamRoot

/*class zzzParamRoot {
public:
  DeclClassParam // writeParams, outputLevel,
    paramPrecision;
}; // zzzParamRoot
*/

ParamRoot * GetParamRoot();

extern bool writeParams;

inline void ReadParams() { GetParamRoot()->ReadParams(); }
inline void WriteParams() { GetParamRoot()->WriteParams(); }
inline void WriteParamsShort(ostream&os)
  { GetParamRoot()->WriteParams(1, &os); }

inline VirtParam::VirtParam(const char *sec, const char *nam,
     const char *d, const bool fInsert)
      : section(sec), name(nam), comment(d) {
      assert(sec); assert(nam);
      if (fInsert) // only creation, not recognition
        GetParamRoot()->AddParam(this);
    }

} // namespace param

} // namespace bg

#endif // __MYPARAM_H

