#ifndef _OX_BASE_H
#define _OX_BASE_H

#ifdef __cplusplus
 extern "C" {
#endif

#define PORT_BITS 16
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000


typedef __IO uint32_t reg32;
typedef const char *const ccstr;
#define BAD_ADDR ((void*)(0xFFFFFFFF))


void taskYieldFun();


// timings for bad loop delays TODO: for other too
// for 72MHz
#define T_MKS_MUL    8
#define T_MS_MUL  7979
// #define T_MS_MUL  10245
#define T_S_MUL   10261378

// local board functions
void die4led( uint16_t n );
void delay_ms( uint32_t ms ); // base on vTaskDelay - switch to sheduler
void delay_mcs( uint32_t mcs );
// dumb delay fuctions - loop based - for use w/o timer and for small times
void delay_bad_n( uint32_t n );
void delay_bad_s( uint32_t s );
void delay_bad_ms( uint32_t ms );
inline void delay_bad_mcs( uint32_t mcs )
{
  uint32_t n = mcs * T_MKS_MUL;
  for( uint32_t i=0; i<n; ++i ) {
    __asm volatile ( "nop;");
  }
}

/** write some (mask based) bits to port, keep all other */
void GPIO_WriteBits( GPIO_TypeDef* GPIOx, uint16_t PortVal, uint16_t mask );

extern const char hex_digits[];
extern const char dec_digits[];
// 64/log_2[10] \approx 20
#define INT_STR_SZ_DEC 24
#define INT_STR_SZ_HEX 20
// converts char to hex represenration (2 digits+EOL, store to s)
char* char2hex( char c, char *s );
// converts uint32 to hex represenration (8(64=16) digits+EOL, store to s)
char* word2hex( uint32_t d,  char *s );
// 64/log_2[10] \approx 20
#define INT_STR_SZ 24
// if s == 0 returns ptr to inner static buffer
char* i2dec( int n, char *s );


#ifdef __cplusplus
}
#endif

#endif

