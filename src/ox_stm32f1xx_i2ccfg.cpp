#include <ox_i2c.h>

#ifndef STM32F1
 #error "This file is only for STM32F1xx series"
#endif

PinPlace I2C1Conf1_pins[]  {
  { GPIOB, GPIO_Pin_6  }, // SCK
  { GPIOB, GPIO_Pin_7  }, // SDA
  { GPIOB, GPIO_Pin_5  }  // SMBA
};

DevConfig I2C1Conf1 {
  I2C1_BASE,   // base
  { RCC_APB1ENR_I2C1EN, RCC_APB2ENR_IOPBEN, 0, 0, 0 },
  0, // remap
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
  { RCC_APB1ENR_I2C1EN, RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN, 0, 0, 0 },
  AFIO_MAPR_I2C1_REMAP, // remap
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
  { RCC_APB1ENR_I2C2EN, RCC_APB2ENR_IOPBEN, 0, 0, 0 },
  0, // remap
  ARR_AND_SZ( I2C2Conf1_pins ),
  I2C2_EV_IRQn, I2C2_ER_IRQn
};



