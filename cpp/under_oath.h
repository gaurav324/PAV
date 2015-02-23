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

#ifndef INCLUDED_UNDER_OATH
#define INCLUDED_UNDER_OATH

namespace pav {

 struct PassOnceException : public std::exception {
   std:: string s;
   PassOnceException(std::string description): s(description) {;}
   ~PassOnceException() throw()  {;}
   virtual std::string what(){ return s;}
 };

 template <class T>
 struct PassOncePointer {
   const T* it;
   PassOncePointer(const T &t) {it = &t;}

   PassOncePointer( PassOncePointer<T> &that) {
    if (that.it == NULL) {
      throw PassOnceException("This is supposed to be passed only once");
    } else {
      it = that.it;
      that.it = NULL;
    }
  }

  T get() {return *it;}
 };

 template <class T>
 struct UnaliasedUnderOath : public PassOncePointer<T> {
   // I hereby solemnly declare
   // that 'it' is not aliased 
   // with anything that may get
   // deleted during this object's
   // lifetime. May the  runtime
   // smite me to my core otherwise.
   UnaliasedUnderOath(const T &t) : PassOncePointer<T>(t) {;}
 };

}

#endif  // INCLUDED_UNDER_OATH
