/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @brief   Generic media access Layer.
  */

#include "stm32f10x_conf.h"


#include "usbd_cdc_vcp.h"
#include "usb_conf.h"

#include <ox_base.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

LINE_CODING linecoding =
{
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
};


// USART_InitTypeDef USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN
   transfer management. */
extern uint8_t  APP_Rx_Buffer[];  /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     ( uint32_t Cmd, uint8_t* Buf, uint32_t Len );
static uint16_t VCP_DataRx   ( uint8_t* Buf, uint32_t Len );
uint16_t VCP_DataTx   ( uint8_t* Buf, uint32_t Len );

// static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops =
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{
  // atu: more here if req here

  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{
  // log_add( "VCP_DeInit " ); // too often
  // leds_off( 0x10 );
  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl( uint32_t Cmd, uint8_t* Buf, uint32_t Len )
{
  // leds_toggle( 0x20 );
  // log_add( "C." );
  // char buf[20];
  // i2dec( Cmd, buf );
  // log_add( buf ); log_add( ":" );
  // i2dec( Len, buf );
  // log_add( buf ); log_add( " " );

  switch (Cmd)  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING: // 0x20 = 32 (Len=7)
    linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype = Buf[6];
    /* Set the new configuration */
    // VCP_COMConfig(OTHER_CONFIG);
    break;

  case GET_LINE_CODING: // 0x21 = 33 (?)
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype;
    break;

  case SET_CONTROL_LINE_STATE: // 0x22 = 34 (Len=0)
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;

  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t VCP_DataTx( uint8_t* Buf, uint32_t Len )
{
  // leds_on( BIT7 );
  for( uint32_t i=0; i<Len; ++i ) {
    APP_Rx_Buffer[APP_Rx_ptr_in++] = Buf[i]; // USART_ReceiveData(EVAL_COM1);
    //  To avoid buffer overflow
    if( APP_Rx_ptr_in >= APP_RX_DATA_SIZE ) {
       APP_Rx_ptr_in = 0;
    }
  }

  // leds_off( BIT7 );
  return USBD_OK;
}

extern xQueueHandle qh_USB_recv;
/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t VCP_DataRx( uint8_t* Buf, uint32_t Len )
{
  portBASE_TYPE wake = pdFALSE;
  for( uint32_t i=0; i<Len; ++i ) {
    xQueueSendFromISR( qh_USB_recv, Buf, &wake  );
  }
  // user_vars[2]  = Len; // debug
  // user_vars[3] += Len; // debug

  // leds_off( BIT6 );
  if( wake != pdFALSE ) {
    taskYIELD();
  }

  return USBD_OK;
}

/**
  * @brief  VCP_COMConfig
  *         Configure the COM Port with default values or values received from host.
  * @param  Conf: can be DEFAULT_CONFIG to set the default configuration or OTHER_CONFIG
  *         to set a configuration received from the host.
  * @retval None.
  */
//static uint16_t VCP_COMConfig(uint8_t Conf)
//{
// // atu: more here........
  // return USBD_OK;
//}


