#ifndef _OX_STM32F1XX_USARTCFG_H
#define _OX_STM32F1XX_USARTCFG_H

#ifndef STM32F1
 #error "This file is only for STM32F1xx series"
#endif

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include "ox_common.h"

struct Usart1Conf1 {
  enum {
    base = USART1_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = 0,
    rcc_bits2 = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN,
    rcc_bits3 = 0,
    remap = 0
  };
  static const PinPlace pins[];
};
const PinPlace Usart1Conf1::pins[] = {
  { GPIOA, GPIO_Pin_9  }, // TX
  { GPIOA, GPIO_Pin_10 }, // RX
  { GPIOA, GPIO_Pin_8  }, // CK
  { GPIOA, GPIO_Pin_11 }, // CTS
  { GPIOA, GPIO_Pin_12 }  // RTS
};

struct Usart1Conf2 {
  enum {
    base = USART1_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = 0,
    rcc_bits2 = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN,
    rcc_bits3 = 0,
    remap = 1   // TODO
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 5; }
};
const PinPlace Usart1Conf2::pins[] = {
  { GPIOB, GPIO_Pin_6  }, // TX
  { GPIOB, GPIO_Pin_7  }, // RX
  { GPIOA, GPIO_Pin_8  }, // CK
  { GPIOA, GPIO_Pin_11 }, // CTS
  { GPIOA, GPIO_Pin_12 }  // RTS
};

struct Usart2Conf1 {
  enum {
    base = USART2_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_USART2EN,
    rcc_bits2 = RCC_APB2ENR_IOPAEN,
    rcc_bits3 = 0,
    remap = 0
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 5; }
};
const PinPlace Usart2Conf1::pins[] = {
  { GPIOA, GPIO_Pin_2  }, // TX
  { GPIOA, GPIO_Pin_3  }, // RX
  { GPIOA, GPIO_Pin_4  }, // CK
  { GPIOA, GPIO_Pin_0  }, // CTS
  { GPIOA, GPIO_Pin_1  }  // RTS
};

struct Usart2Conf2 {
  enum {
    base = USART2_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_USART2EN,
    rcc_bits2 = RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN,
    rcc_bits3 = 0,
    remap = 1  // TODO
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 5; }
};
const PinPlace Usart2Conf2::pins[] = {
  { GPIOD, GPIO_Pin_5  }, // TX
  { GPIOD, GPIO_Pin_6  }, // RX
  { GPIOD, GPIO_Pin_7  }, // CK
  { GPIOD, GPIO_Pin_3  }, // CTS
  { GPIOD, GPIO_Pin_4  }  // RTS
};

struct Usart3Conf1 {
  enum {
    base = USART3_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_USART3EN,
    rcc_bits2 = RCC_APB2ENR_IOPBEN,
    rcc_bits3 = 0,
    remap = 0
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 5; }
};
const PinPlace Usart3Conf1::pins[] = {
  { GPIOB, GPIO_Pin_10 }, // TX
  { GPIOB, GPIO_Pin_11 }, // RX
  { GPIOB, GPIO_Pin_12 }, // CK
  { GPIOB, GPIO_Pin_13 }, // CTS
  { GPIOB, GPIO_Pin_14 }  // RTS
};

struct Usart3Conf2 {
  enum {
    base = USART3_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_USART3EN,
    rcc_bits2 = RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN,
    rcc_bits3 = 0,
    remap = 1  // TODO
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 5; }
};
const PinPlace Usart3Conf2::pins[] = {
  { GPIOC, GPIO_Pin_10 }, // TX
  { GPIOC, GPIO_Pin_11 }, // RX
  { GPIOC, GPIO_Pin_12 }, // CK
  { GPIOB, GPIO_Pin_13 }, // CTS
  { GPIOB, GPIO_Pin_14 }  // RTS
};

struct Usart3Conf3 {
  enum {
    base = USART3_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_USART3EN,
    rcc_bits2 = RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN,
    rcc_bits3 = 0,
    remap = 2  // TODO
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 5; }
};
const PinPlace Usart3Conf3::pins[] = {
  { GPIOD, GPIO_Pin_8  }, // TX
  { GPIOD, GPIO_Pin_9  }, // RX
  { GPIOD, GPIO_Pin_10 }, // CK
  { GPIOD, GPIO_Pin_11 }, // CTS
  { GPIOD, GPIO_Pin_12 }  // RTS
};


struct Uart4Conf1 {
  enum {
    base = UART4_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_UART4EN,
    rcc_bits2 = RCC_APB2ENR_IOPCEN,
    rcc_bits3 = 0,
    remap = 0
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 3; }
};
const PinPlace Uart4Conf1::pins[] = {
  { GPIOC, GPIO_Pin_9  }, // TX
  { GPIOC, GPIO_Pin_10 }, // RX
  { 0    , 0           }, // CK
  { 0    , 0           }, // CTS
  { 0    , 0           }  // RTS
};


struct Uart5Conf1 {
  enum {
    base = UART5_BASE,
    rcc_bits0 = 0,
    rcc_bits1 = RCC_APB1ENR_UART5EN,
    rcc_bits2 = RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN,
    rcc_bits3 = 0,
    remap = 0
  };
  static const PinPlace pins[];
  static constexpr bool canMode( int n_pins ) { return n_pins > 0 && n_pins < 3; }
};
const PinPlace Uart5Conf1::pins[] = {
  { GPIOC, GPIO_Pin_12 }, // TX
  { GPIOD, GPIO_Pin_2  }, // RX
  { 0    , 0           }, // CK
  { 0    , 0           }, // CTS
  { 0    , 0           }  // RTS
};

#endif // guard

