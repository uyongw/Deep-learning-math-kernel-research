#include <assert.h>
#include <x86intrin.h>
#include "el_def.hpp"
#include "el_utils.hpp"
#include "elx_conv.hpp"
#include "elk_conv.hpp"

#undef ADD
#undef SUB
#undef FMADD
#undef FMSUB
#undef MUL
#define ADD _mm512_add_ps
#define SUB _mm512_sub_ps
#define MUL _mm512_mul_ps
#define FMADD _mm512_fmadd_ps
#define FMSUB _mm512_fmsub_ps

namespace euler {

template <typename T, const int A, const int K, const int V, const int I>
void elk_trans_input(elx_conv_t<T> &xc, T atinput[A][A][V], T *input) {}

template <typename T, const int A, const int K, const int V, const int I>
void elk_trans_input(elx_conv_t<T> &xc, T atinput[A][A][V], T *input,
                     int _hA_start, int _hA_end, int _wA_start, int _wA_end) {}


template <>
void elk_trans_input<float, 5, 3, 16, ISA_GENERIC>(elx_conv_t<float> &xc,
                                                   float atinput[5][5][16],
                                                   float *input,
                                                   int _hT_start, int _hT_end,
                                                   int _wT_start, int _wT_end) {
  const float z2 = 2.0f;
  const float z3 = 3.0f;
  const float z4 = 4.0f;
  const float z6 = 6.0f;

  auto f = [&](int _h, int _w, int _V) {
    int _i = _h * xc.input_strides[2] + _w * 16 + _V;
    if (_h < _hT_start || _w < _wT_start || _h > _hT_end || _w > _wT_end)
      return 0.0f;
    else
      return *(input + _i);
  };

#undef F
#undef C
#undef T
#define F(_h, _w) f(_h, _w, _V)
#define C(n) C##n[_V]
#define T(_h, _w) atinput[_h][_w][_V]

  float C1[16], C2[16], C3[16];
#pragma omp simd
  for (int _V = 0; _V < 16; ++_V) {
    C(1) = F(1, 1) + z2 * F(1, 2) - z2 * F(1, 0) - F(1, 3);
    C(2) = F(2, 1) + z2 * F(2, 2) - z2 * F(2, 0) - F(2, 3);
    C(3) = F(3, 1) + z2 * F(3, 2) - z2 * F(3, 0) - F(3, 3);
    T(0, 0) = z4 * F(0, 0) - z2 * F(0, 1) - z4 * F(0, 2) + z2 * F(0, 3) + C(1) +
              z2 * C(2) - C(3);
    T(1, 0) = z3 * C(2) - z2 * C(1) - C(3);
    T(2, 0) = z2 * C(1) + C(2) - C(3);
    T(3, 0) = C(1) - C(3);
    T(4, 0) = z2 * F(4, 0) - F(4, 1) - z2 * F(4, 2) + F(4, 3) - z2 * C(1) +
              C(2) + z2 * C(3);

    C(1) = z3 * F(1, 2) - z2 * F(1, 1) - F(1, 3);
    C(2) = z3 * F(2, 2) - z2 * F(2, 1) - F(2, 3);
    C(3) = z3 * F(3, 2) - z2 * F(3, 1) - F(3, 3);
    T(0, 1) =
        z4 * F(0, 1) - z6 * F(0, 2) + z2 * F(0, 3) + C(1) + z2 * C(2) - C(3);
    T(1, 1) = z3 * C(2) - z2 * C(1) - C(3);
    T(2, 1) = z2 * C(1) + C(2) - C(3);
    T(3, 1) = C(1) - C(3);
    T(4, 1) =
        z2 * F(4, 1) - z3 * F(4, 2) + F(4, 3) - z2 * C(1) + C(2) + z2 * C(3);

    C(1) = z2 * F(1, 1) + F(1, 2) - F(1, 3);
    C(2) = z2 * F(2, 1) + F(2, 2) - F(2, 3);
    C(3) = z2 * F(3, 1) + F(3, 2) - F(3, 3);
    T(0, 2) =
        z2 * F(0, 3) - z2 * F(0, 2) - z4 * F(0, 1) + C(1) + z2 * C(2) - C(3);
    T(1, 2) = z3 * C(2) - z2 * C(1) - C(3);
    T(2, 2) = z2 * C(1) + C(2) - C(3);
    T(3, 2) = C(1) - C(3);
    T(4, 2) = F(4, 3) - z2 * F(4, 1) - F(4, 2) - z2 * C(1) + C(2) + z2 * C(3);

    C(1) = F(1, 1) - F(1, 3);
    C(2) = F(2, 1) - F(2, 3);
    C(3) = F(3, 1) - F(3, 3);
    T(0, 3) = z2 * F(0, 3) - z2 * F(0, 1) + C(1) + z2 * C(2) - C(3);
    T(1, 3) = z3 * C(2) - z2 * C(1) - C(3);
    T(2, 3) = z2 * C(1) + C(2) - C(3);
    T(3, 3) = C(1) - C(3);
    T(4, 3) = F(4, 3) - F(4, 1) - z2 * C(1) + C(2) + z2 * C(3);

    C(1) = F(1, 2) + z2 * F(1, 3) - z2 * F(1, 1) - F(1, 4);
    C(2) = F(2, 2) + z2 * F(2, 3) - z2 * F(2, 1) - F(2, 4);
    C(3) = F(3, 2) + z2 * F(3, 3) - z2 * F(3, 1) - F(3, 4);
    T(0, 4) = z4 * F(0, 1) - z2 * F(0, 2) - z4 * F(0, 3) + z2 * F(0, 4) + C(1) +
              z2 * C(2) - C(3);
    T(1, 4) = z3 * C(2) - z2 * C(1) - C(3);
    T(2, 4) = z2 * C(1) + C(2) - C(3);
    T(3, 4) = C(1) - C(3);
    T(4, 4) = z2 * F(4, 1) - F(4, 2) - z2 * F(4, 3) + F(4, 4) - z2 * C(1) +
              C(2) + z2 * C(3);
  }
}

template <>
void elk_trans_input<float, 5, 3, 16, ISA_GENERIC>(elx_conv_t<float> &xc,
                                                   float atinput[5][5][16],
                                                   float *input) {
  elk_trans_input<float, 5, 3, 16, ISA_GENERIC>(xc, atinput, input, 0, 4, 0, 4);
}

template <>
void elk_trans_input<float, 5, 3, 16, ISA_SKX_AVX512>(elx_conv_t<float> &xc,
                                                      float atinput[5][5][16],
                                                      float *input) {
  ENABLE_AVX512F();

  // Inputs
  __m512 f00, f01, f02, f03, f04,
         f10, f11, f12, f13, f14,
         f20, f21, f22, f23, f24,
         f30, f31, f32, f33, f34,
         f40, f41, f42, f43, f44;
  // Cache
  __m512 c1, c2, c3;
  // Outputs
  __m512 t00, t01, t02, t03, t04,
         t10, t11, t12, t13, t14,
         t20, t21, t22, t23, t24,
         t30, t31, t32, t33, t34,
         t40, t41, t42, t43, t44;

  __m512 z2 = _mm512_set_ps(IMM_BCAST16(2.0f));
  __m512 z3 = _mm512_set_ps(IMM_BCAST16(3.0f));
  __m512 z4 = _mm512_set_ps(IMM_BCAST16(4.0f));
  __m512 z6 = _mm512_set_ps(IMM_BCAST16(6.0f));

  auto f = [&](int _h, int _w) {
    mdarray<float, 3> ainput(input, xc.ih, xc.iw, 16);
    return _mm512_load_ps(&ainput(_h, _w, 0));
  };

#undef F
#undef C
#undef T
#define F(_h, _w) f(_h, _w)
#define T(h, w) atinput[h][w]

  f00 = F(0, 0); f01 = F(0, 1); f02 = F(0, 2); f03 = F(0, 3);
  f10 = F(1, 0); f11 = F(1, 1); f12 = F(1, 2); f13 = F(1, 3);
  f20 = F(2, 0); f21 = F(2, 1); f22 = F(2, 2); f23 = F(2, 3);
  f30 = F(3, 0); f31 = F(3, 1); f32 = F(3, 2); f33 = F(3, 3);
  f40 = F(4, 0); f41 = F(4, 1); f42 = F(4, 2); f43 = F(4, 3);

  c1 = FMSUB(z2, f10, FMADD(z2, f12, SUB(f11, f13)));
  c2 = FMSUB(z2, f20, FMADD(z2, f22, SUB(f21, f23)));
  c3 = FMSUB(z2, f30, FMADD(z2, f32, SUB(f31, f33)));
  t00 =
      FMADD(z2, c2,
            FMADD(z2, f03,
                  FMSUB(z4, f02, FMSUB(z2, f01, FMADD(z4, f00, SUB(c1, c3))))));
  _mm512_store_ps(T(0, 0), t00);
  t10 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 0), t10);
  t20 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 0), t20);
  t30 = SUB(c1, c3);
  _mm512_store_ps(T(3, 0), t30);
  t40 = FMADD(z2, c3,
              FMSUB(z2, c1, FMSUB(z2, f42, FMADD(z2, f40, SUB(f43, f41)))));
  _mm512_store_ps(T(4, 0), t40);

  c1  = SUB(FMSUB(z2, f11, MUL(z3, f12)), f13);
  c2  = SUB(FMSUB(z2, f21, MUL(z3, f22)), f23);
  c3  = SUB(FMSUB(z2, f31, MUL(z3, f32)), f33);
  t01 = FMADD(z2, c2,
              FMADD(z2, f03, FMSUB(z6, f02, FMADD(z4, f01, SUB(c1, c3)))));
  _mm512_store_ps(T(0, 1), t01);
  t11 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 1), t11);
  t21 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 1), t21);
  t31 = SUB(c1, c3);
  _mm512_store_ps(T(3, 1), t31);
  t41 = FMADD(z2, c3,
              FMSUB(z2, c1, FMSUB(z3, f42, FMADD(z2, f41, ADD(f43, c2)))));
  _mm512_store_ps(T(4, 1), t41);

  c1  = FMADD(z2, f11, SUB(f12, f13));
  c2  = FMADD(z2, f21, SUB(f22, f23));
  c3  = FMADD(z2, f31, SUB(f32, f33));
  t02 = FMADD(z2, c2,
              FMSUB(z4, f01, FMSUB(z2, f02, FMADD(z2, f03, SUB(c1, c3)))));
  _mm512_store_ps(T(0, 2), t02);
  t12 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 2), t12);
  t22 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 2), t22);
  t32 = SUB(c1, c3);
  _mm512_store_ps(T(3, 2), t32);
  t42 = ADD(FMADD(z2, c3, FMSUB(z2, c1, FMSUB(z2, f41, SUB(f43, f42)))), c2);
  _mm512_store_ps(T(4, 2), t42);

  c1 = SUB(f11, f13);
  c2 = SUB(f21, f23);
  c3 = SUB(f31, f33);
  t03 = FMADD(z2, c2, FMSUB(z2, f01, FMADD(z2, f03, SUB(c1, c3))));
  _mm512_store_ps(T(0, 3), t03);
  t13 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 3), t13);
  t23 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 3), t23);
  t33 = SUB(c1, c3);
  _mm512_store_ps(T(3, 3), t33);
  t43 = ADD(FMADD(z2, c3, FMSUB(z2, c1, SUB(f43, f41))), c2);
  _mm512_store_ps(T(4, 3), t43);

  f04 = F(0, 4); f14 = F(1, 4); f24 = F(2, 4); f34 = F(3, 4); f44 = F(4, 4);

  c1 = FMSUB(z2, f11, FMADD(z2, f13, SUB(f12, f14)));
  c2 = FMSUB(z2, f21, FMADD(z2, f23, SUB(f22, f24)));
  c3 = FMSUB(z2, f31, FMADD(z2, f33, SUB(f32, f34)));

  t34 = SUB(c1, c3);
  _mm512_store_ps(T(3, 4), t34);
  t04 =
      ADD(FMADD(z2, c2, FMADD(z2, f04, FMSUB(z2, f02, MUL(z4, SUB(f03, f03))))),
          t34);
  _mm512_store_ps(T(0, 4), t04);
  t44 = ADD(
      FMADD(z2, c3, FMSUB(z2, c1, ADD(SUB(MUL(z2, SUB(f41, f43)), f42), f44))),
      c2);
  _mm512_store_ps(T(4, 4), t44);
  t14 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 4), t14);
  t24 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 4), t24);
}

template <>
void elk_trans_input<float, 5, 3, 16, ISA_SKX_AVX512>(
    elx_conv_t<float> &xc, float atinput[5][5][16], float *input, int _hT_start,
    int _hT_end, int _wT_start, int _wT_end) {

  ENABLE_AVX512F();

  // Inputs
  __m512 f00, f01, f02, f03, f04, f10, f11, f12, f13, f14, f20, f21, f22, f23,
      f24, f30, f31, f32, f33, f34, f40, f41, f42, f43, f44;
  // Cache
  __m512 c1, c2, c3;
  // Outputs
  __m512 t00, t01, t02, t03, t04, t10, t11, t12, t13, t14, t20, t21, t22, t23,
      t24, t30, t31, t32, t33, t34, t40, t41, t42, t43, t44;

  __m512 z0 = _mm512_setzero_ps();
  __m512 z2 = _mm512_set_ps(IMM_BCAST16(2.0f));
  __m512 z3 = _mm512_set_ps(IMM_BCAST16(3.0f));
  __m512 z4 = _mm512_set_ps(IMM_BCAST16(4.0f));
  __m512 z6 = _mm512_set_ps(IMM_BCAST16(6.0f));

  auto f = [&](int _h, int _w) {
    int _i = _h * xc.input_strides[2] + _w * 16;
    if (_h < _hT_start || _w < _wT_start || _h > _hT_end || _w > _wT_end)
      return z0;
    else
      return _mm512_load_ps(input + _i);
  };

#undef F
#undef C
#undef T
#define F(_h, _w) f(_h, _w)
#define T(h, w) atinput[h][w]

  f00 = F(0, 0); f01 = F(0, 1); f02 = F(0, 2); f03 = F(0, 3);
  f10 = F(1, 0); f11 = F(1, 1); f12 = F(1, 2); f13 = F(1, 3);
  f20 = F(2, 0); f21 = F(2, 1); f22 = F(2, 2); f23 = F(2, 3);
  f30 = F(3, 0); f31 = F(3, 1); f32 = F(3, 2); f33 = F(3, 3);
  f40 = F(4, 0); f41 = F(4, 1); f42 = F(4, 2); f43 = F(4, 3);

  c1 = FMSUB(z2, f10, FMADD(z2, f12, SUB(f11, f13)));
  c2 = FMSUB(z2, f20, FMADD(z2, f22, SUB(f21, f23)));
  c3 = FMSUB(z2, f30, FMADD(z2, f32, SUB(f31, f33)));
  t00 =
      FMADD(z2, c2,
            FMADD(z2, f03,
                  FMSUB(z4, f02, FMSUB(z2, f01, FMADD(z4, f00, SUB(c1, c3))))));
  _mm512_store_ps(T(0, 0), t00);
  t10 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 0), t10);
  t20 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 0), t20);
  t30 = SUB(c1, c3);
  _mm512_store_ps(T(3, 0), t30);
  t40 = FMADD(z2, c3,
              FMSUB(z2, c1, FMSUB(z2, f42, FMADD(z2, f40, SUB(f43, f41)))));
  _mm512_store_ps(T(4, 0), t40);

  c1  = SUB(FMSUB(z2, f11, MUL(z3, f12)), f13);
  c2  = SUB(FMSUB(z2, f21, MUL(z3, f22)), f23);
  c3  = SUB(FMSUB(z2, f31, MUL(z3, f32)), f33);
  t01 = FMADD(z2, c2,
              FMADD(z2, f03, FMSUB(z6, f02, FMADD(z4, f01, SUB(c1, c3)))));
  _mm512_store_ps(T(0, 1), t01);
  t11 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 1), t11);
  t21 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 1), t21);
  t31 = SUB(c1, c3);
  _mm512_store_ps(T(3, 1), t31);
  t41 = FMADD(z2, c3,
              FMSUB(z2, c1, FMSUB(z3, f42, FMADD(z2, f41, ADD(f43, c2)))));
  _mm512_store_ps(T(4, 1), t41);

  c1  = FMADD(z2, f11, SUB(f12, f13));
  c2  = FMADD(z2, f21, SUB(f22, f23));
  c3  = FMADD(z2, f31, SUB(f32, f33));
  t02 = FMADD(z2, c2,
              FMSUB(z4, f01, FMSUB(z2, f02, FMADD(z2, f03, SUB(c1, c3)))));
  _mm512_store_ps(T(0, 2), t02);
  t12 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 2), t12);
  t22 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 2), t22);
  t32 = SUB(c1, c3);
  _mm512_store_ps(T(3, 2), t32);
  t42 = ADD(FMADD(z2, c3, FMSUB(z2, c1, FMSUB(z2, f41, SUB(f43, f42)))), c2);
  _mm512_store_ps(T(4, 2), t42);

  c1 = SUB(f11, f13);
  c2 = SUB(f21, f23);
  c3 = SUB(f31, f33);
  t03 = FMADD(z2, c2, FMSUB(z2, f01, FMADD(z2, f03, SUB(c1, c3))));
  _mm512_store_ps(T(0, 3), t03);
  t13 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 3), t13);
  t23 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 3), t23);
  t33 = SUB(c1, c3);
  _mm512_store_ps(T(3, 3), t33);
  t43 = ADD(FMADD(z2, c3, FMSUB(z2, c1, SUB(f43, f41))), c2);
  _mm512_store_ps(T(4, 3), t43);

  f04 = F(0, 4); f14 = F(1, 4); f24 = F(2, 4); f34 = F(3, 4); f44 = F(4, 4);

  c1 = FMSUB(z2, f11, FMADD(z2, f13, SUB(f12, f14)));
  c2 = FMSUB(z2, f21, FMADD(z2, f23, SUB(f22, f24)));
  c3 = FMSUB(z2, f31, FMADD(z2, f33, SUB(f32, f34)));

  t34 = SUB(c1, c3);
  _mm512_store_ps(T(3, 4), t34);
  t04 =
      ADD(FMADD(z2, c2, FMADD(z2, f04, FMSUB(z2, f02, MUL(z4, SUB(f03, f03))))),
          t34);
  _mm512_store_ps(T(0, 4), t04);
  t44 = ADD(
      FMADD(z2, c3, FMSUB(z2, c1, ADD(SUB(MUL(z2, SUB(f41, f43)), f42), f44))),
      c2);
  _mm512_store_ps(T(4, 4), t44);
  t14 = SUB(FMSUB(z2, c1, MUL(z3, c2)), c3);
  _mm512_store_ps(T(1, 4), t14);
  t24 = FMADD(z2, c1, SUB(c2, c3));
  _mm512_store_ps(T(2, 4), t24);

}

}  // namespace euler
