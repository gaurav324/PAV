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

#ifndef INCLUDED_MPAV_H
#define INCLUDED_MPAV_H

#include <algorithm>
#include <deque>
#include <functional>
#include <list>
#include <numeric>
#include <vector>
#include "pav.h"
#include "chain_it.h"
#include "utils.h"

namespace pav {
using namespace std;
using namespace margin_utils;
using namespace chainit;

enum Z_OR_X {RETURN_X, RETURN_Z};

template<template <typename, typename> class PARTSUM, 
         template <typename> class AL,
         template <typename> class OP1,
         template <typename> class OP2,
         class T, class TO, class M>
void bound_margin_pav(const T in_strt,
                      const T in_end,
                      TO out_strt,
                      const M margins_start,
                      Z_OR_X zorx,
                      typename iterator_traits<T>::value_type bot,
                      typename iterator_traits<T>::value_type top) {
  typedef typename iterator_traits<T>::value_type V;
  vector<V, AL<V> > z(in_strt, in_end);
  size_t size = distance(in_strt, in_end);
  typedef vector<V, AL<V> > VV;
  VV b(size);  // does unnecessary initiallization
  M margins_end = advanced(margins_start, size);
  PARTSUM<M, VV>::apply(margins_start, margins_end, b);
  transform(z.begin(), z.end(), b.begin(), z.begin(), OP1<V>());
  //UnaliasedUnderOath<TO> wrapped_out_iter(out_strt);
  pav<std::less_equal, AL>(z.begin(), z.end(), out_strt, bot, top);
  if (zorx == RETURN_X) {
    transform(out_strt, advanced(out_strt, size), b.begin(), out_strt, OP2<V>());
  }
}


// y is specified with iterators [in_strt, in_end)
// m is specified with iterators [margin_start, )
// This solves:
//    argmin_x ||x - y||^2
//    s.t. x_0           >=  m_0
//    s.t. x_i - x_{i+1} <= -m_i  \forall  i \in [1, n)
template<template <typename> class AL, class T, class TO, class M>
void lbound_margin_pav(const T in_strt,
                       const T in_end,
                       TO out_strt, 
                       const M margins_start,
                       Z_OR_X zorx = RETURN_X) {
  typedef typename iterator_traits<T>::value_type V;
  bound_margin_pav<cum_sum, AL, std::minus, std::plus>
  (in_strt, in_end, out_strt, margins_start, zorx, static_cast<V>(0.0),
                                                   numeric_limits<V>::max() );
} 


// y is specified with iterators [in_strt, in_end)
// m is specified with iterators [margin_start, )
// This solves:
//    argmin_x ||x - y||^2
//    s.t. x_{n-1}       <= -m_{n-1}
//    s.t. x_{i+1} - x_i >=  m_i  \forall  i \in [0, n-1)
template<template <typename> class AL, class T, class TO, class M>
void ubound_margin_pav(const T in_strt,
                       const T in_end,
                       TO out_strt, 
                       const M margins_start,
                       Z_OR_X zorx = RETURN_X) {
  typedef typename iterator_traits<T>::value_type V;
  bound_margin_pav<rev_cum_sum, AL, std::plus, std::minus>
  (in_strt, in_end, out_strt, margins_start, zorx, - numeric_limits<V>::max(),
                                                   static_cast<V>(0.0) );
}


template<class A_TIMES_C, template <typename> class AL, class T, class TO, class CITER>
void bound_maxmargin_pav(const T in_strt,
                       const T in_end,
                       TO out_strt,
                       CITER c,
                       typename iterator_traits<T>::value_type bot,
                       typename iterator_traits<T>::value_type top,
                       const typename iterator_traits<T>::value_type delta=1e-5
                       ) {
  size_t size = distance(in_strt, in_end);
  typedef typename iterator_traits<T>::value_type V;

  V *m_new = new V[4*size]; // TODO use auto_ptr
  V *m_old = m_new + size;
  V *Ac    = m_old + size;
  V *z     = Ac + size;
  V *m_save= m_new;
  V *Ac_end = Ac + size;

  fill(m_old, m_old+size, static_cast<V>(0));
  A_TIMES_C::apply(c, size, Ac); // Ac = - A^t c

  pav<std::less_equal, AL>
  (vminus(in_strt, m_old), vminus(in_end, m_old+size), z, bot, top);
  pav<std::less_equal, AL>
  (vplus(Ac, vminus(in_strt, z)), vplus(Ac_end, vminus(in_end, z+size)),
   m_new, bot, top);

  while (dist(m_old, m_new, size) >= delta*delta) {
    pav<std::less_equal, AL>
    (vminus(in_strt, m_new), vminus(in_end, m_new+size), z, bot, top);
    swap(m_old, m_new);
    pav<std::less_equal, AL>
    (vplus(Ac, vminus(in_strt, z)), vplus(Ac_end, vminus(in_end, z+size)),
    m_new, bot, top);
  }
  copy( vplus(z, m_new),  vplus(z+size, m_new+size), out_strt);  // compute x
  delete [] m_save;
}


// y is specified with iterators [in_strt, in_end)
// c is specified with iterators [margin_start, )
// This solves:
//    argmin_x ||x - y||^2 - <C, m>
//    s.t. x_0           >= m_0
//    s.t. x_i - x_{i+1} <= -m_i  \forall  i \in [1, n)
//    s.t. m >= 0
template<template <typename> class AL, class T, class TO, class CITER>
void lbound_maxmargin_pav(const T in_strt,
                       const T in_end,
                       TO out_strt,
                       CITER c,
                       const typename iterator_traits<T>::value_type delta=1e-5
                       ) {
    typedef typename iterator_traits<T>::value_type V;
    assert(*c == static_cast<V>(0.0));  // first component is not a gap
    bound_maxmargin_pav<Al_Times_C<CITER, V*>, AL>
    (in_strt, in_end, out_strt, c,  static_cast<V>(0.0),
     numeric_limits<V>::max(), delta);
}


// y is specified with iterators [in_strt, in_end)
// c is specified with iterators [margin_start, )
// This solves:
//    argmin_x ||x - y||^2 - <C, m>
//    s.t. x_{n-1}       <= -m_{n-1}
//    s.t. x_{i+1} - x_i >=  m_i  \forall  i \in [0, n-1)
//    s.t. m >= 0
template<class T, class TO, class CITER, template <typename> class AL>
void ubound_maxmargin_pav(const T in_strt,
                       const T in_end,
                       TO out_strt,
                       CITER c,
                       const typename iterator_traits<T>::value_type delta=1e-5
                       ) {
    typedef typename iterator_traits<T>::value_type V;
    assert(*advanced(c, static_cast<size_t>(distance(in_strt, in_end)))
                     == static_cast<V>(0.0));   // last component is not a gap
    bound_maxmargin_pav<Au_Times_C<CITER, V*>, AL>
    (in_strt, in_end, out_strt, c, - numeric_limits<V>::max(),
     static_cast<V>(0.0), delta);
}

 
}
#endif // MPAV_H
