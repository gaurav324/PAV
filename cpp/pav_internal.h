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

#ifndef INCLUDED_PAV_INTERNAL_H
#define INCLUDED_PAV_INTERNAL_H

#include <iterator>
#include <numeric>

namespace pav {

namespace priv {

  using namespace std;

  template<class T>
  struct Interval {
   T left;
   T right;
   Interval(T l, T r): left(l), right(r){;}
  };


  template <class OP, class T> size_t
  first_violation(T beg, T end, Interval<T>& viol) {
  // __attribute__((pure));
    T first = beg;
    T secnd = ++beg;
    size_t count = 0;
    for (; (secnd != end) && OP()(*first, *secnd); ++first, ++secnd);
    if (secnd == end) {
      viol.left = first; viol.right= secnd;
      return count;
    }
    // Now (*first > *secnd) strictly
    T third = ++secnd; --secnd; count = 2;
    for(; (third != end) && OP()(*third, *secnd); ++secnd, ++third, ++count);
    viol.left = first; viol.right = third;
    return count;
  }


    template <class OP, class T>
    typename iterator_traits<T>::value_type
    avg(const T left,
        const T right,
        const T end,
        size_t count,
        typename iterator_traits<T>::value_type top
     ) {
      typedef typename iterator_traits<T>::value_type V;
      V ret_val = accumulate(left, right, static_cast<V>(0.0));
      return ( (right == end) && !OP()(ret_val, top) ) ? top : ret_val/count;
  }


  template <class OP, template<typename> class AL, class V> inline void
    left_search(vector<V, AL<V> >& tmp, vector<int,  AL<int> >& wts, V bot) {
      typedef typename vector<V>::iterator VIter;
      VIter tmp_old_end;
      typedef typename vector<int, AL<int> >::iterator IIter;
      IIter wts_old_end;

      VIter vbgn = tmp.begin();
      
      VIter vleft = tmp.end();
      tmp_old_end = vleft--;
      VIter vrght = vleft--;   // CAUTION depends on post incr
      IIter wleft = wts.end();
      wts_old_end = wleft--;
      IIter wrght = wleft--;

      OP Op;
      while ( (vrght != vbgn) && !Op(*vleft, *vrght) ) {
        *vleft *=  (*wleft);
        *vleft +=  (*vrght) * (*wrght);
        *wleft += *wrght;
        *vleft /= *wleft;
        vrght = vleft--; 
        wrght = wleft--;
      }
      if ( (vrght == vbgn) && Op(*vrght, bot) ) *vrght = bot; 
      tmp.erase(++vrght, tmp_old_end);
      wts.erase(++wrght, wts_old_end);
    }

   template < template <typename> class OP, template<typename> class AL, 
              class V, class T> void
     insert_and_left_search(vector<V, AL<V> >& tmp, vector<int, AL<int> >& wts,
                            V bot, T range_left, T range_right) {
       if (tmp.empty() ) {
          tmp.insert(tmp.end(), range_left, range_right);
          wts.resize(tmp.size(), 1);
        } else {
          for(T i = range_left; i != range_right; ++i) {
            tmp.push_back(*i);
            wts.push_back(1);
            left_search<OP<V> >(tmp, wts, bot);
          }
       }
    }

  }  // namespace priv
}  // namespace pav

#endif  // INCLUDED_PAV_H
