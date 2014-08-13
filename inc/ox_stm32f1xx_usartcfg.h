#ifndef _OX_STM32F1XX_USARTCFG_H
#define _OX_STM32F1XX_USARTCFG_H

#ifndef STM32F1
 #error "This file is only for STM32F1xx series"
#endif

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include <ox_dev.h>

extern DevConfig USART1Conf1;
extern DevConfig USART1Conf2;
extern DevConfig USART2Conf1;
extern DevConfig USART2Conf2;
extern DevConfig USART3Conf1;
extern DevConfig USART3Conf2;
extern DevConfig UART4Conf1;
extern DevConfig UART5Conf2;


#endif // guard

