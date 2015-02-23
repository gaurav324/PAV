#include <cstdio>
#include <functional>
#include "pav.h"
#include "margin_pav.h"
#include "func_iterator.h"


extern "C" {
void pav_float(float *in_array, size_t size, float *out_array) {
  pav::pav<std::less_equal, std::allocator>(in_array, in_array + size, out_array);
  }

void lbound_pav_float(float* in_strt, size_t size, float* margins, float* out ) {
  pav::lbound_margin_pav<std::allocator>(in_strt, in_strt + size, out, margins); 
  }

void ubound_pav_float(float* in_strt, size_t size, float* margins, float* out ) {
  pav::ubound_margin_pav<std::allocator>(in_strt, in_strt + size, out, margins); 
  }

  void lbound_mmpav_float(float* in_strt, size_t size, float* out, const float* c ) {
    pav::lbound_maxmargin_pav<std::allocator>(in_strt, in_strt + size, out, c);
  }
  
  void lbound_mmfpav_float(float* in_strt, size_t size, float* out) {
    pav::lbound_maxmargin_pav<std::allocator>(in_strt, in_strt + size, out,
       pav::FuncIterator<pav::Arith<float>,
                         pav::greater_<0> >(size, 0.0) );
  }
}


void print_const_size_array(float *a_out, size_t size) {
  for (int i = 0; i < size; ++i)
    printf("%f ", a_out[i]);
  printf("\n");
}


int main() {
  float a[] = {1, 11, 8, 7, 6, 7, 13, 12, 11, 8, 9, 10, 4, 8};
  float a_out[14];
  float lmargins[14] = { -5, 0.5,  0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
  float umargins[14] = { 0.5, 0.5,  0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                        0.5, 0.5, 0.5, 0.5, 0.5, -25 };
  float null_lmargins[14] = { -5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, 0.0}; 
  float null_umargins[14] = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                             0.0, 0.0, 0.0, 0.0, -25};
  float costs[14] = {0, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  print_const_size_array(a, 14);

  printf("calling pav\n");
  pav_float(a, 14, a_out);
  print_const_size_array(a_out, 14);

  printf("expected:\n");
  printf("1.000000 7.800000 7.800000 7.800000 7.800000 7.800000 9.375000 " 
         "9.375000 9.375000 9.375000 9.375000 9.375000 9.375000 9.375000\n");

  lbound_pav_float(a, 14, lmargins, a_out);
  print_const_size_array(a_out, 14);
  printf("\n");
  
  lbound_pav_float(a, 14, null_lmargins, a_out);
  print_const_size_array(a_out, 14);
  printf("\n");

  ubound_pav_float(a, 14, umargins, a_out);
  print_const_size_array(a_out, 14);
  printf("\n");
  
  ubound_pav_float(a, 14, null_umargins, a_out);
  print_const_size_array(a_out, 14);
  printf("\n");

  printf("\n\n\n");
  lbound_mmpav_float(a, 14, a_out, costs);
  print_const_size_array(a_out, 14);

  printf("\n\n\n");
  lbound_mmfpav_float(a, 14, a_out);
  print_const_size_array(a_out, 14);
  return 0;
}
