#ifndef _LOCALFUN_H
#define _LOCALFUN_H

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

#define NL "\r\n"
extern int idle_flag;

void on_received_char( char c );

typedef const char *const ccstr;
#define BAD_ADDR ((void*)(0xFFFFFFFF))

// number of user one-char vars
#define N_USER_VARS  ('z' - 'a' + 1 )
// user vars
extern int user_vars[N_USER_VARS];

typedef int (*CmdFun)(int argc, const char * const * argv );

void taskYieldFun();


#define CMDS_NMAX 100
typedef struct _CmdInfo
{
  const char *name; //* full command name
  char acr;         //* acronym of command name, or 0
  CmdFun fun;       //* ptr to command
  const char *hint; //* help hint
} CmdInfo;
extern CmdInfo global_cmds[];

// typedef struct _OutPins
// {
//   GPIO_TypeDef* GPIOx;
//   uint8_t start, n;
//   uint16_t mask; // autocalc on init
// } OutPins;
// extern OutPins *LEDS;

// control common output pins
// int  outpins_init( OutPins *oup ); // w/o RCC now
// void outpins_set( const OutPins *oup, uint16_t v );
// void outpins_on( const OutPins *oup, uint16_t v );
// void outpins_off( const OutPins *oup, uint16_t v );
// void outpins_toggle( const OutPins *oup, uint16_t v );

// the same as above, but for LEDS (set as LEDS = &my_outpins);
// int  leds_init();
// void leds_set( uint16_t v );
// void leds_on(  uint16_t v );
// void leds_off( uint16_t v );
// void leds_toggle( uint16_t v );

// timings for bad loop delays
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
/** init single user button on board (A0) */
// void BOARD_initBtn(void);
// /* return current pure button state */
// uint8_t BOARD_btnState(void);

// general buffers
#define GBUF_SZ 128
extern char gbuf_a[GBUF_SZ];
extern char gbuf_b[GBUF_SZ];
extern int log_buf_idx; // gbuf_b in log place too
void log_add( const char *s );
void log_reset();
void log_print();

extern const char hex_digits[];
extern const char dec_digits[];
// helper function : converts some names and numbers to address, fail = -1 (BAD_ADDR)
char* str2addr( const char *str );
// converts char to hex represenration (2 digits, store to s)
char* char2hex( char c, char *s );
char* word2hex( uint32_t d,  char *s );
// 64/log_2[10] \approx 20
#define INT_STR_SZ 24
// if s == 0 returns ptr to inner static buffer
char* i2dec( int n, char *s );

int pr( const char *s ); // redefine in main to current output
int prl( const char *s, int l ); // redefine in main to current output
int pr_d( int d );
int pr_h( uint32_t d );
#define pr_a(a) pr_h( (uint32_t)(a) )
int pr_sd( const char *s, int d );
int pr_sh( const char *s, int d );
#define pr_sdx(x) pr_sd( " " #x "= ", (uint32_t)(x) );
#define pr_shx(x) pr_sh( " " #x "= ", (uint32_t)(x) );

void dump8( const void *addr, int n );

void print_user_var( int idx );

// common commands:
int cmd_info( int argc, const char * const * argv );
int cmd_echo( int argc, const char * const * argv );
int cmd_help( int argc, const char * const * argv );
int cmd_dump( int argc, const char * const * argv );
int cmd_fill( int argc, const char * const * argv );
int cmd_pvar( int argc, const char * const * argv );
int cmd_svar( int argc, const char * const * argv );
int cmd_die( int argc, const char * const * argv );
int cmd_reboot( int argc, const char * const * argv );

#ifdef __cplusplus
}
#endif

#endif

