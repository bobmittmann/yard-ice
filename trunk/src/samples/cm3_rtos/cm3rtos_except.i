# 1 "cm3rtos_except.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "cm3rtos_except.c"
# 24 "cm3rtos_except.c"
# 1 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h" 1
# 28 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h"
# 1 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 1
# 17 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h"
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-pwr.h" 1
# 148 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-pwr.h"
# 1 "/home/bob/devel/stm32f-dk/include/stdint.h" 1
# 29 "/home/bob/devel/stm32f-dk/include/stdint.h"
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long long int int64_t;


typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;


typedef int intptr_t;
typedef unsigned int uintptr_t;
# 149 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-pwr.h" 2

struct stm32f_pwr {
 volatile uint32_t cr;
 volatile uint32_t csr;
};
# 18 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-syscfg.h" 1
# 160 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-syscfg.h"
struct stm32f_syscfg {
 volatile uint32_t memrmp;
 volatile uint32_t pmc;
 volatile uint32_t exticr1;
 volatile uint32_t exticr2;
 volatile uint32_t exticr3;
 volatile uint32_t exticr4;
 volatile uint32_t res[2];
 volatile uint32_t cmpcr;
};
# 19 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-rcc.h" 1
# 1402 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-rcc.h"
struct stm32f_rcc {
 volatile uint32_t cr;
 volatile uint32_t pllcfgr;
 volatile uint32_t cfgr;
 volatile uint32_t cir;
 volatile uint32_t ahb1rstr;
 volatile uint32_t ahb2rstr;
 volatile uint32_t ahb3rstr;
 uint32_t res0;
 volatile uint32_t apb1rstr;
 volatile uint32_t pb2rstr;
 uint32_t res1[2];
 volatile uint32_t ahb1enr;
 volatile uint32_t ahb2enr;
 volatile uint32_t ahb3enr;
 uint32_t res2;
 volatile uint32_t apb1enr;
 volatile uint32_t apb2enr;
 uint32_t res3[2];
 volatile uint32_t ahb1lpenr;
 volatile uint32_t hb2lpenr;
 volatile uint32_t ahb3lpenr;
 uint32_t res4;
 volatile uint32_t apb1lpenr;
 volatile uint32_t apb2lpenr;
 uint32_t res5[2];
 volatile uint32_t bdcr;
 volatile uint32_t csr;
 uint32_t res6[2];
 volatile uint32_t sscgr;
 volatile uint32_t plli2scfgr;
};
# 20 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-usart.h" 1
# 430 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-usart.h"
struct stm32f_usart {
 volatile uint32_t sr;
 volatile uint32_t dr;
 volatile uint32_t brr;
 volatile uint32_t cr1;
 volatile uint32_t cr2;
 volatile uint32_t cr3;
 volatile uint32_t gtpr;
};
# 21 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-gpio.h" 1
# 83 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-gpio.h"
struct stm32f_gpio {
 volatile uint32_t moder;
 volatile uint32_t otyper;
 volatile uint32_t ospeedr;
 volatile uint32_t pupdr;
 volatile uint32_t idr;
 volatile uint32_t odr;
 volatile uint16_t bsrrl;
 volatile uint16_t bsrrh;
 volatile uint32_t lckr;
 volatile uint32_t afrl;
 volatile uint32_t afrh;
};

static inline void stm32f_gpio_set(struct stm32f_gpio * __gpio, int __pin) {
 __gpio->bsrrl = (1 << (__pin));
};

static inline void stm32f_gpio_clr(struct stm32f_gpio * __gpio, int __pin) {
 __gpio->bsrrh = (1 << (__pin));
};


static inline int stm32f_gpio_status(struct stm32f_gpio * __gpio, int __pin) {
 return __gpio->idr & (1 << __pin);
};
# 22 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-flash.h" 1
# 231 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-flash.h"
struct stm32f_flash {
 volatile uint32_t acr;
 volatile uint32_t keyr;
 volatile uint32_t optkeyr;
 volatile uint32_t sr;
 volatile uint32_t cr;
 volatile uint32_t optcr;
};
# 23 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-dac.h" 1
# 253 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-dac.h"
struct stm32f_dac {
 volatile uint32_t cr;
 volatile uint32_t swtrigr;
 volatile uint32_t dhr12r1;
 volatile uint32_t dhr12l1;
 volatile uint32_t dhr8r1;
 volatile uint32_t dhr12r2;
 volatile uint32_t dhr12l2;
 volatile uint32_t dhr8r2;
 volatile uint32_t dhr12rd;
 volatile uint32_t dhr12ld;
 volatile uint32_t dhr8rd;
 volatile uint32_t dor1;
 volatile uint32_t dor2;
 volatile uint32_t sr;
};
# 24 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-adc.h" 1
# 794 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-adc.h"
struct stm32f_adc {
 volatile uint32_t sr;
 volatile uint32_t cr1;
 volatile uint32_t cr2;
 volatile uint32_t smpr1;

 volatile uint32_t smpr2;
 volatile uint32_t jofr1;
 volatile uint32_t jofr2;
 volatile uint32_t jofr3;

 volatile uint32_t jofr4;
 volatile uint32_t htr;
 volatile uint32_t ltr;
 volatile uint32_t sqr1;

 volatile uint32_t sqr2;
 volatile uint32_t sqr3;
 volatile uint32_t jsqr;
 volatile uint32_t jsdr1;

 volatile uint32_t jsdr2;
 volatile uint32_t jsdr3;
 volatile uint32_t jsdr4;
 volatile uint32_t dr;
};

struct stm32f_adcc {
 volatile uint32_t csr;
 volatile uint32_t ccr;
 volatile uint32_t cdr;
};

static inline void stm32f_adc_smp_set(struct stm32f_adc * adc,
            unsigned int chan, unsigned int val) {
 uint32_t smpr;

 if (chan > 9) {
  smpr = adc->smpr1 & ~(0x7 << (3 * (chan - 10)));
  adc->smpr1 = smpr | (((val) << (3 * (chan - 10))) & (0x7 << (3 * (chan - 10))));
 } else {
  smpr = adc->smpr2 & ~(0x7 << (3 * (chan)));
  adc->smpr2 = smpr | (((val) << (3 * (chan))) & (0x7 << (3 * (chan))));
 }
}
# 25 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-rtc.h" 1
# 732 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-rtc.h"
struct stm32f_rtc {
 volatile uint32_t tr;
 volatile uint32_t dr;
 volatile uint32_t cr;
 volatile uint32_t isr;
 volatile uint32_t prer;
 volatile uint32_t calibr;
 volatile uint32_t alarmar;
 volatile uint32_t alarmbr;
 volatile uint32_t wpr;
 volatile uint32_t reserved0[2];
 volatile uint32_t tstr;
 volatile uint32_t tsdr;
 volatile uint32_t reserved1[2];
 volatile uint32_t tafcr;
 volatile uint32_t reserved2[3];
 union {
  struct {
   volatile uint32_t bk0r;
   volatile uint32_t bk1r;
   volatile uint32_t bk2r;
   volatile uint32_t bk3r;
   volatile uint32_t bk4r;
   volatile uint32_t bk5r;
   volatile uint32_t bk6r;
   volatile uint32_t bk7r;
   volatile uint32_t bk8r;
   volatile uint32_t bk9r;
   volatile uint32_t bk10r;
   volatile uint32_t bk11r;
   volatile uint32_t bk12r;
   volatile uint32_t bk13r;
   volatile uint32_t bk14r;
   volatile uint32_t bk15r;
   volatile uint32_t bk16r;
   volatile uint32_t bk17r;
   volatile uint32_t bk18r;
   volatile uint32_t bk19r;
  };
  volatile uint32_t bkr[20];
 };
};
# 26 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-spi.h" 1
# 416 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-spi.h"
struct stm32f_spi {
 volatile uint32_t cr1;
 volatile uint32_t cr2;
 volatile uint32_t sr;
 volatile uint32_t dr;
 volatile uint32_t crcpr;
 volatile uint32_t rxcrcr;
 volatile uint32_t txcrcr;
 volatile uint32_t i2scfgr;
 volatile uint32_t i2spr;
};
# 27 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-eth.h" 1
# 2235 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-eth.h"
struct stm32f_eth {
 volatile uint32_t maccr;
 volatile uint32_t macffr;
 volatile uint32_t machthr;
 volatile uint32_t machtlr;
 volatile uint32_t macmiiar;
 volatile uint32_t macmiidr;
 volatile uint32_t macfcr;
 volatile uint32_t macvlantr;
 uint32_t reserved0[2];
 volatile uint32_t macrwuffr;
 volatile uint32_t macpmtcsr;
 uint32_t reserved1[2];
 volatile uint32_t macsr;
 volatile uint32_t macimr;
 volatile uint32_t maca0hr;
 volatile uint32_t maca0lr;
 volatile uint32_t maca1hr;
 volatile uint32_t maca1lr;
 volatile uint32_t maca2hr;
 volatile uint32_t maca2lr;
 volatile uint32_t maca3hr;
 volatile uint32_t maca3lr;
 uint32_t reserved2[40];
 volatile uint32_t mmccr;
 volatile uint32_t mmcrir;
 volatile uint32_t mmctir;
 volatile uint32_t mmcrimr;
 volatile uint32_t mmctimr;
 uint32_t reserved3[14];
 volatile uint32_t mmctgfsccr;
 volatile uint32_t mmctgfmsccr;
 uint32_t reserved4[5];
 volatile uint32_t mmctgfcr;
 uint32_t reserved5[10];
 volatile uint32_t mmcrfcecr;
 volatile uint32_t mmcrfaecr;
 uint32_t reserved6[10];
 volatile uint32_t mmcrgufcr;
 uint32_t reserved7[334];
 volatile uint32_t ptptscr;
 volatile uint32_t ptpssir;
 volatile uint32_t ptptshr;
 volatile uint32_t ptptslr;
 volatile uint32_t ptptshur;
 volatile uint32_t ptptslur;
 volatile uint32_t ptptsar;
 volatile uint32_t ptptthr;
 volatile uint32_t ptpttlr;
 volatile uint32_t reserved8;
 volatile uint32_t ptptssr;
 uint32_t reserved9[565];
 volatile uint32_t dmabmr;
 volatile uint32_t dmatpdr;
 volatile uint32_t dmarpdr;
 volatile uint32_t dmardlar;
 volatile uint32_t dmatdlar;
 volatile uint32_t dmasr;
 volatile uint32_t dmaomr;
 volatile uint32_t dmaier;
 volatile uint32_t dmamfbocr;
 volatile uint32_t dmarswtr;
 uint32_t reserved10[8];
 volatile uint32_t dmachtdr;
 volatile uint32_t dmachrdr;
 volatile uint32_t dmachtbar;
 volatile uint32_t dmachrbar;
};
# 28 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-otg_fs.h" 1
# 2735 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-otg_fs.h"
struct stm32f_otg_fs {

 volatile uint32_t gotgctl;
 volatile uint32_t gotgin;
 volatile uint32_t gahbcfg;
 volatile uint32_t gusbcfg;

 volatile uint32_t grstctl;
 volatile uint32_t gintsts;
 volatile uint32_t gintmsk;
 volatile uint32_t grxstsr;

 volatile uint32_t grxstsp;
 volatile uint32_t grxfsiz;
 union {
  volatile uint32_t hnptxfsiz;
  volatile uint32_t dieptxf0;
 };
 volatile uint32_t hnptxsts;

 uint32_t res1[2];
 volatile uint32_t gccfg;
 volatile uint32_t cid;

 uint32_t res2[(0x100 - 0x040) / 4];

 volatile uint32_t hptxfsiz;
 volatile uint32_t dieptxf1;
 volatile uint32_t dieptxf2;
 volatile uint32_t dieptxf3;

 uint32_t res3[(0x400 - 0x110) / 4];

 volatile uint32_t hcfg;
 volatile uint32_t hfir;
 volatile uint32_t hfnum;
 uint32_t res4;

 volatile uint32_t hptxsts;
 volatile uint32_t haint;
 volatile uint32_t haintmsk;

 uint32_t res5[(0x440 - 0x41c) / 4];

 volatile uint32_t hptr;

 uint32_t res6[(0x500 - 0x444) / 4];

 struct {
  volatile uint32_t hcchar;
  uint32_t res1;
  volatile uint32_t hcint;
  volatile uint32_t hcintmsk;
  volatile uint32_t hctsiz;
  uint32_t res2[3];
 } hc[8];

 uint32_t res7[(0x800 - 0x600) / 4];

 volatile uint32_t dcfg;
 volatile uint32_t dctl;
 volatile uint32_t dsts;
 uint32_t res8;

 volatile uint32_t diepmsk;
 volatile uint32_t doepmsk;
 volatile uint32_t daint;
 volatile uint32_t daintmsk;

 uint32_t res9[2];

 volatile uint32_t dvbusdis;
 volatile uint32_t dvbuspulse;

 uint32_t res10;
 volatile uint32_t diepempmsk;

 uint32_t res11[(0x900 - 0x838) / 4];

 struct {
  volatile uint32_t diepctl;
  uint32_t res1;
  volatile uint32_t diepint;
  uint32_t res2;
  volatile uint32_t dieptsiz;
  uint32_t res3;
  volatile uint32_t dtxfsts;
  uint32_t res4;
 } inep[4];

 uint32_t res12[(0xb00 - 0x980) / 4];
 struct {
  volatile uint32_t doepctl;
  uint32_t res1;
  volatile uint32_t doepint;
  uint32_t res2;
  volatile uint32_t doeptsiz;
  uint32_t res3[3];
 } outep[4];

 uint32_t res13[(0xe00 - 0xb80) / 4];

 volatile uint32_t pcgcctl;

 uint32_t res14[(0x1000 - 0xe04) / 4];

 struct {
  union {
   volatile uint32_t pop;
   uint32_t push;
  };
  uint32_t res1[(0x1000 - 4) / 4];
 } dfifo[8];
};
# 29 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-fsmc.h" 1
# 673 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-fsmc.h"
struct stm32f_fsmc {
 volatile uint32_t bcr1;
 volatile uint32_t btr1;
 volatile uint32_t bcr2;
 volatile uint32_t btr2;
 volatile uint32_t bcr3;
 volatile uint32_t btr3;
 volatile uint32_t bcr4;
 volatile uint32_t btr4;

 uint32_t res1[(0x060 - 0x020) / 4];


 volatile uint32_t pcr2;
 volatile uint32_t sr2;
 volatile uint32_t pmem2;
 volatile uint32_t patt2;
 uint32_t res2;
 volatile uint32_t eccr2;
 uint32_t res3[2];


 volatile uint32_t pcr3;
 volatile uint32_t sr3;
 volatile uint32_t pmem3;
 volatile uint32_t patt3;
 uint32_t res4;
 volatile uint32_t eccr3;
 uint32_t res5[2];


 volatile uint32_t pcr4;
 volatile uint32_t sr4;
 volatile uint32_t pmem4;
 volatile uint32_t patt4;
 volatile uint32_t pio4;

 uint32_t res6[(0x104 - 0x0b4) / 4];


 volatile uint32_t bwtr1;
 uint32_t res7;
 volatile uint32_t bwtr2;
 uint32_t res8;
 volatile uint32_t bwtr3;
 uint32_t res9;
 volatile uint32_t bwtr4;
};
# 30 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-tim.h" 1
# 1107 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-tim.h"
struct stm32f_tim {
 volatile uint32_t cr1;
 volatile uint32_t cr2;
 volatile uint32_t smcr;
 volatile uint32_t dier;

 volatile uint32_t sr;
 volatile uint32_t egr;
 volatile uint32_t ccmr1;
 volatile uint32_t ccmr2;

 volatile uint32_t ccer;
 volatile uint32_t cnt;
 volatile uint32_t psc;
 volatile uint32_t arr;

 volatile uint32_t rcr;
 volatile uint32_t ccr1;
 volatile uint32_t ccr2;
 volatile uint32_t ccr3;

 volatile uint32_t ccr4;
 volatile uint32_t bdtr;
 volatile uint32_t dcr;
 volatile uint32_t dmar;

 volatile uint32_t or;
};
# 31 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 1 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-dma.h" 1
# 599 "/home/bob/devel/stm32f-dk/include/stm32f/stm32f-dma.h"
struct stm32f_dma {
 volatile uint32_t lisr;
 volatile uint32_t hisr;
 volatile uint32_t lifcr;
 volatile uint32_t hifcr;
 struct {
  volatile uint32_t cr;
  volatile uint32_t ndtr;
  volatile void * par;
  volatile void * m0ar;
  volatile void * m1ar;
  volatile uint32_t fcr;
 } s[8];
};
# 32 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h" 2
# 250 "/home/bob/devel/stm32f-dk/include/arch/stm32f207.h"
typedef enum {
 STM32F_GPIOA_ID = 0,
 STM32F_GPIOB_ID,
 STM32F_GPIOC_ID,
 STM32F_GPIOD_ID,
 STM32F_GPIOE_ID,
 STM32F_GPIOF_ID,
 STM32F_GPIOG_ID,
 STM32F_GPIOH_ID,
 STM32F_GPIOI_ID
} stmf32_gpio_id_t;
# 29 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h" 2
# 43 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h"
typedef enum {
 STM32F_UART1_ID = 0,
 STM32F_UART2_ID,
 STM32F_UART3_ID,
 STM32F_UART4_ID,
 STM32F_UART5_ID,
 STM32F_UART6_ID
} stmf32_uart_id_t;
# 72 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h"
enum {
 INPUT = 0,
 OUTPUT,
 ALT_FUNC,
 ANALOG
};
# 92 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h"
extern const struct stm32f_gpio * stm32f_gpio_lut[];
# 105 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h"
struct stm32f_gpio_io {
 uint8_t pin:4;
 uint8_t port:4;
} __attribute__((__packed__));

typedef struct stm32f_gpio_io gpio_io_t;




static inline void gpio_set(gpio_io_t __io) {
 struct stm32f_gpio * gpio = ((struct stm32f_gpio *)(0x40020000 + (__io.port)*0x400));
 gpio->bsrrl = (1 << (__io.pin));
};


static inline void gpio_clr(gpio_io_t __io) {
 struct stm32f_gpio * gpio = ((struct stm32f_gpio *)(0x40020000 + (__io.port)*0x400));
 gpio->bsrrh = (1 << (__io.pin));
};


static inline int gpio_status(gpio_io_t __io) {
 struct stm32f_gpio * gpio = ((struct stm32f_gpio *)(0x40020000 + (__io.port)*0x400));
 return (gpio->idr & (1 << __io.pin));
};







int stm32f_gpio_id(struct stm32f_gpio * gpio);

void stm32f_gpio_clock_en(struct stm32f_gpio * gpio);


void stm32f_gpio_output(struct stm32f_gpio * gpio, int port,
      int type, int speed);


void stm32f_gpio_mode(struct stm32f_gpio * gpio, unsigned int port,
       unsigned int mode, unsigned int opt);


void stm32f_gpio_af(struct stm32f_gpio * gpio, int port, int af);




int stm32f_usart_putc(struct stm32f_usart * usart, int c);

int stm32f_usart_getc(struct stm32f_usart * usart, unsigned int msec);

int stm32f_usart_read(struct stm32f_usart * usart, char * buf,
       unsigned int len, unsigned int msec);

int stm32f_usart_write(struct stm32f_usart * usart, const void * buf,
        unsigned int len);

int stm32f_usart_flush(struct stm32f_usart * usart);

int stm32f_usart_init(struct stm32f_usart * us,
       unsigned int baudrate, unsigned int flags);

int stm32f_usart_release(struct stm32f_usart * usart);

struct file * stm32f_usart_open(struct stm32f_usart * us,
        unsigned int baudrate, unsigned int flags);

# 1 "/home/bob/devel/stm32f-dk/include/sys/file.h" 1
# 26 "/home/bob/devel/stm32f-dk/include/sys/file.h"
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
# 177 "/home/bob/devel/stm32f-dk/include/sys/stm32f.h" 2

const struct fileop stm32f_usart_ops;

const struct file stm32f_usart5_file;
# 25 "cm3rtos_except.c" 2
# 1 "/home/bob/devel/stm32f-dk/include/stdio.h" 1
# 27 "/home/bob/devel/stm32f-dk/include/stdio.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 212 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 3 4
typedef unsigned int size_t;
# 28 "/home/bob/devel/stm32f-dk/include/stdio.h" 2
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h" 1 3 4
# 40 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 102 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 29 "/home/bob/devel/stm32f-dk/include/stdio.h" 2

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
# 26 "cm3rtos_except.c" 2
# 1 "/home/bob/devel/stm32f-dk/include/string.h" 1
# 29 "/home/bob/devel/stm32f-dk/include/string.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 30 "/home/bob/devel/stm32f-dk/include/string.h" 2
# 48 "/home/bob/devel/stm32f-dk/include/string.h"
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
# 27 "cm3rtos_except.c" 2
# 1 "/home/bob/devel/stm32f-dk/include/stdlib.h" 1
# 27 "/home/bob/devel/stm32f-dk/include/stdlib.h"
# 1 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 1 3 4
# 150 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 324 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stddef.h" 3 4
typedef unsigned int wchar_t;
# 28 "/home/bob/devel/stm32f-dk/include/stdlib.h" 2





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
# 28 "cm3rtos_except.c" 2
# 1 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
# 335 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h"
struct cm3_context {
 uint32_t r[13];
 uint32_t sp;
 uint32_t lr;
 uint32_t pc;
 uint32_t xpsr;
};







struct cm3_scb {
 volatile uint32_t cpuid;
 volatile uint32_t icsr;
 volatile uint32_t vtor;
 volatile uint32_t aircr;
 volatile uint32_t scr;
 volatile uint32_t ccr;
 volatile uint8_t shp[12];
 volatile uint32_t shcsr;
 volatile uint32_t cfsr;
 volatile uint32_t hfsr;
 volatile uint32_t dfsr;
 volatile uint32_t mmfar;
 volatile uint32_t bfar;
 volatile uint32_t afsr;
 volatile uint32_t pfr[2];
 volatile uint32_t dfr;
 volatile uint32_t adr;
 volatile uint32_t mmfr[4];
 volatile uint32_t isar[5];
};





struct cm3_dcb {
 volatile uint32_t dhcsr;
 volatile uint32_t dcrsr;
 volatile uint32_t dcrdr;
 volatile uint32_t demcr;
};






struct cm3_nvic {
 volatile uint32_t iser[8];
 uint32_t reserved0[24];
 volatile uint32_t icer[8];
 uint32_t rserved1[24];
 volatile uint32_t ispr[8];
 uint32_t reserved2[24];
 volatile uint32_t icpr[8];
 uint32_t reserved3[24];
 volatile uint32_t iabr[8];
 uint32_t reserved4[56];
 volatile uint8_t ip[240];
 uint32_t reserved5[644];
 volatile uint32_t stir;
};





struct cm3_systick {
 volatile uint32_t ctrl;
 volatile uint32_t load;
 volatile uint32_t val;
 volatile uint32_t calib;
};




struct cm3_mpu {
 volatile uint32_t type;
 volatile uint32_t ctrl;
 volatile uint32_t rnr;
 volatile uint32_t rbar;
 volatile uint32_t rasr;
 volatile uint32_t rbar_a1;
 volatile uint32_t rasr_a1;
 volatile uint32_t rbar_a2;
 volatile uint32_t rasr_a2;
 volatile uint32_t rbar_a3;
 volatile uint32_t rasr_a3;
};





struct cm3_itm {
 union {
  volatile uint8_t u8;
  volatile uint16_t u16;
  volatile uint32_t u32;
 } port[32];
 uint32_t reserved0[864];
 volatile uint32_t ter;
 uint32_t reserved1[15];
 volatile uint32_t tpr;
 uint32_t reserved2[15];
 volatile uint32_t tcr;
 uint32_t reserved3[29];
 volatile uint32_t iwr;
 volatile uint32_t irr;
 volatile uint32_t imcr;
 uint32_t reserved4[43];
 volatile uint32_t lar;
 volatile uint32_t lsr;
 uint32_t reserved5[6];
 volatile uint32_t pid4;
 volatile uint32_t pid5;
 volatile uint32_t pid6;
 volatile uint32_t pid7;
 volatile uint32_t pid0;
 volatile uint32_t pid1;
 volatile uint32_t pid2;
 volatile uint32_t pid3;
 volatile uint32_t cid0;
 volatile uint32_t cid1;
 volatile uint32_t cid2;
 volatile uint32_t cid3;
};
# 484 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h"
static inline void cm3_irq_enable(int irq) {
 ((struct cm3_nvic *) ((0xe000e000) + 0x0100))->iser[irq >> 5] = (1 << (irq & 0x1f));
}


static inline void cm3_irq_disable(int irq) {
 ((struct cm3_nvic *) ((0xe000e000) + 0x0100))->icer[irq >> 5] = (1 << (irq & 0x1f));
}

static inline int cm3_irq_is_enabled(int irq) {
 return (((struct cm3_nvic *) ((0xe000e000) + 0x0100))->icer[irq >> 5] & (1 << (irq & 0x1f))) ? 1 : 0;
}


static inline int cm3_irq_pend_get(int irq) {
 return (((struct cm3_nvic *) ((0xe000e000) + 0x0100))->ispr[irq >> 5] & (1 << (irq & 0x1f))) ? 1 : 0;
}


static inline void cm3_irq_pend_set(int irq) {
 ((struct cm3_nvic *) ((0xe000e000) + 0x0100))->ispr[irq >> 5] = (1 << (irq & 0x1f));
}


static inline void cm3_irq_pend_clr(int irq) {
 ((struct cm3_nvic *) ((0xe000e000) + 0x0100))->icpr[irq >> 5] = (1 << (irq & 0x1f));
}


static inline int cm3_irq_act_get(int irq) {
 return (((struct cm3_nvic *) ((0xe000e000) + 0x0100))->iabr[irq >> 5] & (1 << (irq & 0x1f))) ? 1 : 0;
}


static inline void cm3_irq_pri_set(int irq, int pri) {
 ((struct cm3_nvic *) ((0xe000e000) + 0x0100))->ip[irq] = pri;
}


static inline int cm3_irq_pri_get(int irq) {
 return ((struct cm3_nvic *) ((0xe000e000) + 0x0100))->ip[irq];
}


static inline void cm3_except_pri_set(int xcpt, int pri) {
 ((struct cm3_scb *) ((0xe000e000) + 0x0d00))->shp[xcpt - 4] = pri;
}


static inline int cm3_except_pri_get(int xcpt) {
 return ((struct cm3_scb *) ((0xe000e000) + 0x0d00))->shp[xcpt - 4];
}







static inline void cm3_control_set(uint32_t value) {
 asm volatile ("msr CONTROL, %0\n" : : "r" (value));
}

static inline void cm3_basepri_set(uint32_t value) {
 asm volatile ("msr BASEPRI, %0\n" : : "r" (value));
}

static inline uint32_t cm3_ipsr_get(void) {
 uint32_t ipsr;
 asm volatile ("mrs %0, IPSR\n" : "=r" (ipsr));
 return ipsr;
}

static void inline __attribute__((always_inline)) cm3_cpsid_i(void) {
 asm volatile ("cpsid i\n");
}

static void inline __attribute__((always_inline)) cm3_cpsie_i(void) {
 asm volatile ("cpsie i\n");
}


static void inline __attribute__((always_inline)) cm3_cpsid_f(void) {
 asm volatile ("cpsid f\n");
}


static void inline __attribute__((always_inline)) cm3_cpsie_f(void) {
 asm volatile ("cpsie f\n");
}

static inline void __attribute__((always_inline))
 cm3_primask_set(uint32_t value) {
 asm volatile ("msr PRIMASK, %0\n" : : "r" (value));
}

static inline uint32_t __attribute__((always_inline)) cm3_primask_get(void) {
 uint32_t value;
 asm volatile ("mrs %0, PRIMASK\n" : "=r" (value));
 return value;
}

static inline void __attribute__((always_inline))
 cm3_faultmask_set(uint32_t value) {
 asm volatile ("msr FAULTMASK, %0\n" : : "r" (value));
}

static inline uint32_t __attribute__((always_inline)) cm3_faultmask_get(void) {
 uint32_t value;
 asm volatile ("mrs %0, FAULTMASK\n" : "=r" (value));
 return value;
}

static inline void cm3_msp_set(uint32_t value) {
 asm volatile ("msr MSP, %0\n" : : "r" (value));
}

static inline uint32_t cm3_msp_get(void) {
 uint32_t msp;
 asm volatile ("mrs %0, MSP\n" : "=r" (msp));
 return msp;
}

static inline void cm3_psp_set(uint32_t value) {
 asm volatile ("msr PSP, %0\n" : : "r" (value));
}

static inline uint32_t cm3_psp_get(void) {
 uint32_t psp;
 asm volatile ("mrs %0, PSP\n" : "=r" (psp));
 return psp;
}

static inline uint32_t cm3_sp_get(void) {
 register uint32_t sp;
 asm volatile ("mov %0, sp\n" : "=r" (sp));
 return sp;
}

static inline uint32_t __attribute__((always_inline)) cm3_clz(uint32_t val) {
 register uint32_t ret;
 asm volatile ("clz %0, %1\n" : "=r" (ret) : "r" (val));
 return ret;
}

static inline uint32_t __attribute__((always_inline)) cm3_rbit(uint32_t val) {
 register uint32_t ret;
 asm volatile ("rbit %0, %1\n" : "=r" (ret) : "r" (val));
 return ret;
}

static inline uint32_t cm3_ldrexw(uint32_t *addr) {
 register uint32_t ret;
 asm volatile ("ldrex %0, [%1]\n" : "=r" (ret) : "r" (addr));
 return ret;
}

static inline uint32_t cm3_strexw(uint32_t *addr, uint32_t val) {
   register uint32_t ret;
   asm volatile ("strex %0, %2, [%1]\n" : "=r" (ret) : "r" (addr), "r" (val) );
   return ret;
}



void cm3_udelay_calibrate(void);
# 29 "cm3rtos_except.c" 2
# 1 "/home/bob/devel/stm32f-dk/include/sys/serial.h" 1
# 30 "cm3rtos_except.c" 2
# 1 "/home/bob/devel/stm32f-dk/include/sys/delay.h" 1
# 30 "/home/bob/devel/stm32f-dk/include/sys/delay.h"
void udelay_calibrate(unsigned int ticks1ms,
       unsigned int (* get_ticks)(void));

void udelay(unsigned int usec);

void delay(unsigned int sec);
# 31 "cm3rtos_except.c" 2

# 1 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos.h" 1
# 26 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos.h"
# 1 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos_svc.h" 1
# 27 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos.h" 2
# 44 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos.h"
static inline void cm3rtos_sleep(unsigned int ms) {
 ( { register int ret asm("r0"); register int r0 asm("r0") = (int)(ms); asm volatile ("svc " "4" "\n" : "=r"(ret) : "0"(r0) : ); ret; } );
}

static inline int cm3rtos_join(unsigned int id) {
 return ( { register int ret asm("r0"); register int r0 asm("r0") = (int)(id); asm volatile ("svc " "5" "\n" : "=r"(ret) : "0"(r0) : ); ret; } );
}

static inline int cm3rtos_cancel(unsigned int id, int code) {
 return ( { register int ret asm("r0"); register int r0 asm("r0") = (int)(id); register int r1 asm("r1") = (int)(code); asm volatile ("svc " "3" "\n" : "=r"(ret) : "0"(r0), "r"(r1) : ); ret; } );
}

static inline int cm3rtos_thread_create(int (* task)(void *),
          void * arg, void * stack_ptr,
          unsigned int stack_size,
          int priority) {
 return ( { register int ret asm("r0"); register int r0 asm("r0") = (int)(task); register int r1 asm("r1") = (int)(arg); register int r2 asm("r2") = (int)(stack_ptr); register int r3 asm("r3") = (int)(stack_size); register int r12 asm("r12") = (int)(priority); asm volatile ("svc " "2" "\n" : "=r"(ret) : "0"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r12) : ); ret; } )
                                      ;
}

static inline void cm3rtos_yield(void) {
 struct cm3_scb * scb = ((struct cm3_scb *) ((0xe000e000) + 0x0d00));
 scb->icsr = (1 << 28);
 asm volatile ("dsb\n");
}

int cm3rtos_init(void);

uint32_t cm3rtos_idle_val(void);

int cm3rtos_thread_create(int (* task)(void *),
        void * arg, void * stack_ptr,
        unsigned int stack_size,
        int priority);

int cm3rtos_thread_id(void);
void cm3rtos_yield(void);

int cm3rtos_cancel(unsigned int id, int code);
int cm3rtos_join(unsigned int id);
void cm3rtos_sleep(unsigned int ms);
# 33 "cm3rtos_except.c" 2

# 1 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos_sys.h" 1
# 36 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos_sys.h"
struct cm3rtos_context {

 uint32_t r4;
 uint32_t r5;
 uint32_t r6;
 uint32_t r7;

 uint32_t r8;
 uint32_t r9;
 uint32_t r10;
 uint32_t r11;

 uint32_t r0;
 uint32_t r1;
 uint32_t r2;
 uint32_t r3;

 uint32_t r12;
 uint32_t lr;
 uint32_t pc;
 uint32_t xpsr;
};





struct cm3rtos_thread_init {
 void * task;
 void * arg;
 void * stack_ptr;
 uint32_t stack_size;
 int32_t priority;
};





struct cm3rtos_idle {
 union {
  struct cm3rtos_context ctx;
  struct {
   uint32_t r[12];
   uint32_t val;
   uint32_t * ptr;
  } snapshot;
 };
} __attribute__ ((aligned (8)));

struct cm3rtos_except_and_idle {
 uint32_t res[64];
 union {
  struct cm3rtos_context ctx;
  struct {
   uint32_t r[12];
   uint32_t val;
   uint32_t * ptr;
  } snapshot;
 };
} __attribute__ ((aligned (8)));
# 107 "/home/bob/devel/stm32f-dk/samples/cm3_rtos/cm3rtos_sys.h"
void cm3rtos_idle_task(void);



void cm3_msp_init(uint64_t * stack_top);






static int inline bmp_bit_rd(void * bmp, unsigned int bit)
{

 uint32_t * bit_addr = ((uint32_t *) (((((uint32_t)bmp) - 0x20000000 + 0x01100000) * 32) + (4 * (0))));
 return bit_addr[bit];
}


static void inline bmp_bit_wr(void * bmp, unsigned int bit, int val)
{

 uint32_t * bit_addr = ((uint32_t *) (((((uint32_t)bmp) - 0x20000000 + 0x01100000) * 32) + (4 * (0))));
 bit_addr[bit] = val;
}



static void inline bmp_bit_set(void * bmp, unsigned int bit)
{

 uint32_t * bit_addr = ((uint32_t *) (((((uint32_t)bmp) - 0x20000000 + 0x01100000) * 32) + (4 * (0))));
 bit_addr[bit] = 1;
}


static void inline bmp_bit_clr(void * bmp, unsigned int bit)
{

 uint32_t * bit_addr = ((uint32_t *) (((((uint32_t)bmp) - 0x20000000 + 0x01100000) * 32) + (4 * (0))));
 bit_addr[bit] = 0;
}

static int inline bmp_ffs32(void * bmp)
{
 return cm3_clz(*(uint32_t *)bmp);
}


static void inline __attribute__((always_inline)) cm3rtos_defer_sched(void) {
 struct cm3_scb * scb = ((struct cm3_scb *) ((0xe000e000) + 0x0d00));

 scb->icsr = (1 << 28);
}

static inline int cm3rtos_alloc_lo(uint32_t * ptr)
{
 int idx;

 idx = cm3_clz(cm3_rbit(~*ptr));
 if (idx == 32)
  return -1;

 bmp_bit_set(ptr, idx);
 return idx;
}

static inline int cm3rtos_alloc_hi(uint32_t * ptr)
{
 int idx;

 idx = 31 - cm3_clz(~*ptr);
 if (idx < 0)
  return -1;

 bmp_bit_set(ptr, idx);
 return idx;
}
# 35 "cm3rtos_except.c" 2




extern uint64_t cm3rtos_except_stack[];

void __attribute__((naked, noreturn)) cm3_hard_fault_isr(void)
{
 struct cm3_scb * scb = ((struct cm3_scb *) ((0xe000e000) + 0x0d00));
 uint32_t hfsr;

 cm3_faultmask_set(1);
 cm3_msp_set((uint32_t)&cm3rtos_except_stack);

 printf("---\n");
 printf("Hard fault:");

 hfsr = scb->hfsr;

 if (hfsr & (1 << 31))
  printf(" DEBUGEVT");
 if (hfsr & (1 << 30))
  printf(" FORCED");
 if (hfsr & (1 << 1))
  printf(" VECTTBL");

 printf("\n");

 for(;;);
}

void __attribute__((naked, noreturn)) cm3_bus_fault_isr(void)
{
 struct cm3_scb * scb = ((struct cm3_scb *) ((0xe000e000) + 0x0d00));
 uint32_t bfsr;

 cm3_faultmask_set(1);
 cm3_msp_set((uint32_t)&cm3rtos_except_stack);

 printf("---\n");
 printf("Bus fault:");

 bfsr = (((scb->cfsr) >> 8) & 0xff);

 if (bfsr & (1 << 7))
  printf(" BFARVALID");
 if (bfsr & (1 << 5))
  printf(" LSPERR");
 if (bfsr & (1 << 4))
  printf(" STKERR");
 if (bfsr & (1 << 3))
  printf(" INVPC");
 if (bfsr & (1 << 2))
  printf(" IMPRECISERR");
 if (bfsr & (1 << 1))
  printf(" PRECISERR");
 if (bfsr & (1 << 0))
  printf(" IBUSERR");
 printf("\n");

 if (bfsr & (1 << 7)) {
  printf(" * ADDR = 0x%08x\n", scb->bfar);
 }

 for(;;);
}

void __attribute__((naked, noreturn)) cm3_usage_fault_isr(void)
{
 struct cm3_scb * scb = ((struct cm3_scb *) ((0xe000e000) + 0x0d00));
 uint32_t ufsr;

 cm3_faultmask_set(1);
 cm3_msp_set((uint32_t)&cm3rtos_except_stack);

 printf("---\n");
 printf("Usage fault:");

 ufsr = (((scb->cfsr) >> 16) & 0xffff);
 if (ufsr & (1 << 9))
  printf(" DIVBYZERO");
 if (ufsr & (1 << 8))
  printf(" UNALIGNED");
 if (ufsr & (1 << 3))
  printf(" NOCP");
 if (ufsr & (1 << 2))
  printf(" INVPC");
 if (ufsr & (1 << 1))
  printf(" INVSTATE");
 if (ufsr & (1 << 0))
  printf(" UNDEFINSTR");

 printf("\n");

 for(;;);
}
