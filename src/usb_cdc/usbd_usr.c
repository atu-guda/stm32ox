/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @brief   This file includes the user application layer
  */

#include "stm32f10x_conf.h"

#include "usbd_usr.h"
#include "usbd_ioreq.h"

#include <ox_base.h>


USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,

  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,
};


/**
* @brief  USBD_USR_Init
*         Displays the message on LCD for host lib initialization
*/
void USBD_USR_Init(void)
{
  // leds_on( 0x02 );
  // log_add( "UI ");
  // pr( __FUNCTION__ ); pr( NL );
  /* Initialize LEDs */

  // LCD_LOG_SetHeader(" USB OTG FS VCP Device");
  // LCD_UsrLog("> USB device library started.\n");
  // LCD_LOG_SetFooter ("     USB Device Library v1.1.0" );
}

/**
* @brief  USBD_USR_DeviceReset
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset( uint8_t speed )
{
  //leds_off( 0x02 );
  // log_add( "UR ");
  // pr( __FUNCTION__ ); pr( NL );
  switch( speed ) {
    case USB_OTG_SPEED_HIGH:
      // LCD_LOG_SetFooter ("     USB Device Library v1.1.0 [HS]" );
      break;

    case USB_OTG_SPEED_FULL:
      // LCD_LOG_SetFooter ("     USB Device Library v1.1.0 [FS]" );
      break;
    default: ;
             // LCD_LOG_SetFooter ("     USB Device Library v1.1.0 [??]" );
  }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConfigured (void)
{
  // log_add( "UC ");
  // leds_set( 0x02 );
  // pr( __FUNCTION__ ); pr( NL );
  // LCD_UsrLog("> VCP Interface configured.\n");
}

/**
* @brief  USBD_USR_DeviceSuspended
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  // log_add( "U- ");
  // pr( __FUNCTION__ ); pr( NL );
  // leds_set( 0x0D );
  // LCD_UsrLog("> USB Device in Suspend Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
  // log_add( "U+ ");
  // pr( __FUNCTION__ ); pr( NL );
  //leds_set( 0x0E );
    // LCD_UsrLog("> USB Device in Idle Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConnected (void)
{
  // log_add( "U= ");
  // leds_set( 0x05 );
  // pr( __FUNCTION__ ); pr( NL );
  // LCD_UsrLog("> USB Device Connected.\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceDisconnected (void)
{
  // log_add( "U: ");
  // leds_set( 0x06 );
  // LCD_UsrLog("> USB Device Disconnected.\n");
}

