/**
  * @file    usbd_cdc_vcp.h
  * @brief   Header for usbd_cdc_vcp.c file.
  */
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

// #if defined STM32F1
//  #include <stm32f10x.h>
// #elif defined STM32F2
//  #include <stm32f2xx.h>
// #elif defined STM32F3
//  #include <stm32f30x.h>
// #elif defined STM32F4
//  #include <stm32f4xx.h>
// #else
//  #error "Unsupported MC type"
// #endif // STM32Fxxx


#include <usbd_cdc_core.h>
#include <usbd_conf.h>

extern USBD_Usr_cb_TypeDef USR_cb;

/* Exported typef ------------------------------------------------------------*/
/* The following structures groups all needed parameters to be configured for the
   ComPort. These parameters can modified on the fly by the host through CDC class
   command class requests. */
typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} LINE_CODING;


// #define DEFAULT_CONFIG                  0
// #define OTHER_CONFIG                    1


#endif /* __USBD_CDC_VCP_H */

