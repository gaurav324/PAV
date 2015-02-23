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

#ifndef INCLUDED_PAV_ASSERT_H
#define INCLUDED_PAV_ASSERT_H

#include <cassert>

#ifndef NDEBUG

#include <iostream>

class WarnOnce {
 public:

  static bool inited;

  WarnOnce() {
    if (!inited) {
      std::ios_base::sync_with_stdio(false);
      std::cerr <<"\t ___________________________________________________________\n" 
                <<"\t|                                                           |\n" 
                <<"\t| For performance, compile with the macro NDEBUG undefined. |\n"
                <<"\t| Usually it is the flag -DNDEBUG                           |\n"
                <<"\t|___________________________________________________________|\n\n"
                << std::endl;
      inited = true;
    }
  }

};

bool WarnOnce::inited = false;
static WarnOnce w ;
 
#endif  //  ifndef NDEBUG
#endif  //  #INCLUDED_PAV_ASSERT_H
