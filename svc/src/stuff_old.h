
/*template <class D>
void ComputeWMinAfter(const vec<D> &w, vec<D> &wma) {
  wma.clear();
  wma.resize(w.size());
  D WMA = w.back();
  for (int i=w.size()-2;--i;i>=0) {
    if (w[i] < WMA) WMA = w[i];
    wma[i] = WMA;
  }
}*/
