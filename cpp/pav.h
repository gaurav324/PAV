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

#ifndef INCLUDED_PAV_H
#define INCLUDED_PAV_H

#include <functional>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>
#include "pav_assert.h"
#include "pav_internal.h"
#include "under_oath.h"

namespace pav {

//using namespace std;
using std::iterator_traits;
using std::numeric_limits;
using std::vector;

//using namespace priv;
using priv::Interval;
using priv::first_violation;
using priv::left_search;
using priv::insert_and_left_search;
using priv::avg;

// REQUIRES: (const T in_strt, const T in_end) to be a valid interval,
//           that is, in_end has to be reachable from in_strt using ++.

//           Output iterator out_strt is safe to use to write as many
//           elements as in range (const T in_strt, const T in_end).

//           out_strt may equal in_strt.

//           Binary operator OP should define an order
//           (transitive, assymetric, reflexive)
//
//           AL should be of the same type as an STL allocator.
//           On linux the default allocator scales well enough
//           with multithreading. If it does not in your OS, you
//           can use your own allocator here.
// EXAMPLES:
//          // isotonic regress array of floats in_strt into array of floats
//          // out_strt according to ">"
//          pav<foat*, greater, std::allocator>(in_strt, in_end, out_strt);
//
//          // isotonic regress vector v1 into vector v2 according to "<=",
//          // but v2 filled bottom to top.
//
//          vector<int> v1(200), v2(200);
//          typedef vector<int>::iterator iter;
//          init(v1);
//          pav<iter, less_equal, std::allocator>
//                (v1.begin(), v1.end(), v2.rbegin());
//
//          // isotonic regress linked_list l1 into l1 according to "<=",
//          // but v2 filled from top, using a special multithreaded allocator
//
//          // typedef list<double>::iterator liter;
//          pav<liter, less_equal, Threaded::alloc>
//                             (l1.begin(), l1.end(), l1.begin());
//              ^^^^^^^^^^^^  Always a good idea to litter less.

template<template <typename> class OP, template <typename> class AL, class T, class TO>
void pav(T in_strt, T in_end, TO out_strt,
         typename iterator_traits<T>::value_type bot =
         - numeric_limits<typename iterator_traits<T>::value_type>::max(),
         typename iterator_traits<T>::value_type top =
           numeric_limits<typename iterator_traits<T>::value_type>::max()
        ) {
  typedef typename iterator_traits<T>::value_type V;
  typedef typename vector<V, AL<V> >::iterator VIter;
  assert( OP<V>()(bot, top) );
  
  std::vector<V, AL<V> > tmp;
  std::vector<int, AL<int> > wts;
  size_t n = std::distance(in_strt, in_end);
  tmp.reserve(n); wts.reserve(n);
  T bgn = in_strt;
  Interval<T> viol(in_end, in_end);
  std::pair<VIter, typename std::vector<int, AL<int> >::iterator> v_ends;
  VIter tmp_begin = tmp.begin();  // make sure  tmp does not get relocated
  while (bgn != in_end) {
    size_t merge_count = first_violation< OP<V> >(bgn, in_end, viol);
    if (merge_count > 1) {
      insert_and_left_search<OP>(tmp, wts, bot, bgn, viol.left);
      tmp.push_back(avg<OP<V> >(viol.left,
                                viol.right,
                                in_end, merge_count, top) );
      wts.push_back(merge_count);
      left_search<OP<V> >(tmp, wts, bot);
    } else { // merge count < 1
      insert_and_left_search<OP>(tmp, wts, bot, bgn, viol.right);
    }
    bgn = viol.right;
  }
  VIter tmp_end = tmp.end();
  typename std::vector<int, AL<int> >::iterator wt_iter = wts.begin();
  for (VIter i = tmp.begin(); i != tmp_end; ++i, ++wt_iter) {
    const V val = *i;
    for (int j = 0; j < *wt_iter; ++j) *(out_strt++) = val;
  }
}

} // namespace pav

#endif  // INCLUDED_PAV_H
