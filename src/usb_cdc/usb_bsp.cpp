/**
  * @file    usb_bsp.c
  * @brief   This file is responsible to offer board support package and is
  *          configurable by user.
  */

#include <usb_bsp.h>
#include <usbd_conf.h>
#include <usbd_cdc_core.h>

#include <ox_base.h>
#include <ox_usbotgfs.h>
// debug
// #include <ox_gpio.h>


/**
* @brief  USB_OTG_BSP_Init
*         Initilizes BSP configurations
* @param  pdev ???
* @retval None
*/
void USB_OTG_BSP_Init( USB_OTG_CORE_HANDLE *pdev UNUSED )
{
  if( usbfs_main ) {
    usbfs_main->initHW();
    // leds_on( 0x10 );
  }
}


/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enabele USB Global interrupt
* @param  pdev ?
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt( USB_OTG_CORE_HANDLE *pdev UNUSED )
{
  NVIC_InitTypeDef nvic;
  // log_add( "BSP_EI " );

  nvic.NVIC_IRQChannel = OTG_FS_IRQn; // 67
  nvic.NVIC_IRQChannelPreemptionPriority = configKERNEL_INTERRUPT_PRIORITY;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &nvic );
}

/**
* @brief  USB_OTG_BSP_uDelay
*         This function provides delay time in micro sec
* @param  usec : Value of delay required in micro sec
* TODO: use os
*/
void USB_OTG_BSP_uDelay( const uint32_t usec )
{
  delay_bad_mcs( usec );
}


/**
* @brief  USB_OTG_BSP_mDelay
*          This function provides delay time in milli sec
* @param  msec : Value of delay required in milli sec
* TODO: use os
*/
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
  delay_ms( msec );
  // USB_OTG_BSP_uDelay( msec * 1000 );
}

