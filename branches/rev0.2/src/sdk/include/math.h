#ifndef  __MATH_H__
#define  __MATH_H__

#define _M_LN2        0.693147180559945309417

# ifndef HUGE_VAL
#  define HUGE_VAL (__builtin_huge_val())
# endif

# ifndef HUGE_VALF
#  define HUGE_VALF (__builtin_huge_valf())
# endif

# ifndef HUGE_VALL
#  define HUGE_VALL (__builtin_huge_vall())
# endif

# ifndef INFINITY
#  define INFINITY (__builtin_inff())
# endif

# ifndef NAN
#  define NAN (__builtin_nanf(""))
# endif


#define MAXFLOAT    3.40282347e+38F
#define M_E         2.7182818284590452354
#define M_LOG2E     1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2       _M_LN2
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_TWOPI     (M_PI * 2.0)
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_3PI_4		2.3561944901923448370E0
#define M_SQRTPI    1.77245385090551602792981
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#define M_LN2LO     1.9082149292705877000E-10
#define M_LN2HI     6.9314718036912381649E-1
#define M_SQRT3     1.73205080756887719000
#define M_IVLN10    0.43429448190325182765 
#define M_LOG2_E    _M_LN2
#define M_INVLN2    1.4426950408889633870E0  

#define FP_NAN         0
#define FP_INFINITE    1
#define FP_ZERO        2
#define FP_SUBNORMAL   3
#define FP_NORMAL      4

#ifndef FP_ILOGB0
# define FP_ILOGB0 (-INT_MAX)
#endif
#ifndef FP_ILOGBNAN
# define FP_ILOGBNAN INT_MAX
#endif

#ifndef MATH_ERRNO
# define MATH_ERRNO 1
#endif

#ifndef MATH_ERREXCEPT
# define MATH_ERREXCEPT 2
#endif

#define DOMAIN 1
#define SING 2
#define OVERFLOW 3
#define UNDERFLOW 4
#define TLOSS 5
#define PLOSS 6

typedef float  float_t;
typedef double double_t;


#ifdef __cplusplus
extern "C" {
#endif

double atan(double);
double cos(double);
double sin(double);
double tan(double);
double tanh(double);
double frexp(double, int *);
double modf(double, double *);
double ceil(double);
double fabs(double);
double floor(double);
double acos(double);
double asin(double);
double atan2(double, double);
double cosh(double);
double sinh(double);
double exp(double);
double ldexp(double, int);
double log(double);
double log10(double);
double pow(double, double);
double sqrt(double);
double fmod(double, double);

int __isinff (float x);
int __isinfd (double x);
int __isnanf (float x);
int __isnand (double x);
int __fpclassifyf (float x);
int __fpclassifyd (double x);
int __signbitf (float x);
int __signbitd (double x);

double infinity(void);
double nan(const char *);
int finite(double);
double copysign(double, double);
double logb(double);
int ilogb(double);

double asinh(double);
double cbrt(double);
double nextafter(double, double);
double rint(double);
double scalbn(double, int);

double exp2(double);
double scalbln(double, long int);
double tgamma(double);
double nearbyint(double);
long int lrint(double);
long long int llrint(double);
double round(double);
long int lround(double);
long long int llround(double);
double trunc(double);
double remquo(double, double, int *);
double fdim(double, double);
double fmax(double, double);
double fmin(double, double);
double fma(double, double, double);

double log1p(double);
double expm1(double);

double acosh(double);
double atanh(double);
double remainder(double, double);
double gamma(double);
double lgamma(double);
double erf(double);
double erfc(double);
double log2(double);

double hypot(double, double);

float atanf(float);
float cosf(float);
float sinf(float);
float tanf(float);
float tanhf(float);
float frexpf(float, int *);
float modff(float, float *);
float ceilf(float);
float fabsf(float);
float floorf(float);
float acosf(float);
float asinf(float);
float atan2f(float, float);
float coshf(float);
float sinhf(float);
float expf(float);
float ldexpf(float, int);
float logf(float);
float log10f(float);
float powf(float, float);
float sqrtf(float);
float fmodf(float, float);

float exp2f(float);
float scalblnf(float, long int);
float tgammaf(float);
float nearbyintf(float);
long int lrintf(float);
long long llrintf(float);
float roundf(float);
long int lroundf(float);
long long int llroundf(float);
float truncf(float);
float remquof(float, float, int *);
float fdimf(float, float);
float fmaxf(float, float);
float fminf(float, float);
float fmaf(float, float, float);

float infinityf(void);
float nanf(const char *);
int finitef(float);
float copysignf(float, float);
float logbf(float);
int ilogbf(float);

float asinhf(float);
float cbrtf(float);
float nextafterf(float, float);
float rintf(float);
float scalbnf(float, int);
float log1pf(float);
float expm1f(float);

float acoshf(float);
float atanhf(float);
float remainderf(float, float);
float gammaf(float);
float lgammaf(float);
float erff(float);
float erfcf(float);
float log2f(float);
float hypotf(float, float);

long double atanl(long double);
long double cosl(long double);
long double sinl(long double);
long double tanl(long double);
long double tanhl(long double);
long double frexpl(long double value, int *);
long double modfl(long double, long double *);
long double ceill(long double);
long double fabsl(long double);
long double floorl(long double);
long double log1pl(long double);
long double expm1l(long double);
long double acosl(long double);
long double asinl(long double);
long double atan2l(long double, long double);
long double coshl(long double);
long double sinhl(long double);
long double expl(long double);
long double ldexpl(long double, int);
long double logl(long double);
long double log10l(long double);
long double powl(long double, long double);
long double sqrtl(long double);
long double fmodl(long double, long double);
long double hypotl(long double, long double);
long double copysignl(long double, long double);
long double nanl(const char *);
int ilogbl(long double);
long double asinhl(long double);
long double cbrtl(long double);
long double nextafterl(long double, long double);
long double rintl(long double);
long double scalbnl(long double, int);
long double exp2l(long double);
long double scalblnl(long double, long);
long double tgammal(long double);
long double nearbyintl(long double);
long int lrintl(long double);
long long int llrintl(long double);
long double roundl(long double);
long lroundl(long double);
long long int llroundl(long double);
long double truncl(long double);
long double remquol(long double, long double, int *);
long double fdiml(long double, long double);
long double fmaxl(long double, long double);
long double fminl(long double, long double);
long double fmal(long double, long double, long double);
long double acoshl(long double);
long double atanhl(long double);
long double remainderl(long double, long double);
long double lgammal(long double);
long double erfl(long double);
long double erfcl(long double);

double drem(double, double);
void sincos(double, double *, double *);
double gamma_r(double, int *);
double lgamma_r(double, int *);

double y0(double);
double y1(double);
double yn(int, double);
double j0(double);
double j1(double);
double jn(int, double);

float dremf(float, float);
void sincosf(float, float *, float *);
float gammaf_r(float, int *);
float lgammaf_r(float, int *);

float y0f(float);
float y1f(float);
float ynf(int, float);
float j0f(float);
float j1f(float);
float jnf(int, float);

double exp10(double);
double pow10(double);
float exp10f(float);
float pow10f(float);

#ifdef __cplusplus
}
#endif

#endif /* __MATH_H__ */

