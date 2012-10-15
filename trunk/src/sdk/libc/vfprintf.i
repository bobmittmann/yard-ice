# 1 "vfprintf.c"
# 1 "/home/bob/devel/uboot/src/libc//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "vfprintf.c"
# 14 "vfprintf.c"
# 1 "/home/bob/devel/uboot/yard-ice/config.h" 1
# 15 "vfprintf.c" 2


# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h" 1 3 4
# 40 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 102 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 18 "vfprintf.c" 2
# 1 "/home/bob/devel/uboot/include/stdio.h" 1
# 17 "/home/bob/devel/uboot/include/stdio.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 212 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 3 4
typedef unsigned int size_t;
# 18 "/home/bob/devel/uboot/include/stdio.h" 2


struct file;

typedef struct file FILE;

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;







int fgetc(FILE * f);

int fputc(int c, FILE * f);

int fputs(const char * s, FILE * f);

int putchar(int c);

int getchar(void);

int puts(const char * s);

int vfprintf(FILE * f, const char * fmt, va_list ap);

int fprintf(FILE * f, const char * fmt, ...);

int printf(const char * fmt, ...);

int sprintf(char * str, const char * fmt, ...);

int fflush(FILE * f);

int feof(FILE * f);

int fclose(FILE * f);

char * fgets(char * s, int size, FILE * f);

size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE * f);

size_t fread(void * ptr, size_t size, size_t nmemb, FILE * f);





extern inline int putchar(int c) {
 return fputc(c, stdout);
};

extern inline int getchar(void) {
 return fgetc(stdin);
};
# 19 "vfprintf.c" 2
# 1 "/home/bob/devel/uboot/include/string.h" 1
# 17 "/home/bob/devel/uboot/include/string.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 18 "/home/bob/devel/uboot/include/string.h" 2
# 36 "/home/bob/devel/uboot/include/string.h"
long int strtol(const char *nptr, char **endptr, int base);

size_t strlen(const char * s);

char * strcat(char * dst, const char * src);

int strcmp(const char * s1, const char * s2);

char * strchr(const char * s, int c);

char * strcpy(char * dst, const char * src);

void * memcpy(void * dst, const void * src, size_t n);

void * memset(void * s, int c, size_t n);

int memcmp(const void * s1, const void * s2, size_t n);

int ffs(int i);
# 20 "vfprintf.c" 2
# 1 "/home/bob/devel/uboot/include/sys/file.h" 1
# 16 "/home/bob/devel/uboot/include/sys/file.h"
# 1 "/home/bob/devel/uboot/include/sys/types.h" 1
# 17 "/home/bob/devel/uboot/include/sys/types.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 18 "/home/bob/devel/uboot/include/sys/types.h" 2

typedef long ssize_t;
typedef long clock_t;
typedef long time_t;

typedef int clockid_t;

typedef void * ino_t;
typedef void * dev_t;
typedef unsigned short off_t;

typedef unsigned char uid_t;
typedef unsigned char gid_t;
typedef int pid_t;
typedef long key_t;

typedef unsigned short mode_t;
typedef unsigned short nlink_t;
# 17 "/home/bob/devel/uboot/include/sys/file.h" 2

struct file;

struct fileop {
 int (* write)(void * arg, const void * buf, int count);
 int (* read)(void * arg, void * buf, int count);
 int (* flush)(void * arg);
 int (* close)(void * arg);
};

struct file {
 void * data;
 const struct fileop * op;
};
# 21 "vfprintf.c" 2
# 1 "/home/bob/devel/uboot/include/stdlib.h" 1
# 15 "/home/bob/devel/uboot/include/stdlib.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 150 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 324 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 3 4
typedef unsigned int wchar_t;
# 16 "/home/bob/devel/uboot/include/stdlib.h" 2





typedef struct {
 int quot;
 int rem;
} div_t;






char ** __environ(void **) __attribute__ ((const));





long strtol(const char * nptr, char ** endptr, int base);

unsigned long strtoul(const char * nptr, char ** endptr, int base);

int abs(int num);

int atoi(const char *nptr);

long atol(const char *nptr);

int rand(void);

void srand(unsigned int seed);

div_t div(int numer, int denom);

char * getenv(const char *name);

int setenv(const char *name, const char * value, int overwrite);

int unsetenv(const char *name);

int clearenv(void);
# 22 "vfprintf.c" 2



int uint2hex(char * s, unsigned int val);
# 34 "vfprintf.c"
static const char zeros[] = {

 '0', '0', '0', '0', '0', '0', '0', '0',
 '0', '0', '0', '0', '0', '0', '0', '0',
 '0', '0', '0', '0',

 '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', };

static const char blanks[] = {

 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',

 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static inline div_t div10(unsigned int n)
{
 div_t d;
 int x;

 x = (0x33333333 * n + (n >> 3)) >> 29;
 x = (n - ((0x04432210 >> (x << 2)) & 7)) * 0xcccccccd;

 d.quot = x >> 1;
 d.rem = n - ((x >> 1) * 10);

 return d;
}
# 86 "vfprintf.c"
int vfprintf(struct file * f, const char * fmt, va_list ap)
{
 char buf[12];
 int flags;
 int cnt;
 int i;
 int c;
 int w;
 int n;
 union {
  char * cp;
  unsigned int n;
  int i;
 } val;
 div_t d;

 n = 0;
 w = 0;
 cnt = 0;

 val.cp = (char *)fmt;

 for (flags = 0; (c = *fmt++); ) {
  if (flags == 0) {
   if (c == '%') {
    w = 0;
    flags = 0x01;

    if (n) {
     f->op->write(f->data, val.cp, n);
     val.cp = (char *)fmt;
     cnt += n;;
     n = 0;
    }

    continue;
   }


   n++;





   continue;
  }

  if ((c >= '0') && (c <= '9')) {
   if (!(flags & 0x02)) {
    flags |= 0x02;
    if (c == '0') {
     flags |= 0x04;
     continue;
    }
   }

   w = (((w << 2) + w) << 1) + (c - '0');
   continue;
  }


  if (c == '-') {
   flags |= 0x08;
   continue;
  }




  if (c == '+')
   continue;

  if (c == 'c') {
   c = __builtin_va_arg(ap,int);
   goto print_char;
  }

  if (c == 'd') {
   val.i = __builtin_va_arg(ap,int);
   if (val.i < 0) {
    buf[0] = '-';
    val.i = -val.i;
    flags |= 0x10;
    w--;
   }

decimal:

   i = 12;

   if ((d.quot = val.n) == 0) {
    buf[--i] = '0';
   } else {
    while (d.quot) {
     d = div10(d.quot);
     buf[--i] = d.rem + '0';
    }
   }

   val.cp = buf + i;
   n = 12 - i;
   goto print_buf;
  }

  if (c == 'x') {
   val.n = __builtin_va_arg(ap,unsigned int);

hexadecimal:

   n = uint2hex(buf, val.n);
   val.cp = buf;
   goto print_buf;
  }

  if (c == 's') {
   val.cp = __builtin_va_arg(ap,char *);
   n = strlen(val.cp);
   goto print_buf;
  }


  if (c == 'u') {
   val.n = __builtin_va_arg(ap,unsigned int);
   goto decimal;
  }



  if (c == 'p') {
   val.cp = __builtin_va_arg(ap,char *);
   w = 8;
   flags |= 0x04;
   goto hexadecimal;
  }


  flags = 0;
  w = 0;

print_char:
  val.cp = buf;
  buf[0] = c;
  n = 1;

print_buf:

  if (!(flags & 0x08) && (w > n)) {



   if (flags & 0x04) {
    if (flags & 0x10) {
     flags &= ~0x10;
     f->op->write(f->data, buf, 1);
    }
    f->op->write(f->data, zeros, w - n);
   } else {
    f->op->write(f->data, blanks, w - n);
   }
   cnt += w - n;
  }

  if (flags & 0x10)
   f->op->write(f->data, buf, 1);
  f->op->write(f->data, val.cp, n);
  cnt += n;



  if ((flags & 0x08) && (w > n)) {
   f->op->write(f->data, blanks, w - n);
   cnt += w - n;
  }


  flags = 0;
  w = 0;


  val.cp = (char *)fmt;
  n = 0;

 }


 if (n) {
  f->op->write(f->data, val.cp, n);
  cnt+= n;;
 }


 return cnt;
}
