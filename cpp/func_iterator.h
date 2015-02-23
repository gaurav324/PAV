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

#ifndef INCLUDED_PAV_FUNCITER
#define INCLUDED_PAV_FUNCITER

#include <iterator>

namespace pav {
 
  using namespace std;

template <class V, class DERIVED>
  struct FuncBase {
    const V value;
    explicit FuncBase(V val ) : value(val) {}
    const V operator() (size_t i) { return static_cast<DERIVED&>(*this)(i); }
  };

  template <class T>
  struct ConstFunc : public FuncBase<T, ConstFunc<T> > {
    ConstFunc(T val) : FuncBase<T, ConstFunc<T>  >(val) {}
    typedef T value_type;
    const T operator() (size_t i) const {return this->value;}
  };

  template <class T>
  struct ConstOverT : public FuncBase<T, ConstOverT<T> > {
    ConstOverT(T val) : FuncBase<T, ConstOverT<T> >(val) {}
    typedef T value_type;
    const T operator() (size_t i) const {return (this->value)/i;}
  };

  template <class T>
  struct ConstOverSqT : public FuncBase<T, ConstOverSqT<T> >  {
    ConstOverSqT(T val) : FuncBase<T, ConstOverSqT<T>  >(val) {}
    typedef T value_type;
    const T operator() (size_t i) const {return this->value/(i*i);}
  };

  template <class T>
  struct  Arith : public FuncBase<T, ConstOverSqT<T> >  {
    Arith(T val) : FuncBase<T, ConstOverSqT<T>  >(val) {}
    typedef T value_type;
    const T operator() (size_t i) const {return this->value - i ;}
  };

  template <int T>
  struct greater_ {
    const bool operator() (size_t i) const { return (i > static_cast<size_t>(T)) ;}
  }; 

  template <class FUNC, class COND>
  struct FuncIterator : iterator<
                       random_access_iterator_tag,
                       typename FUNC::value_type > {
    typedef typename FUNC::value_type V;
    typedef typename iterator<random_access_iterator_tag,
                     typename FUNC::value_type >::difference_type D;

    FUNC func;
    const V init_val;
    size_t i;
    const COND test;  // examples test(i) = (i > 0)
                //                  = (i < len)
    FuncIterator(const FUNC& f, V init) : func(f), init_val(init), i(0), test(COND()) {}

    FuncIterator(V val, V init): func( FUNC(val) ), init_val(init), i(0), test(COND()) {}

    const V operator *() const {return (test(i)) ? func(i) : init_val ;}

    FuncIterator<FUNC, COND>& operator ++() {++i; return *this;}

    FuncIterator<FUNC, COND>& operator --() {--i; return *this;}

    FuncIterator<FUNC, COND>& operator +=(size_t t) {i+=t; return *this;}
    FuncIterator<FUNC, COND>& operator -=(size_t t) {i-=t; return *this;}

    D operator -(const FuncIterator<FUNC, COND>& rhs) const {return (i - rhs.i);}

    const bool operator== (const FuncIterator<FUNC, COND>& rhs) const {
      return ((func.value == rhs.func.value) &&  (init_val == rhs.init_val) && (i == rhs.i));
    }

    const bool operator!= (const FuncIterator<FUNC, COND>& rhs) const {return ! (*this==(rhs));}
  };
}

#endif  // INCLUDED_PAV_FUNCITER
