/**
  * @file    usb_bsp.c
  * @brief   This file is responsible to offer board support package and is
  *          configurable by user.
  */

#include "stm32f10x_conf.h"
#include "FreeRTOSConfig.h"


#include "usb_bsp.h"
#include "usbd_conf.h"
#include <usbd_cdc_core.h>

extern uint32_t USBD_OTG_ISR_Handler( USB_OTG_CORE_HANDLE *pdev );

extern USB_OTG_CORE_HANDLE   USB_OTG_dev; // in main.c


/**
* @brief  USB_OTG_BSP_Init
*         Initilizes BSP configurations
* @param  pdev ???
* @retval None
*/
void USB_OTG_BSP_Init( USB_OTG_CORE_HANDLE *pdev )
{
  // leds_on( 0x10 );
  // log_add( "BSP_I " );
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

  RCC_OTGFSCLKConfig( RCC_OTGFSCLKSource_PLLVCO_Div3 );
  delay_ms( 10 );
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_OTG_FS, ENABLE );
  delay_ms( 10 );
  // leds_on( 0x10 );
  // leds_set( 0x00 );
}


/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enabele USB Global interrupt
* @param  pdev ?
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt( USB_OTG_CORE_HANDLE *pdev )
{
  NVIC_InitTypeDef nvic;
  // log_add( "BSP_EI " );

  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  nvic.NVIC_IRQChannel = OTG_FS_IRQn; // 67
  nvic.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &nvic );
  // leds_on( 0x20 );
  // leds_off( 0x10 );

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
  // uint32_t count = 0;
  // const uint32_t utime = (120 * usec / 7);
  // do {
  //   if ( ++count > utime ) {
  //     return ;
  //   }
  // }
  // while(1);
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
  // delay_bad_ms( msec );
  // USB_OTG_BSP_uDelay( msec * 1000 );
}

/**
  * @brief  This function handles OTG_FS Handler.
  */
void OTG_FS_IRQHandler(void)
{
  // leds_on( BIT4 );
  USBD_OTG_ISR_Handler( &USB_OTG_dev );
  // leds_off( BIT4 );
}

