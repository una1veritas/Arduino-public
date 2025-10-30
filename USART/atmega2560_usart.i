# 1 "atmega2560_usart.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "atmega2560_usart.c"
# 1 "atmega2560_usart.h" 1



# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdint.h" 1 3 4
# 9 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdint.h" 3 4
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 1 3 4
# 125 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 3 4

# 125 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 3 4
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
# 146 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 3 4
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
# 163 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 3 4
typedef int8_t int_least8_t;




typedef uint8_t uint_least8_t;




typedef int16_t int_least16_t;




typedef uint16_t uint_least16_t;




typedef int32_t int_least32_t;




typedef uint32_t uint_least32_t;







typedef int64_t int_least64_t;






typedef uint64_t uint_least64_t;
# 217 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 3 4
typedef int8_t int_fast8_t;




typedef uint8_t uint_fast8_t;




typedef int16_t int_fast16_t;




typedef uint16_t uint_fast16_t;




typedef int32_t int_fast32_t;




typedef uint32_t uint_fast32_t;







typedef int64_t int_fast64_t;






typedef uint64_t uint_fast64_t;
# 277 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdint.h" 3 4
typedef int64_t intmax_t;




typedef uint64_t uintmax_t;
# 10 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdint.h" 2 3 4
# 5 "atmega2560_usart.h" 2
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 1 3
# 44 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/inttypes.h" 1 3
# 77 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/inttypes.h" 3
typedef int32_t int_farptr_t;



typedef uint32_t uint_farptr_t;
# 45 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 2 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdarg.h" 1 3 4
# 40 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 99 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 46 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 2 3




# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stddef.h" 1 3 4
# 209 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stddef.h" 3 4
typedef unsigned int size_t;
# 51 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 2 3
# 244 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
struct __file {
 char *buf;
 unsigned char unget;
 uint8_t flags;
# 263 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
 int size;
 int len;
 int (*put)(char, struct __file *);
 int (*get)(struct __file *);
 void *udata;
};
# 277 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
typedef struct __file FILE;
# 407 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern struct __file *__iob[];
# 419 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern FILE *fdevopen(int (*__put)(char, FILE*), int (*__get)(FILE*));
# 436 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int fclose(FILE *__stream);
# 610 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int vfprintf(FILE *__stream, const char *__fmt, va_list __ap);





extern int vfprintf_P(FILE *__stream, const char *__fmt, va_list __ap);






extern int fputc(int __c, FILE *__stream);




extern int putc(int __c, FILE *__stream);


extern int putchar(int __c);
# 651 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int printf(const char *__fmt, ...);





extern int printf_P(const char *__fmt, ...);







extern int vprintf(const char *__fmt, va_list __ap);





extern int sprintf(char *__s, const char *__fmt, ...);





extern int sprintf_P(char *__s, const char *__fmt, ...);
# 687 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int snprintf(char *__s, size_t __n, const char *__fmt, ...);





extern int snprintf_P(char *__s, size_t __n, const char *__fmt, ...);





extern int vsprintf(char *__s, const char *__fmt, va_list ap);





extern int vsprintf_P(char *__s, const char *__fmt, va_list ap);
# 715 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int vsnprintf(char *__s, size_t __n, const char *__fmt, va_list ap);





extern int vsnprintf_P(char *__s, size_t __n, const char *__fmt, va_list ap);




extern int fprintf(FILE *__stream, const char *__fmt, ...);





extern int fprintf_P(FILE *__stream, const char *__fmt, ...);






extern int fputs(const char *__str, FILE *__stream);




extern int fputs_P(const char *__str, FILE *__stream);





extern int puts(const char *__str);




extern int puts_P(const char *__str);
# 764 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern size_t fwrite(const void *__ptr, size_t __size, size_t __nmemb,
         FILE *__stream);







extern int fgetc(FILE *__stream);




extern int getc(FILE *__stream);


extern int getchar(void);
# 812 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int ungetc(int __c, FILE *__stream);
# 824 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern char *fgets(char *__str, int __size, FILE *__stream);






extern char *gets(char *__str);
# 842 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern size_t fread(void *__ptr, size_t __size, size_t __nmemb,
        FILE *__stream);




extern void clearerr(FILE *__stream);
# 859 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int feof(FILE *__stream);
# 870 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
extern int ferror(FILE *__stream);






extern int vfscanf(FILE *__stream, const char *__fmt, va_list __ap);




extern int vfscanf_P(FILE *__stream, const char *__fmt, va_list __ap);







extern int fscanf(FILE *__stream, const char *__fmt, ...);




extern int fscanf_P(FILE *__stream, const char *__fmt, ...);






extern int scanf(const char *__fmt, ...);




extern int scanf_P(const char *__fmt, ...);







extern int vscanf(const char *__fmt, va_list __ap);







extern int sscanf(const char *__buf, const char *__fmt, ...);




extern int sscanf_P(const char *__buf, const char *__fmt, ...);
# 940 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/stdio.h" 3
static __inline__ int fflush(FILE *stream __attribute__((unused)))
{
 return 0;
}






__extension__ typedef long long fpos_t;
extern int fgetpos(FILE *stream, fpos_t *pos);
extern FILE *fopen(const char *path, const char *mode);
extern FILE *freopen(const char *path, const char *mode, FILE *stream);
extern FILE *fdopen(int, const char *);
extern int fseek(FILE *stream, long offset, int whence);
extern int fsetpos(FILE *stream, fpos_t *pos);
extern long ftell(FILE *stream);
extern int fileno(FILE *);
extern void perror(const char *s);
extern int remove(const char *pathname);
extern int rename(const char *oldpath, const char *newpath);
extern void rewind(FILE *stream);
extern void setbuf(FILE *stream, char *buf);
extern int setvbuf(FILE *stream, char *buf, int mode, size_t size);
extern FILE *tmpfile(void);
extern char *tmpnam (char *s);
# 6 "atmega2560_usart.h" 2
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/lib/avr-gcc/9/gcc/avr/9.4.0/include/stdbool.h" 1 3 4
# 7 "atmega2560_usart.h" 2








# 14 "atmega2560_usart.h"
void usart0_init(uint32_t baud);
void usart0_tx(uint8_t b);

# 16 "atmega2560_usart.h" 3 4
_Bool 
# 16 "atmega2560_usart.h"
    usart0_tx_ready(void);
uint8_t usart0_rx_blocking(void);
int usart0_getchar_nb(uint8_t *out);
void usart0_send_string(const char *s);


int usart0_putchar_printf(char c, FILE *stream);


void usart0_rx_enable_interrupt(void);
void usart0_rx_disable_interrupt(void);
uint8_t usart0_rx_available(void);


void usart1_init(uint32_t baud);
void usart1_tx(uint8_t b);
# 2 "atmega2560_usart.c" 2
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 1 3
# 99 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/sfr_defs.h" 1 3
# 100 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3
# 174 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/iom2560.h" 1 3
# 38 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/iom2560.h" 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/iomxx0_1.h" 1 3
# 1630 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/iomxx0_1.h" 3
       
# 1631 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/iomxx0_1.h" 3

       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
       
# 39 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/iom2560.h" 2 3
# 175 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3
# 585 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/portpins.h" 1 3
# 586 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3

# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/common.h" 1 3
# 588 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3

# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/version.h" 1 3
# 590 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3






# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/fuse.h" 1 3
# 239 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/fuse.h" 3

# 239 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/fuse.h" 3
typedef struct
{
    unsigned char low;
    unsigned char high;
    unsigned char extended;
} __fuse_t;
# 597 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3


# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/lock.h" 1 3
# 600 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/io.h" 2 3
# 3 "atmega2560_usart.c" 2
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/avr/interrupt.h" 1 3
# 4 "atmega2560_usart.c" 2

# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 1 3
# 45 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay_basic.h" 1 3
# 40 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay_basic.h" 3
static __inline__ void _delay_loop_1(uint8_t __count) __attribute__((__always_inline__));
static __inline__ void _delay_loop_2(uint16_t __count) __attribute__((__always_inline__));
# 80 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay_basic.h" 3
void
_delay_loop_1(uint8_t __count)
{
 __asm__ volatile (
  "1: dec %0" "\n\t"
  "brne 1b"
  : "=r" (__count)
  : "0" (__count)
 );
}
# 102 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay_basic.h" 3
void
_delay_loop_2(uint16_t __count)
{
 __asm__ volatile (
  "1: sbiw %0,1" "\n\t"
  "brne 1b"
  : "=w" (__count)
  : "0" (__count)
 );
}
# 46 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 2 3
# 1 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 1 3
# 144 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern float cosf (float __x);
__attribute__((__const__)) extern double cos (double __x) __asm("cosf");




__attribute__((__const__)) extern float sinf (float __x);
__attribute__((__const__)) extern double sin (double __x) __asm("sinf");




__attribute__((__const__)) extern float tanf (float __x);
__attribute__((__const__)) extern double tan (double __x) __asm("tanf");





static inline float fabsf (float __x)
{
    return __builtin_fabsf (__x);
}

static inline double fabs (double __x)
{
    return __builtin_fabs (__x);
}





__attribute__((__const__)) extern float fmodf (float __x, float __y);
__attribute__((__const__)) extern double fmod (double __x, double __y) __asm("fmodf");
# 192 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
extern float modff (float __x, float *__iptr);


extern double modf (double __x, double *__iptr) __asm("modff");




__attribute__((__const__)) extern float sqrtf (float __x);


__attribute__((__const__)) extern double sqrt (double __x) __asm("sqrtf");




__attribute__((__const__)) extern float cbrtf (float __x);
__attribute__((__const__)) extern double cbrt (double __x) __asm("cbrtf");
# 219 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern float hypotf (float __x, float __y);
__attribute__((__const__)) extern double hypot (double __x, double __y) __asm("hypotf");






__attribute__((__const__)) extern float squaref (float __x);
__attribute__((__const__)) extern double square (double __x) __asm("squaref");





__attribute__((__const__)) extern float floorf (float __x);
__attribute__((__const__)) extern double floor (double __x) __asm("floorf");





__attribute__((__const__)) extern float ceilf (float __x);
__attribute__((__const__)) extern double ceil (double __x) __asm("ceilf");
# 259 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern float frexpf (float __x, int *__pexp);
__attribute__((__const__)) extern double frexp (double __x, int *__pexp) __asm("frexpf");






__attribute__((__const__)) extern float ldexpf (float __x, int __exp);
__attribute__((__const__)) extern double ldexp (double __x, int __exp) __asm("ldexpf");




__attribute__((__const__)) extern float expf (float __x);
__attribute__((__const__)) extern double exp (double __x) __asm("expf");




__attribute__((__const__)) extern float coshf (float __x);
__attribute__((__const__)) extern double cosh (double __x) __asm("coshf");




__attribute__((__const__)) extern float sinhf (float __x);
__attribute__((__const__)) extern double sinh (double __x) __asm("sinhf");




__attribute__((__const__)) extern float tanhf (float __x);
__attribute__((__const__)) extern double tanh (double __x) __asm("tanhf");






__attribute__((__const__)) extern float acosf (float __x);
__attribute__((__const__)) extern double acos (double __x) __asm("acosf");






__attribute__((__const__)) extern float asinf (float __x);
__attribute__((__const__)) extern double asin (double __x) __asm("asinf");





__attribute__((__const__)) extern float atanf (float __x);
__attribute__((__const__)) extern double atan (double __x) __asm("atanf");







__attribute__((__const__)) extern float atan2f (float __y, float __x);
__attribute__((__const__)) extern double atan2 (double __y, double __x) __asm("atan2f");




__attribute__((__const__)) extern float logf (float __x);
__attribute__((__const__)) extern double log (double __x) __asm("logf");




__attribute__((__const__)) extern float log10f (float __x);
__attribute__((__const__)) extern double log10 (double __x) __asm("log10f");




__attribute__((__const__)) extern float powf (float __x, float __y);
__attribute__((__const__)) extern double pow (double __x, double __y) __asm("powf");





__attribute__((__const__)) extern int isnanf (float __x);
__attribute__((__const__)) extern int isnan (double __x) __asm("isnanf");
# 358 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern int isinff (float __x);
__attribute__((__const__)) extern int isinf (double __x) __asm("isinff");





__attribute__((__const__)) static inline int isfinitef (float __x)
{
    unsigned char __exp;
    __asm__ (
 "mov	%0, %C1		\n\t"
 "lsl	%0		\n\t"
 "mov	%0, %D1		\n\t"
 "rol	%0		"
 : "=r" (__exp)
 : "r" (__x) );
    return __exp != 0xff;
}


static inline int isfinite (double __x)
{
    return isfinitef (__x);
}






__attribute__((__const__)) static inline float copysignf (float __x, float __y)
{
    __asm__ (
 "bst	%D2, 7	\n\t"
 "bld	%D0, 7	"
 : "=r" (__x)
 : "0" (__x), "r" (__y) );
    return __x;
}

__attribute__((__const__)) static inline double copysign (double __x, double __y)
{
    __asm__ (
 "bst	%r1+%2-1, 7" "\n\t"
 "bld	%r0+%2-1, 7"
 : "+r" (__x)
 : "r" (__y), "n" (4));
    return __x;
}
# 416 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern int signbitf (float __x);
__attribute__((__const__)) extern int signbit (double __x) __asm("signbitf");





__attribute__((__const__)) extern float fdimf (float __x, float __y);
__attribute__((__const__)) extern double fdim (double __x, double __y) __asm("fdimf");







__attribute__((__const__)) extern float fmaf (float __x, float __y, float __z);
__attribute__((__const__)) extern double fma (double __x, double __y, double __z) __asm("fmaf");






__attribute__((__const__)) extern float fmaxf (float __x, float __y);
__attribute__((__const__)) extern double fmax (double __x, double __y) __asm("fmaxf");






__attribute__((__const__)) extern float fminf (float __x, float __y);
__attribute__((__const__)) extern double fmin (double __x, double __y) __asm("fminf");





__attribute__((__const__)) extern float truncf (float __x);
__attribute__((__const__)) extern double trunc (double __x) __asm("truncf");
# 466 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern float roundf (float __x);
__attribute__((__const__)) extern double round (double __x) __asm("roundf");
# 479 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern long lroundf (float __x);
__attribute__((__const__)) extern long lround (double __x) __asm("lroundf");
# 493 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/math.h" 3
__attribute__((__const__)) extern long lrintf (float __x);
__attribute__((__const__)) extern long lrint (double __x) __asm("lrintf");
# 47 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 2 3
# 86 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
static __inline__ void _delay_us(double __us) __attribute__((__always_inline__));
static __inline__ void _delay_ms(double __ms) __attribute__((__always_inline__));
# 165 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
void
_delay_ms(double __ms)
{
 double __tmp ;



 uint32_t __ticks_dc;
 extern void __builtin_avr_delay_cycles(unsigned long);
 __tmp = ((
# 174 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h"
          16000000UL
# 174 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
               ) / 1e3) * __ms;
# 184 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
  __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));


 __builtin_avr_delay_cycles(__ticks_dc);
# 210 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
}
# 254 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
void
_delay_us(double __us)
{
 double __tmp ;



 uint32_t __ticks_dc;
 extern void __builtin_avr_delay_cycles(unsigned long);
 __tmp = ((
# 263 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h"
          16000000UL
# 263 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
               ) / 1e6) * __us;
# 273 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
  __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));


 __builtin_avr_delay_cycles(__ticks_dc);
# 300 "/opt/homebrew/Cellar/avr-gcc@9/9.4.0_1/avr/include/util/delay.h" 3
}
# 6 "atmega2560_usart.c" 2
# 15 "atmega2560_usart.c"

# 15 "atmega2560_usart.c"
static volatile uint8_t rx_buf[128];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;


void usart0_init(uint32_t baud) {
    uint16_t ubrr = (16000000UL / (16UL * baud)) - 1UL ;


    
# 24 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0xC5)) 
# 24 "atmega2560_usart.c"
          = ((uint8_t*) &ubrr)[0];
    
# 25 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0xC4)) 
# 25 "atmega2560_usart.c"
          = (uint8_t) ubrr;

    
# 27 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0xC2)) 
# 27 "atmega2560_usart.c"
          = (1 << 
# 27 "atmega2560_usart.c" 3
                  2
# 27 "atmega2560_usart.c"
                        ) | (1 << 
# 27 "atmega2560_usart.c" 3
                                  1
# 27 "atmega2560_usart.c"
                                        );

    
# 29 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0XC1)) 
# 29 "atmega2560_usart.c"
          = (1 << 
# 29 "atmega2560_usart.c" 3
                  4
# 29 "atmega2560_usart.c"
                       ) | (1 << 
# 29 "atmega2560_usart.c" 3
                                 3
# 29 "atmega2560_usart.c"
                                      );
}


void usart0_rx_enable_interrupt(void) {
    
# 34 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0XC1)) 
# 34 "atmega2560_usart.c"
          |= (1 << 
# 34 "atmega2560_usart.c" 3
                   7
# 34 "atmega2560_usart.c"
                         );
}


void usart0_rx_disable_interrupt(void) {
    
# 39 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0XC1)) 
# 39 "atmega2560_usart.c"
          &= ~(1 << 
# 39 "atmega2560_usart.c" 3
                    7
# 39 "atmega2560_usart.c"
                          );
}


void usart0_tx(uint8_t b) {
    while (!(
# 44 "atmega2560_usart.c" 3
            (*(volatile uint8_t *)(0xC0)) 
# 44 "atmega2560_usart.c"
                   & (1 << 
# 44 "atmega2560_usart.c" 3
                           5
# 44 "atmega2560_usart.c"
                                )));
    
# 45 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0XC6)) 
# 45 "atmega2560_usart.c"
        = b;
}


# 48 "atmega2560_usart.c" 3 4
_Bool 
# 48 "atmega2560_usart.c"
    usart0_tx_ready(void) {
    return (
# 49 "atmega2560_usart.c" 3
           (*(volatile uint8_t *)(0xC0)) 
# 49 "atmega2560_usart.c"
                  & (1 << 
# 49 "atmega2560_usart.c" 3
                          5
# 49 "atmega2560_usart.c"
                               )) != 0;
}


int usart0_getchar_nb(uint8_t *out) {
    if (rx_head == rx_tail) return 0;
    uint8_t val = rx_buf[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1) & (128 - 1));
    *out = val;
    return 1;
}


uint8_t usart0_rx_blocking(void) {
    uint8_t ch;
    while (!usart0_getchar_nb(&ch));
    return ch;
}

void usart0_send_string(const char *s) {
    while (*s) {
        usart0_tx((uint8_t)*s++);
    }
}


int usart0_putchar_printf(char c, FILE *stream) {
    (void)stream;
    if (c == '\n') usart0_tx('\r');
    usart0_tx((uint8_t)c);
    return 0;
}



# 83 "atmega2560_usart.c" 3
void __vector_25 (void) __attribute__ ((__signal__,__used__, __externally_visible__)) ; void __vector_25 (void) 
# 83 "atmega2560_usart.c"
                   {
    uint8_t data = 
# 84 "atmega2560_usart.c" 3
                  (*(volatile uint8_t *)(0XC6))
# 84 "atmega2560_usart.c"
                      ;
    uint8_t next = (uint8_t)((rx_head + 1) & (128 - 1));
    if (next == rx_tail) {


        rx_tail = (uint8_t)((rx_tail + 1) & (128 - 1));
    }
    rx_buf[rx_head] = data;
    rx_head = next;
}

uint8_t usart0_rx_available(void) {
    return (uint8_t)((128 + rx_head - rx_tail) & (128 - 1));
}



void usart1_init(uint32_t baud) {
    uint16_t ubrr = (uint16_t)((16000000UL / (16UL * baud)) - 1UL);
    
# 103 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0xCD)) 
# 103 "atmega2560_usart.c"
          = (uint8_t)(ubrr >> 8);
    
# 104 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0xCC)) 
# 104 "atmega2560_usart.c"
          = (uint8_t)ubrr;
    
# 105 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0xCA)) 
# 105 "atmega2560_usart.c"
          = (1 << 
# 105 "atmega2560_usart.c" 3
                  2
# 105 "atmega2560_usart.c"
                        ) | (1 << 
# 105 "atmega2560_usart.c" 3
                                  1
# 105 "atmega2560_usart.c"
                                        );
    
# 106 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0XC9)) 
# 106 "atmega2560_usart.c"
          = (1 << 
# 106 "atmega2560_usart.c" 3
                  4
# 106 "atmega2560_usart.c"
                       ) | (1 << 
# 106 "atmega2560_usart.c" 3
                                 3
# 106 "atmega2560_usart.c"
                                      );
}

void usart1_tx(uint8_t b) {
    while (!(
# 110 "atmega2560_usart.c" 3
            (*(volatile uint8_t *)(0xC8)) 
# 110 "atmega2560_usart.c"
                   & (1 << 
# 110 "atmega2560_usart.c" 3
                           5
# 110 "atmega2560_usart.c"
                                )));
    
# 111 "atmega2560_usart.c" 3
   (*(volatile uint8_t *)(0XCE)) 
# 111 "atmega2560_usart.c"
        = b;
}
