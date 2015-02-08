#include <ox_spi.h>

#ifndef STM32F4
 #error "This file is only for STM32F4xx series"
#endif

PinPlace SPI1Conf1_pins[]  {
  { GPIOA, GPIO_Pin_5  }, // SCK
  { GPIOA, GPIO_Pin_6  }, // MISO
  { GPIOA, GPIO_Pin_7  }, // MOSI
  { GPIOA, GPIO_Pin_4  }  // NSS
};

DevConfig SPI1Conf1 {
  SPI1_BASE,   // base
  { 0, RCC_APB2ENR_SPI1EN, RCC_AHB1ENR_GPIOAEN, 0, 0 },
  GPIO_AF_SPI1,
  ARR_AND_SZ( SPI1Conf1_pins ),
  SPI1_IRQn, 0
};

PinPlace SPI1Conf2_pins[]  {
  { GPIOB, GPIO_Pin_3  }, // SCK  !
  { GPIOB, GPIO_Pin_4  }, // MISO !
  { GPIOB, GPIO_Pin_5  }, // MOSI
  { GPIOA, GPIO_Pin_15 }  // NSS
};

DevConfig SPI1Conf2 {
  SPI1_BASE,   // base
  { 0, RCC_APB2ENR_SPI1EN, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN, 0, 0 },
  GPIO_AF_SPI1,
  ARR_AND_SZ( SPI1Conf2_pins ),
  SPI1_IRQn, 0
};




PinPlace SPI2Conf1_pins[]  {
  { GPIOB, GPIO_Pin_10 }, // SCK
  { GPIOC, GPIO_Pin_2  }, // MISO
  { GPIOC, GPIO_Pin_3  }, // MOSI
  { GPIOB, GPIO_Pin_12 }  // NSS
};


DevConfig SPI2Conf1 {
  SPI2_BASE,   // base
  { RCC_APB1ENR_SPI2EN, 0, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN, 0, 0 },
  GPIO_AF_SPI2,
  ARR_AND_SZ( SPI2Conf1_pins ),
  SPI2_IRQn, 0
};



PinPlace SPI2Conf2_pins[]  {
  { GPIOB, GPIO_Pin_13 }, // SCK
  { GPIOB, GPIO_Pin_14 }, // MISO
  { GPIOB, GPIO_Pin_15 }, // MOSI
  { GPIOB, GPIO_Pin_9  }  // NSS
};


DevConfig SPI2Conf2 {
  SPI2_BASE,   // base
  { RCC_APB1ENR_SPI2EN, 0, RCC_AHB1ENR_GPIOBEN, 0, 0 },
  GPIO_AF_SPI2,
  ARR_AND_SZ( SPI2Conf2_pins ),
  SPI2_IRQn, 0
};



PinPlace SPI3Conf1_pins[]  {
  { GPIOC, GPIO_Pin_10 }, // SCK
  { GPIOC, GPIO_Pin_11 }, // MISO
  { GPIOC, GPIO_Pin_12 }, // MOSI
  { GPIOA, GPIO_Pin_4  }  // NSS !
};

DevConfig SPI3Conf1 {
  SPI3_BASE,   // base
  { RCC_APB1ENR_SPI3EN, 0, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN, 0, 0 },
  GPIO_AF_SPI3,
  ARR_AND_SZ( SPI3Conf1_pins ),
  SPI3_IRQn, 0
};


PinPlace SPI3Conf2_pins[]  {
  { GPIOB, GPIO_Pin_3  }, // SCK
  { GPIOB, GPIO_Pin_4  }, // MISO !
  { GPIOB, GPIO_Pin_5  }, // MOSI !
  { GPIOA, GPIO_Pin_15 }  // NSS
};

DevConfig SPI3Conf2 {
  SPI3_BASE,   // base
  { RCC_APB1ENR_SPI3EN, 0, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN, 0, 0 },
  GPIO_AF_SPI3,
  ARR_AND_SZ( SPI3Conf2_pins ),
  SPI3_IRQn, 0
};


