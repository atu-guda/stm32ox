#include <ox_i2c.h>

#ifndef STM32F4
 #error "This file is only for STM32F4xx series"
#endif

PinPlace I2C1Conf1_pins[]  {
  { GPIOB, GPIO_Pin_6  }, // SCK
  { GPIOB, GPIO_Pin_7  }, // SDA
  { GPIOB, GPIO_Pin_5  }  // SMBA
};

DevConfig I2C1Conf1 {
  I2C1_BASE,   // base
  { RCC_APB1ENR_I2C1EN, 0, RCC_AHB1ENR_GPIOBEN, 0, 0 },
  GPIO_AF_I2C1,
  ARR_AND_SZ( I2C1Conf1_pins ),
  I2C1_EV_IRQn, I2C1_ER_IRQn
};

PinPlace I2C1Conf2_pins[]  {
  { GPIOB, GPIO_Pin_8  }, // SCK
  { GPIOB, GPIO_Pin_9  }, // SDA
  { GPIOB, GPIO_Pin_5  }  // SMBA
};

DevConfig I2C1Conf2 {
  I2C1_BASE,   // base
  { RCC_APB1ENR_I2C1EN, 0, RCC_AHB1ENR_GPIOBEN, 0, 0 },
  GPIO_AF_I2C1,
  ARR_AND_SZ( I2C1Conf2_pins ),
  I2C1_EV_IRQn, I2C1_ER_IRQn
};



PinPlace I2C2Conf1_pins[]  {
  { GPIOB, GPIO_Pin_10  }, // SCK
  { GPIOB, GPIO_Pin_11  }, // SDA
  { GPIOB, GPIO_Pin_12  }  // SMBA
};


DevConfig I2C2Conf1 {
  I2C2_BASE,   // base
  { RCC_APB1ENR_I2C2EN, 0, RCC_AHB1ENR_GPIOBEN, 0, 0 },
  GPIO_AF_I2C2,
  ARR_AND_SZ( I2C2Conf1_pins ),
  I2C2_EV_IRQn, I2C2_ER_IRQn
};



PinPlace I2C3Conf1_pins[]  {
  { GPIOA, GPIO_Pin_8   }, // SCK
  { GPIOC, GPIO_Pin_9   }, // SDA
  { GPIOA, GPIO_Pin_9   }  // SMBA
};


DevConfig I2C3Conf1 {
  I2C3_BASE,   // base
  { RCC_APB1ENR_I2C3EN, 0, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN, 0, 0 },
  GPIO_AF_I2C3,
  ARR_AND_SZ( I2C3Conf1_pins ),
  I2C3_EV_IRQn, I2C3_ER_IRQn
};


