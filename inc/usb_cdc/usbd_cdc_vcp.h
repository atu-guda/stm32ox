/**
  * @file    usbd_cdc_vcp.h
  * @brief   Header for usbd_cdc_vcp.c file.
  */
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

#include "stm32f10x.h"


#include <usbd_cdc_core.h>
#include "usbd_conf.h"


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

