

class IPCol //: public LPCol
{
// the same as LPCut ? Should produce coef for a given cut?
  Vec<pair<int,int> > ix;
  bool fSlack;
  bool fInfeas;
  double obj;
public:
  IPCol() :fSlack(0), fFeas(0) { }

  int GetSize() { return ix.size(); }
  int GetIndex(int p) { return ix[p].first; } // numbering from 0
  int GetCoef(int p) { return ix[p].second; }
  void AddNZ(int i,int x)
    { assert(i>=0&&x); ix.push_back(make_pair(i,x)); }
  void Sort() { ix.sort(); }
  bool Check() const;

  template<class F>
    void AddFromDenseVector(const F * first_el, int n);
  template<class vec>
    void ProduceDenseVector(vec &) const; // clear vec before

  void SetObj(const double c) { obj = c; }
  double GetObj() const { return obj; }

  void MarkSlack() { fSlack = true; } // sets some flag. Any checking?
  bool IfSlack() const;
  int GetSlackCoef() const;

  void MarkInfeas() { fInfeas=true; }
  bool IfInfeas() const { return fInfeas; }

  // hidden? index? addi_info? VecProd? Intensity?

  /// distinguishes slacks but not infeas:
  virtual bool operator<(const IPCol& ) const;
  // Pseudo-costs here or in variables?
};

  inline bool IPCol::IfSlack() const {
    if (fSlack) {
      assert(GetSize()==1);
      assert(GetCoef(0)==1 or GetCoef(0)==-1);
      return true;
    } return false;
  }

  inline int IPCol::GetSlackCoef() const {
    assert(IfSlack()); // GetSize()==1 and GetIntCoef() = +-1 and ... ?
    return GetCoef(0);
  }

  inline bool IPCol::Check() const {
    if (not GetSize()) return 0;
    for (int p=1;p<GetSize();++p)
      if (GetIndex(p-1) >= GetIndex(p)) // strictly
        return 0;
    IfSlack();
    return 1;
  }

  inline bool IPCol::operator<(const IPCol& c) {
    assert(Check() && c.Check()); // !!!!!!! If Check not too expensive
      // sorting is checked there, needed for comparison!
    if (IfSlack() != c.IfSlack())
      return IfSlack() < c.IfSlack();
    if (IfSlack())
      return GetSlackCoef() < c.GetSlackCoef();
    return lexicographical_compare(ix,c.ix);
  }

  template<class F> void
      IPCol::AddFromDenseVector(const F * first_el, int n) {
        assert(not GetSize());
        for (int i=0;i<n;++n)
        if (first_el[i])
          AddNZ(i,first_el[i]);
        assert(GetSize());
      }

  template<class vec> void
      LPCol::ProduceDenseVector(vec &v) const {
        assert(GetSize());
        if (v.size() < GetIndex(GetSize()-1))
          v.resize(GetIndex(GetSize()-1) + 1);
        for (int p=0;p<GetSize();++p)
          v[GetIndex(p)] = GetCoef(p);
      }

/*
class LPCol; // no virtual! not mixing in 1 app
class IPCol;

class LPCol { // the same as LPCut ? Should produce coef for a given cut?
public:
//  virtual ~LPcol();
  virtual int GetSize() const =0;
  virtual bool IfIntCoefs() const { return true; }
  virtual int GetIndex(int p) =0;
  virtual int GetCoef(int p) =0; // INT !! numbering p from 0
  virtual double GetDblCoef(int p) { return GetIntCoef(p); }
  virtual void AddNZ(int i,int x) =0;
  virtual void AddDblNZ(int i,double d) { AddIntNZ(i,(int)d); }
  virtual void Sort() =0; // const?
  virtual bool Check() const =0; // checks esp. sorting

  template<class F>
    virtual void AddFromDenseVector(const F * first_el, int n);
  template<class vec>
    virtual void ProduceDenseVector(vec &) const; // clear vec before

  virtual void SetObj(const double c) =0;
  virtual double GetObj() const =0;

  virtual void MarkSlack() { assert(0); } // sets some flag. Any checking?
  virtual bool IfSlack() { assert(0); }
  virtual int GetSlackCoef() const;

  virtual void MarkInfeas() { assert(0); }
  virtual bool IfInfeas() const { assert(0); }

  // hidden? index? addi_info? VecProd? Multiplicity?

  /// For integer coefs, distinguishes slacks but not infeas.
  virtual bool operator<(const LPCol& ) const;

  // Pseudo-costs here or in variables?
};

  int LPCol::GetSlackCoef() const {
    assert(IfSlack()); // GetSize()==1 and GetIntCoef() = +-1 and ... ?
    return GetCoef(0);
  }

  bool LPCol::Check() const {
    assert(GetSize());
    for (int p=1;p<GetSize();++p)
      assert(GetIndex(p-1) < GetIndex(p)); // strictly
    IfSlack();
  }

  bool LPCol::operator<(const LPCol& c) {
    assert(Check() && c.Check()); // !!!!!!! If Check not too expensive
      // sorting is checked there, needed for comparison!
    if (IfSlack() != c.IfSlack())
      return IfSlack() < c.IfSlack();
    if (IfSlack())
      return GetSlackCoef() < c.GetSlackCoef();
    for (int p=0;p<GetSize() && p<c.GetSize();++p) {
      if (GetIndex(p)<c.GetIndex(p)) return true;
      if (GetIndex(p)<c.GetIndex(p)) return false;
      if (GetCoef(p)<c.GetCoef(p)) return true;
      if (GetCoef(p)>c.GetCoef(p)) return false;
    }
    return GetSize() < c.GetSize(); // the last criterion
  }

  template<class F>
    virtual void
      LPCol::AddFromDenseVector(const F * first_el, int n) {
        assert(not GetSize());
        for (int i=0;i<n;++n)
        if (first_el[i])
          AddNZ(i,first_el[i]);
        assert(GetSize());
      }

  template<class vec>
    virtual void
      LPCol::ProduceDenseVector(vec &v) const {
        assert(GetSize());
        if (v.size() < GetIndex(GetSize()-1))
          v.resize(GetIndex(GetSize()-1) + 1);
        for (int p=0;p<GetSize();++p)
          v[GetIndex(p)] = GetCoef(p);
      }

// A specialization which should suffice for cutting&packing:

class IPCol : public LPCol
{
// the same as LPCut ? Should produce coef for a given cut?
  Vec<pair<int,int> > ix;
  bool fSlack;
  bool fInfeas;
  double obj;
public:
  IPCol() :fSlack(0), fFeas(0) { }

  int GetSize() { return ix.size(); }
  int GetIndex(int p) { return ix[p].first; } // numbering from 0
  int GetCoef(int p) { return ix[p].second; }
  void AddNZ(int i,int x)
    { assert(i>=0&&x); ix.push_back(make_pair(i,x)); }
  void Sort() { ix.sort(); }
  bool Check() const;

  void SetObj(const double c) { obj = c; }
  double GetObj() const { return obj; }

  void MarkSlack() { fSlack = true; } // sets some flag. Any checking?
  bool IfSlack() const;

  void MarkInfeas() { fInfeas=true; }
  bool IfInfeas() const { return fInfeas; }

  // hidden? index? addi_info? VecProd?

  // Pseudo-costs here or in variables?
};

  bool IPCol::IfSlack() const {
    if (fSlack) {
      assert(GetSize()==1);
      assert(GetCoef(0)==1 or GetCoef(0)==-1);
      return true;
    } return false;
  }
*/

