#include <array>
#include <ox_usart.h>

using namespace std;

#ifndef STM32F1
 #error "This file is only for STM32F1xx series"
#endif


PinPlace USART1Conf1_pins[]  {
    { GPIOA, GPIO_Pin_9  }, // TX
    { GPIOA, GPIO_Pin_10 }, // RX
    { GPIOA, GPIO_Pin_11 }, // CTS
    { GPIOA, GPIO_Pin_12 }, // RTS
    { GPIOA, GPIO_Pin_8  }  // CK
};

DevConfig USART1Conf1 {
  USART1_BASE,   // base
  { 0, RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN, 0, 0, 0 },
  0, // remap
  ARR_AND_SZ(USART1Conf1_pins),
  USART1_IRQn, 0
};

PinPlace USART1Conf2_pins[]  {
  { GPIOB, GPIO_Pin_6  }, // TX
  { GPIOB, GPIO_Pin_7  }, // RX
  { GPIOA, GPIO_Pin_11 }, // CTS
  { GPIOA, GPIO_Pin_12 }, // RTS
  { GPIOA, GPIO_Pin_8  }  // CK
};

DevConfig USART1Conf2 {
  USART1_BASE,   // base
  { 0, RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN |RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN, 0, 0, 0 },
  AFIO_MAPR_USART1_REMAP, // remap
  ARR_AND_SZ(USART1Conf2_pins),
  USART1_IRQn, 0
};



PinPlace USART2Conf1_pins[]  {
  { GPIOA, GPIO_Pin_2  }, // TX
  { GPIOA, GPIO_Pin_3  }, // RX
  { GPIOA, GPIO_Pin_0  }, // CTS
  { GPIOA, GPIO_Pin_1  }, // RTS
  { GPIOA, GPIO_Pin_4  }  // CK
};

DevConfig USART2Conf1 {
  USART2_BASE,   // base
  { RCC_APB1ENR_USART2EN, RCC_APB2ENR_IOPAEN , 0, 0, 0 },
  0, // remap
  ARR_AND_SZ(USART2Conf1_pins),
  USART2_IRQn, 0
};


PinPlace USART2Conf2_pins[]  {
  { GPIOD, GPIO_Pin_5  }, // TX
  { GPIOD, GPIO_Pin_6  }, // RX
  { GPIOD, GPIO_Pin_3  }, // CTS
  { GPIOD, GPIO_Pin_4  }, // RTS
  { GPIOA, GPIO_Pin_4  }  // CK
};

DevConfig USART2Conf2 {
  USART2_BASE,   // base
  { RCC_APB1ENR_USART2EN, RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN, 0, 0, 0 },
  AFIO_MAPR_USART2_REMAP, // remap
  ARR_AND_SZ(USART2Conf2_pins),
  USART2_IRQn, 0
};


PinPlace USART3Conf1_pins[]  {
  { GPIOB, GPIO_Pin_10 }, // TX
  { GPIOB, GPIO_Pin_11 }, // RX
  { GPIOB, GPIO_Pin_13 }, // CTS
  { GPIOB, GPIO_Pin_14 }, // RTS
  { GPIOB, GPIO_Pin_12 }  // CK
};

DevConfig USART3Conf1 {
  USART3_BASE,   // base
  { RCC_APB1ENR_USART3EN, RCC_APB2ENR_IOPBEN, 0, 0, 0 },
  0, // remap
  ARR_AND_SZ(USART3Conf1_pins),
  USART3_IRQn, 0
};

PinPlace USART3Conf2_pins[]  {
  { GPIOC, GPIO_Pin_10 }, // TX
  { GPIOC, GPIO_Pin_11 }, // RX
  {     0,           0 }, // CTS
  {     0,           0 }, // RTS
  { GPIOC, GPIO_Pin_12 }  // CK
};

DevConfig USART3Conf2 {
  USART3_BASE,   // base
  { RCC_APB1ENR_USART3EN,  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN, 0, 0, 0 },
  AFIO_MAPR_USART3_REMAP, // remap
  ARR_AND_SZ(USART3Conf2_pins),
  USART3_IRQn, 0
};

PinPlace UART4Conf1_pins[]  {
  { GPIOC, GPIO_Pin_10 }, // TX
  { GPIOC, GPIO_Pin_11 }  // RX
};

DevConfig UART4Conf1 {
  UART4_BASE,   // base
  { RCC_APB1ENR_UART4EN, RCC_APB2ENR_IOPCEN, 0, 0, 0 },
  0, // remap
  ARR_AND_SZ(UART4Conf1_pins),
  UART4_IRQn, 0
};

PinPlace UART5Conf1_pins[]  {
  { GPIOC, GPIO_Pin_10 }, // TX
  { GPIOD, GPIO_Pin_2  }  // RX
};

DevConfig UART5Conf1 {
  UART5_BASE,   // base
  { RCC_APB1ENR_UART5EN, RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN, 0, 0, 0 },
  0, // remap
  ARR_AND_SZ(UART5Conf1_pins),
  UART5_IRQn, 0
};




