#include "stm32f10x_conf.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stdlib.h>
#include <errno.h>

#include <ox_base.h>


void taskYieldFun()
{
  taskYIELD();
}


void die4led( uint16_t n )
{
  // leds_set( n );
  while(1) {
    delay_bad_ms( 200 );
    // leds_toggle( BIT0 );
  }
}

void GPIO_WriteBits( GPIO_TypeDef* GPIOx, uint16_t PortVal, uint16_t mask )
{
  GPIOx->ODR = ( PortVal & mask ) | ( GPIOx->ODR & (~mask) );
}


void delay_ms( uint32_t ms )
{
  vTaskDelay( ms / portTICK_RATE_MS );
}


void delay_bad_n( uint32_t dly )
{
  for( uint32_t i=0; i<dly; ++i ) {
    __asm volatile ( "nop;");
  }
}

void delay_bad_s( uint32_t s )
{
  uint32_t n = s * T_S_MUL;
  for( uint32_t i=0; i<n; ++i ) {
    __asm volatile ( "nop;");
  }
}

void delay_bad_ms( uint32_t ms )
{
  uint32_t n = ms * T_MS_MUL;
  /*__IO*/
  // for(  uint32_t i = n; i>0; --i ) {
  for(  uint32_t i = 0; i<n; ++i ) {
    __asm volatile ( "nop;");
  }
}

void delay_mcs( uint32_t mcs )
{
  int ms = mcs / 1000;
  int mcs_r = mcs % 1000;
  if( ms )
    delay_ms( ms );
  if( mcs_r )
    delay_bad_mcs( mcs_r );
}




const char hex_digits[] = "0123456789ABCDEFG";
const char dec_digits[] = "0123456789???";



char* char2hex( char c, char *s )
{
  if( s != 0 ) {
    s[0] = hex_digits[ (uint8_t)(c) >> 4 ];
    s[1] = hex_digits[ c & 0x0F ];
    s[2] = 0;
  }
  return s;
}

char* word2hex( uint32_t d,  char *s )
{
  if( s != 0 ) {
    int i;
    for( i=7; i>=0; --i ) {
      s[i] = hex_digits[ d & 0x0F ];
      d >>= 4;
    }
    s[8] = 0;
  }
  return s;
}

char* i2dec( int n, char *s )
{
  static char sbuf[INT_STR_SZ_DEC];
  char tbuf[24];
  unsigned u;
  if( !s ) {
    s = sbuf;
  }
  char *bufptr = s, *tmpptr = tbuf + 1;
  *tbuf = '\0';

  if( n < 0 ){ //  sign
    u = ( (unsigned)(-(1+n)) ) + 1; // INT_MIN handling
    *bufptr++ = '-';
  } else {
    u=n;
  }

  do {
    *tmpptr++ = dec_digits[ u % 10 ];
  } while( u /= 10 );

  while( ( *bufptr++ = *--tmpptr ) != '\0' ) /* NOP */;
  return s;
}


