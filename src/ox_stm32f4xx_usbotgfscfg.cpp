#include <ox_usbotgfs.h>


#ifndef STM32F4
 #error "This file is only for STM32F4xx series"
#endif


PinPlace UsbOtgFsConf1_pins[]  {
    { GPIOA, GPIO_Pin_11 }, // D-
    { GPIOA, GPIO_Pin_12 }, // D+
    { GPIOA, GPIO_Pin_9  }, // Vbus
    { GPIOA, GPIO_Pin_10 }  // Id
};

DevConfig UsbOtgFsConf1 {
  USB_OTG_FS_BASE_ADDR,   // base
  { 0, RCC_APB2ENR_SYSCFGEN, RCC_AHB1ENR_GPIOAEN, RCC_AHB2ENR_OTGFSEN, 0 },
  GPIO_AF_OTG1_FS,
  ARR_AND_SZ(UsbOtgFsConf1_pins),
  OTG_FS_IRQn, 0
};



