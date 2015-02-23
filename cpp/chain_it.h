#include <functional>
#include <iterator>
#include <utility>

namespace chainit {

  using std::iterator;
  using std::pair;
  using std::bidirectional_iterator_tag;
  using std::random_access_iterator_tag;

  template <class V>
  struct Read {typedef V Deref;}; 
  
  template <class V>
  struct Write {typedef V& Deref;}; 

  template <class TT>
   struct DECAY {
    typedef TT T;
  };

  template <class TT>
   struct DECAY<TT*> {
    typedef const TT* T;
  };

  template <class TT, std::size_t N>
   struct DECAY<TT[N]> {
    typedef const TT* T;
  }; 

 #define DK(TT)    typename DECAY<TT>::T
 #define ITCAT(TT) typename std::iterator_traits<TT>::iterator_category
 #define VTYPE(TT) typename std::iterator_traits<TT>::value_type

 #define DEFINE_OP(FUNC, OPRN, T2, T1)   template<class T2, class T1> \
   inline typename TDeduce<OPRN < VTYPE(DK(T2))>,                     \
                           DK(T2),                                    \
                           DK(T1),                                    \
                           ITCAT(DK(T2)),                             \
                           ITCAT(DK(T1))>::T                          \
   FUNC(const T2& a_beg, const T1& b_beg) {                           \
    typedef VTYPE(DK(T2)) V;                                          \
    return vec_op_iter<OPRN<V> >(                                     \
      static_cast<DK(T2)>(a_beg), static_cast<DK(T1)>(b_beg)          \
                                      );                              \
  }


  template <class DRVD>
   struct IterMvAndCmp {

     inline DRVD& operator++ () {
        ++(static_cast<DRVD&>(*this).get_itr());
        return static_cast<DRVD&>(*this);
    }
     
     inline DRVD operator++ (int) {
       DRVD tmp(static_cast<DRVD&>(*this));
        ++(static_cast<DRVD&>(*this).get_itr());
        return tmp;
    }

    inline DRVD& operator-- () {
        --(static_cast<DRVD&>(*this).get_itr());
        return  static_cast<DRVD&>(*this);
    }

   inline  DRVD&
      operator+= (size_t offset) {
         advance(static_cast<DRVD&>(*this).get_itr() , offset);
         return static_cast<DRVD&>(*this);
   }

   inline DRVD&
      operator-= (size_t offset) {
         advance(static_cast<DRVD&>(*this).get_itr(), -offset);
         return static_cast<DRVD&>(*this);
   }

   inline DRVD
      operator+ (size_t offset) const {
          DRVD tmp(static_cast<const DRVD&>(*this));
          advance(tmp.get_itr(), offset);
          return DRVD(tmp);
   }

   inline DRVD
      operator- (size_t offset) const {
          DRVD tmp(static_cast<const DRVD&>(*this));
          advance(tmp.get_itr(), -offset);
          return DRVD(tmp);
   }

   inline size_t
      operator- (const DRVD& rhs) const {
          return distance(rhs.get_itr(),
                 static_cast<const DRVD&>(*this).get_itr());
  }

   inline bool
      operator== (const DRVD& rhs) const {
        return ( static_cast<const DRVD&>(*this).get_st() == rhs.get_st());
  }

  inline bool
      operator!= (const DRVD& rhs) const {
        return ! (*this == rhs);
  }

  inline bool
    operator < (const DRVD& rhs) const {  // for random_access
                                           // bgn only
      return ( static_cast<const DRVD&>(*this).get_st() < rhs.get_st());
  }

 inline bool
    operator <= (const DRVD& rhs) const { // for random_access
                                          // bgn only
      return ( static_cast<const DRVD&>(*this).get_st() <= rhs.get_st());
  }

  inline bool
    operator >= (const DRVD& rhs) const { // for random_access
                                          // bgn only
      return (static_cast<const DRVD&>(*this).get_st() >= rhs.get_st());
  }

  inline bool
    operator > (const DRVD& rhs) const {  // for random_access
                                           // bgn only
      return (static_cast<const DRVD&>(*this).get_st() >  rhs.get_st());
  }
};


  // Read Write Iterator
  template <class T, class IT1, class IT2>
   struct indirector_t : public iterator<ITCAT(IT1), VTYPE(IT1)>,
                                IterMvAndCmp<indirector_t<T, IT1, IT2> > {
    typedef VTYPE(IT1) V;
    IT1 base;
    IT2 cursor;

    explicit indirector_t(const IT1& bs, const IT2 cr):
      base(bs), cursor(cr){}

    typedef pair<IT1, IT2> STATE;


    inline typename T::Deref operator *() { return *(base + *cursor); }

    inline V operator *() const {
         return *(base + *cursor); }

   private:
      friend IterMvAndCmp<indirector_t<T, IT1, IT2> >;

    inline STATE get_st() const { return std::make_pair(base, cursor); }
    inline IT2&  get_itr() {return cursor; }
    inline const IT2&  get_itr() const {return cursor; }

 };

template <class T, class IT1, class IT2>
  inline indirector_t<T, IT1, IT2> indirector(const IT1& bs, const IT2 cr) {
    return indirector_t<T, IT1, IT2>(bs, cr);
  }


  // Read Only Iterator
  template<class V, class IT1>
   struct mapiterator_t : public iterator<ITCAT(IT1), VTYPE(IT1) >,
                          IterMvAndCmp<mapiterator_t<V, IT1> >
                                                               {
     IT1 bgn;
     std::unary_function<VTYPE(IT1), V> func;     // keep pure

    explicit mapiterator_t(const IT1& bg,
                           const std::unary_function<VTYPE(IT1), V>&f):
                           bgn(bg), func(f) {}
    
    typedef IT1 STATE;


     inline V operator *() const { return func(*bgn); }

    private:
     friend IterMvAndCmp<mapiterator_t<V, IT1> >;
    inline STATE get_st() const { return bgn; }
    inline IT1&  get_itr() {return bgn; }

  };

  template<class V, class IT1>
  inline mapiterator_t<V, IT1> scaler(V scalar, IT1 it) { 
    return mapiterator_t<V, IT1>(it, std::bind1st(std::multiplies<V>(),scalar) );
  }


  // Read Only Iterator
  template<class V, class IT1, class ST = V>
   struct mealyterator_t : public iterator<ITCAT(IT1), VTYPE(IT1) >,
                          IterMvAndCmp<mapiterator_t<V, IT1> >
                                                               {
     IT1 bgn;
     ST state_;
     std::binary_function<VTYPE(IT1), ST&, V> trans;

    explicit mealyterator_t(const IT1& bg, 
                           const std::binary_function<VTYPE(IT1), ST&, V>&f,
                           ST st = ST() ):
                           bgn(bg), trans(f), state_(st) {}
    
     inline V operator *() const { return func(*bgn, state_); }

    private:
     friend IterMvAndCmp<mapiterator_t<V, IT1> >;
    inline IT1 get_st() const { return bgn; }
    inline IT1&  get_itr() {return bgn; }          // make private, add friend
  };

  
  // Read Only Iterator
  template <class OP, class IT1, class IT2, class T>
   struct vec_op_t  {
    typedef const VTYPE(IT1) V;
    IT1 arg1_beg;
    IT2 arg2_beg;

    explicit vec_op_t(const IT1& a_beg, const IT2& b_beg):
      arg1_beg(a_beg), arg2_beg(b_beg){}

    // TODO add post increment. maybe not
    inline T& operator++ () {
        ++arg1_beg; ++arg2_beg;
        return  static_cast<T&>(*this);
    }

    inline T& operator-- () {
        --arg1_beg; --arg2_beg;
        return  static_cast<T&>(*this);
    }

    // This is meant to be read-only iterator 
    // therefore it does not return a reference
    inline const V operator* () const {return OP()(*arg1_beg, *arg2_beg);}

    template <class O1>
    inline const bool operator== (const vec_op_t<O1, IT1, IT2, T>& rhs) const  {
      return (rhs.arg1_beg == arg1_beg) && (rhs.arg2_beg == arg2_beg);
    }

    template <class O1>
    inline const bool operator!= (const vec_op_t<O1, IT1, IT2, T>&rhs) const {
     return!(*this==rhs);}
  };


  template <class OP1, class T1, class T2>
   struct vec_op_bidirect_t : public iterator<bidirectional_iterator_tag,
                       VTYPE(T1)>, public vec_op_t< OP1, T1, T2,
                       vec_op_bidirect_t<OP1, T1, T2> >
                       {

   explicit vec_op_bidirect_t(const T1& a_beg, const T2& b_beg)
            :vec_op_t<OP1, T1, T2, vec_op_bidirect_t<OP1, T1, T2> >(
            a_beg, b_beg){}
 
   /*explicit vec_op_bidirect_t(const vec_op_t<OP1, T1, T2,
                             vec_op_bidirect_t<OP1,T1, T2> >& v):
      vec_op_t<OP1, T1, T2, vec_op_bidirect_t<OP1, T1, T2> >(v){}*/

  };

  template <class OP1, class T1, class T2>
   struct vec_op_random_access_t : public iterator<random_access_iterator_tag,
                       VTYPE(T1)>, public vec_op_t< OP1, T1, T2,
                       vec_op_random_access_t<OP1, T1, T2> > {

     explicit vec_op_random_access_t(const T1& a_beg, const T2& b_beg):
     vec_op_t<OP1, T1, T2, vec_op_random_access_t<OP1, T1, T2> > (a_beg, b_beg){}

     /*explicit vec_op_random_access_t(const vec_op_t<OP1, T1, T2,
                             vec_op_random_access_t<OP1,T1, T2> >& v):
      vec_op_t<OP1, T1, T2, vec_op_random_access_t<OP1, T1, T2> >(v) {}*/

      inline vec_op_random_access_t<OP1, T1, T2>&
      operator+= (size_t offset) {
          this->arg1_beg += offset;
          this->arg2_beg += offset;
          return (*this);
      }

      inline vec_op_random_access_t<OP1, T1, T2>
      operator+ (size_t offset) {
          return vec_op_random_access_t<OP1, T1, T2>(
                 (this->arg1_beg + offset) , (this->arg2_beg + offset) ) ;
      }

      inline vec_op_random_access_t<OP1, T1, T2>&
      operator-= (size_t offset) {
          this->arg1_beg -= offset;
          this->arg2_beg -= offset;
          return (*this);
      }

      inline vec_op_random_access_t<OP1, T1, T2>
      operator- (size_t offset) {
          return vec_op_random_access_t<OP1, T1, T2>(
                 (this->arg1_beg - offset) , (this->arg2_beg - offset) ) ;
      }

      template <class O1>
      inline size_t
      operator- (const vec_op_random_access_t<O1, T1, T2>& rhs) {
          return this->arg1_beg - rhs.arg1_beg;
      }

      template <class O1>
      inline const bool operator <
      (const vec_op_random_access_t<O1, T1, T2>& rhs) const {
        return (this->arg1_beg < rhs.arg1_beg);
      }

      template <class O1>
      inline const bool operator > 
      (const vec_op_random_access_t<O1, T1, T2>& rhs) const {
       return (this->arg1_beg > rhs.arg1_beg);
      }

      template <class O1>
      inline const bool operator <= 
      (const vec_op_random_access_t<O1, T1, T2>& rhs) const {
       return (this->arg1_beg <= rhs.arg1_beg);
      }

      template <class O1>
      inline const bool operator >=
      (const vec_op_random_access_t<O1, T1, T2>& rhs) const {
       return (this->arg1_beg >= rhs.arg1_beg);
      }
  };

  template <class OP, class T1, class T2, class IT1, class IT2>
   struct TDeduce {
    typedef vec_op_bidirect_t<OP, T1, T2> T;
  };

  template <class OP, class T1, class T2>
   struct TDeduce<OP, T1, T2 ,
           random_access_iterator_tag, random_access_iterator_tag> {
    typedef vec_op_random_access_t<OP, T1, T2> T;
  };


  template <class OP, class T1, class T2>
   struct TDeduce<OP, T1*, T2*,
           random_access_iterator_tag, random_access_iterator_tag> {
    typedef vec_op_random_access_t<OP, const T1*, const T2*> T;
  };


  template<class OP, class CAT2, class ITER1, class ITER2>
  struct TDUCE2ND { typedef vec_op_bidirect_t<OP, ITER1, ITER2> T; };
  
  template<class OP, class ITER1, class ITER2 >
  struct TDUCE2ND <OP, random_access_iterator_tag, ITER1, ITER2> { 
                     typedef vec_op_random_access_t<OP, ITER1, ITER2> T; 
  };

  template<class OP, class CAT1, class CAT2, class ITER1, class ITER2 >
  struct TDUCE { typedef vec_op_bidirect_t<OP, ITER1, ITER2> T; };

  template<class OP, class CAT2, class ITER1, class ITER2 >
  struct TDUCE <OP, random_access_iterator_tag,
                   CAT2, ITER1, ITER2 > { 
         typedef typename TDUCE2ND <OP, ITCAT(ITER2), ITER1, ITER2 >::T T; 
  };

  // when CAT_TAG is not rand_access_tag
  template<class OP, class CAT_TAG, class ITER1, class ITER2>
    inline vec_op_bidirect_t<OP, ITER1, ITER2>
    vecop_iter_deduce_frm_2nd(const ITER1& a_beg,
                              const ITER2& b_beg,
                              const CAT_TAG& tag) {
                return vec_op_bidirect_t<OP, ITER1, ITER2>(a_beg, b_beg);
  }
  
  // specialized for rand_access_tag
  template<class OP, class ITER1, class ITER2>
    inline vec_op_random_access_t<OP, ITER1, ITER2>
    vecop_iter_deduce_frm_2nd(const ITER1& a_beg,
                              const ITER2& b_beg,
                              const random_access_iterator_tag& tag) {
      return vec_op_random_access_t<OP, ITER1, ITER2>(a_beg, b_beg);
    }

  // when CAT_TAG is not rand_access_tag
  template<class OP, class CAT_TAG, class ITER1, class ITER2>
    inline vec_op_bidirect_t<OP, ITER1, ITER2>
    vecop_iter_deduce_frm_1st(const ITER1& a_beg,
                              const ITER2& b_beg,
                              const CAT_TAG& tag) {
      return vec_op_bidirect_t<OP, ITER1, ITER2>(a_beg, b_beg);
    }

  // specialized for rand_access_tag
  template<class OP, class ITER1, class ITER2>
    inline typename TDUCE2ND <OP, ITCAT(ITER2), ITER1, ITER2 >::T
    vecop_iter_deduce_frm_1st(const ITER1& a_beg,
                              const ITER2& b_beg,
                              const random_access_iterator_tag & tag) {
      return vecop_iter_deduce_frm_2nd<OP> (a_beg, b_beg, ITCAT(ITER2)() );
    }

  template<class OP, class ITER1, class ITER2>
    inline typename TDUCE<OP, ITCAT(ITER1), ITCAT(ITER2), ITER1, ITER2>::T 
    vec_op_iter(const ITER1& a_beg, const ITER2& b_beg) {
     return vecop_iter_deduce_frm_1st<OP> (a_beg, b_beg, ITCAT(ITER1)() );
   }

   template<class T2, class T1>
   inline typename TDeduce<std::minus<VTYPE(DK(T2))>,
                           DK(T2),
                           DK(T1),
                           ITCAT(DK(T2)),
                           ITCAT(DK(T1))>::T
   vminus(const T2& a_beg, const T1& b_beg) {
    typedef VTYPE(DK(T2)) V;
    return vec_op_iter<std::minus<V> >(
      static_cast<DK(T2)>(a_beg), static_cast<DK(T1)>(b_beg)
                                      );
  }

  DEFINE_OP(vplus,  std::plus,  T2, T1)
  DEFINE_OP(vmult,  std::multiplies, T2, T1)

} 

/*
using namespace chainit;
#include <iostream> 
#include <list>
#include <vector>
using std::list;
using std::vector;
int main() {
    
   float a[2] = {1,2}, b[2] = {1,2};
//   typedef float* T2;
//   typedef float* T1;
 
   float c; 
   std::vector<float> al, bl;
//   typedef typename std::list<float>::iterator T2;
//   typedef typename std::list<float>::iterator T1;
//   std::list<float>::iterator a = al.begin();
//   std::list<float>::iterator b = bl.begin();
//   typename TDeduce<std::minus<float>,
//                    DK(T2), 
//                    DK(T1), 
//                    ITCAT(DK(T2)),
//                    ITCAT(DK(T1))>::T
//   c = *vminus(static_cast<float*>(a), static_cast<float*>(b));
//   c = *chainit::vminus(al.begin(), bl.begin());
   c = *chainit::vminus(a, b);
   std::cout << c;
   } */ 
