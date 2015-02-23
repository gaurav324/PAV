// Copyright 2012 Sreangsu Acharyya
//This file is part of libpav.

//    libpav is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    libpav is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser Public License for more details.

//    You should have received a copy of the GNU Lesser Public License
//    along with libpav in a file named COPYING.LESSER. If not,
//    see <http://www.gnu.org/licenses/lgpl.txt>.

#ifndef INCLUDED_PAV_UTILS
#define INCLUDED_PAV_UTILS

#include <functional>
#include <iterator>
#include <numeric>

namespace pav {

  template<class T, class TO, class V, class AL>
  void lbound_margin_pav(T in_strt, T in_end, TO x, V m);  // forward decl
  
  template<class T, class TO, class V, class AL>
  void ubound_margin_pav(T in_strt, T in_end, TO x, V m);  // forward decl

namespace margin_utils {
  using namespace std;
  
  enum UpdateType {OLD, OLD_PLUS_NEW};

  template <class T>
  struct scalar_sq_dist : public binary_function<T, T, T>{ 
  const T  operator() (const T& x, const T& y) const {T r = (x-y); return r*r;}
  };

  template <class T> const T dist(const T* m_old, const T* m_new, size_t size)
   __attribute__((pure));
 
  template <class T>
  const T dist(const T* m_old, const T* m_new, size_t size) {
      return  inner_product(m_old, m_old + size, m_new, static_cast<T>(0),
                        plus<T>(), scalar_sq_dist<T>() );
  }


 template <class T>
  inline T advanced(const T& iter, size_t size) {
    T ret =  iter;
    advance(ret, size);
    return ret;
  }


 // Adds the reverse_iterator functionalities that we
 // need and slightly faster dereferencing of the iterator
 template <class T>
  struct rev_iter : public iterator<
                       //bidirectional_iterator_tag,
                       typename iterator_traits<T>::iterator_category,
                       typename iterator_traits<T>::value_type
                       > {
    typedef typename iterator_traits<T>::value_type V;
    T saved_iter;
  
    // Can be dangerous as decrementing begin() is shady
    explicit 
    rev_iter(const T& iter) : saved_iter(iter) { --saved_iter; }
    
    //inline const V& operator* () const { return *saved_iter;}
    inline V& operator* () {return const_cast<V&>(*saved_iter);}
    
    inline rev_iter<T>& operator++ (){ --(this->saved_iter);  return *this; }
    inline rev_iter<T>& operator-- (){ ++(this->saved_iter);  return *this; }
    
    inline rev_iter<T>& operator+= (size_t offset){ 
      advance(this->saved_iter, -offset);  return *this; 
    }

    inline rev_iter<T>& operator-= (size_t offset){ 
      advance(this->saved_iter, offset);  return *this; 
    }

    template <class TT>
    inline bool operator==(const rev_iter<TT>& rhs) const {
      return (rhs.saved_iter == this->saved_iter); }

    template <class TT>
    inline bool operator==(const TT& rhs) const {
      TT tmp = rhs;
      return (--tmp == this->saved_iter);}

    template <class TT>
    inline bool operator!=(const TT& rhs) const { return !(*this==rhs);}

    inline T base() { return ++(this->saved_iter); }
 };


 // TODO FIXME remove duplication and handle random_access
 template <class T>
  struct const_rev_iter : public iterator<
                       //bidirectional_iterator_tag,
                       typename iterator_traits<T>::iterator_category,
                       typename iterator_traits<T>::value_type
                       > {
    typedef typename iterator_traits<T>::value_type V;
    T saved_iter;
  
    // Can be dangerous as decrementing begin() is shady
    explicit 
    const_rev_iter(const T& iter) : saved_iter(iter) { --saved_iter; }
    
    inline const V operator* () const { return *saved_iter;}
    //inline V& operator* () {return *saved_iter;}
    
    inline const_rev_iter<T>& operator++ (){ --(this->saved_iter);  return *this; }
    inline const_rev_iter<T>& operator-- (){ ++(this->saved_iter);  return *this; }

    inline const_rev_iter<T>& operator+= (size_t offset){ 
      advance(this->saved_iter, -offset);  return *this; 
    }

    inline const_rev_iter<T>& operator-= (size_t offset){ 
      advance(this->saved_iter, offset);  return *this; 
    }

    inline size_t operator- (const const_rev_iter<T>& rhs) const {
     return rhs.saved_iter - saved_iter;
    }

    template <class TT>
    inline bool operator==(const const_rev_iter<TT>& rhs) const {
      return (rhs.saved_iter == this->saved_iter); }

    template <class TT>
    inline bool operator==(const TT& rhs) const {
      TT tmp = rhs;
      return (--tmp == this->saved_iter);}

    template <class TT>
    inline bool operator!=(const TT& rhs) const { return !(*this==rhs);}

    inline T base() { return ++(this->saved_iter); }
 };



 template <class M, class CONTNR>
  struct cum_sum {
    static inline void apply(const M& margins_start,
                             const M& margins_end,
                             CONTNR &b) {
      partial_sum(margins_start, margins_end, b.begin() );
    }
  };

  
 template <class M, class CONTNR>
  struct rev_cum_sum {
    static inline void apply(const M& margins_start,
                             const M& margins_end,
                             CONTNR &b) {
      partial_sum(rev_iter<M>(margins_end), 
                  rev_iter<M>(margins_start), b.rbegin() );
    }
  };


 template <class CITER, class ACITER>  // Assumes ACITER is rand access
  struct Al_Times_C {
    static inline void apply(const CITER& c, size_t size, ACITER& Ac) {
      adjacent_difference( const_rev_iter<CITER>(advanced(c, size)),
              const_rev_iter<CITER>(c), rev_iter<ACITER>(Ac + size));
    }
 };
  

 template <class CITER, class ACITER>
  struct Au_Times_C {
    static inline void apply(const CITER& c, size_t size, ACITER& Ac) {
      adjacent_difference(c, advanced(c, size), Ac);
    }
 };
 
}

}

#endif  // INCLUDED_PAV_UTILS
