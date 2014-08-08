#ifndef _OX_BASE_H
#define _OX_BASE_H

#define UNUSED __attribute__((unused))

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


void taskYieldFun(void);


// timings for bad loop delays TODO: for other too
// for 72MHz
#define T_MKS_MUL    8
#define T_MS_MUL  7979
#define T_S_MUL   10261378

// local board functions
void die4led( uint16_t n );
void delay_ms( uint32_t ms ); // base on vTaskDelay - switch to sheduler
void delay_mcs( uint32_t mcs );
// dumb delay fuctions - loop based - for use w/o timer and for small times
void delay_bad_n( uint32_t n );
void delay_bad_s( uint32_t s );
void delay_bad_ms( uint32_t ms );
void delay_bad_mcs( uint32_t mcs );

// RCC registers for enable devices
enum RCC_Bus { // indexes in RCC_enr
  RCC_Bus0 = 0, RCC_APB1 = 0,
  RCC_Bus1 = 1, RCC_APB2 = 1,
  RCC_Bus2 = 2, RCC_AHB = 2, RCC_AHB1  = 2,
  RCC_Bus3 = 3, RCC_AHB2  = 3,
  RCC_Bus4 = 4, RCC_AHB3  = 4,
  RCC_NBUS = 5
};

// void devPinConf( GPIO_TypeDef* GPIOx, GPIOMode_TypeDef mode, uint16_t pins );

enum PinModeNum {
  pinMode_NONE = 0,
  pinMode_AN,
  pinMode_INF,
  pinMode_IPU,
  pinMode_IPD,
  pinMode_Out_PP,
  pinMode_Out_OD,
  pinMode_AF_PP,
  pinMode_AF_OD,
  pinMode_MAX, // size
  // aliases (default, may be other)
  pinMode_TIM_Capt   = pinMode_INF,
  pinMode_TIM_Out    = pinMode_Out_PP,
  pinMode_USART_TX   = pinMode_AF_PP,
  pinMode_USART_RX   = pinMode_IPU,
  pinMode_SPI_SCK    = pinMode_AF_PP,
  pinMode_SPI_MOSI   = pinMode_AF_PP,
  pinMode_SPI_MISO   = pinMode_INF,
  pinMode_SPI_NSS    = pinMode_INF,
  pinMode_I2C_SCK    = pinMode_AF_OD,
  pinMode_I2C_SDA    = pinMode_AF_OD,
  pinMode_I2C_SMBA   = pinMode_AF_OD
};

#if defined(STM32F1)
const GPIOSpeed_TypeDef GPIO_DFL_Speed = GPIO_Speed_50MHz;


#elif defined(STM32F2)
const GPIOSpeed_TypeDef  GPIO_DFL_Speed = GPIO_Speed_100MHz;

#elif defined(STM32F3)
const GPIOSpeed_TypeDef  GPIO_DFL_Speed = GPIO_Speed_Level_3;

#elif defined(STM32F4)
const GPIOSpeed_TypeDef  GPIO_DFL_Speed = GPIO_High_Speed;
#else
  #error "Unknow MCU type"
#endif
extern GPIO_InitTypeDef GPIO_Modes[pinMode_MAX];


void devPinsConf( GPIO_TypeDef* GPIOx, enum PinModeNum mode_num, uint16_t pins );
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

// ------------------------- Devices (C++ only)---------------------
#ifdef __cplusplus
//
extern reg32 *const RCC_enr[RCC_Bus::RCC_NBUS];

const int max_dev_pins = 8; // May be more? (ETH)

struct PinPlace
{
  GPIO_TypeDef* port;
  uint16_t pin;
};

struct DevConfig {
  uint32_t base;
  uint32_t rcc_bits[RCC_NBUS];
  uint32_t remap;
  PinPlace pins[max_dev_pins];
};


struct DevMode {
  uint32_t mode;
  uint8_t n_req;
  uint8_t pin_num;
  PinModeNum pins[max_dev_pins];
};



class DevBase {
  public:
   DevBase( const DevConfig *dcfg, const DevMode *dmode )
    : cfg(dcfg), mode(dmode) {};
   void initHW();
  protected:
   const DevConfig *cfg;
   const DevMode   *mode;
};


#endif // C++

#endif

