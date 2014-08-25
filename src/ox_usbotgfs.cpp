#include <string.h>
#include <ox_usbotgfs.h>
// #include <ox_gpio.h>

extern uint16_t VCP_DataTx   ( uint8_t* Buf, uint32_t Len );

// TODO: f1/f4 difference
PinModeNum  UsbOtgFsModeOTG_pins[] {
  pinMode_AF_PP,    // D-
  pinMode_AF_PP,    // D+
  pinMode_INF,      // Vbus
  pinMode_AF_OD     // Id
};

DevMode UsbOtgFsModeOTG
{
  1,
  ARR_AND_SZ( UsbOtgFsModeOTG_pins )
};

PinModeNum  UsbOtgFsModeVbus_pins[] {
  pinMode_AF_PP,    // D-
  pinMode_AF_PP,    // D+
  pinMode_INF,      // Vbus
  pinMode_NONE      // Id
};

DevMode UsbOtgFsModeVbus
{
  2,
  ARR_AND_SZ( UsbOtgFsModeVbus_pins )
};


PinModeNum  UsbOtgFsModeDev_pins[] {
  pinMode_AF_PP,    // D-
  pinMode_AF_PP,    // D+
  pinMode_NONE,     // Vbus
  pinMode_NONE      // Id
};

DevMode UsbOtgFsModeDev
{
  3,
  ARR_AND_SZ( UsbOtgFsModeDev_pins )
};


// ---------------------------------------------------------------------------

void UsbOtgFs::init( USBD_DEVICE *pDevice,
               USBD_Class_cb_TypeDef *class_cb,
               USBD_Usr_cb_TypeDef *usr_cb )
{
  USBD_DeInit( &udev );

  USBD_Init( &udev, USB_OTG_FS_CORE_ID, pDevice, class_cb, usr_cb );

  xQueueReset( ibuf );
}

void UsbOtgFs::deInit()
{
  USBD_DeInit( &udev );
  xQueueReset( ibuf );
}

void UsbOtgFs::initHW()
{
  DevBase::initHW();

  #if defined STM43F1
    RCC_OTGFSCLKConfig( RCC_OTGFSCLKSource_PLLVCO_Div3 );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_OTG_FS, ENABLE );
  #endif
}


int UsbOtgFs::sendStr( const char* s )
{
  if( !s || !s[0] ) {
    return 0;
  }
  int l = strlen( s ); // long, but simple code
  return sendBlock( s, l );

}

int UsbOtgFs::sendBlock( const char* d, int n )
{
  if( !d  ||  n < 1 ) {
    return 0;
  }
  VCP_DataTx( (uint8_t*)d, n );

  return n;
}

void UsbOtgFs::recvCallback( uint8_t* buf, uint32_t len )
{
  portBASE_TYPE wake = pdFALSE;
  for( uint32_t i=0; i<len; ++i ) {
    xQueueSendFromISR( ibuf, buf+i, &wake  );
  }
  portEND_SWITCHING_ISR( wake );
}

void UsbOtgFs::handleIRQ()
{
  // leds.toggle( BIT3 );
  USBD_OTG_ISR_Handler( &udev );
}

int  UsbOtgFs::getChar( int wait_tick )
{
  char c;
  BaseType_t r = xQueueReceive( ibuf, &c, wait_tick );
  return ( r == pdTRUE ) ? c : -1;
}



void UsbOtgFs::task_recv()
{
  char cr;
  BaseType_t ts = xQueueReceive( ibuf, &cr, wait_rx );
  if( ts == pdTRUE ) {
    if( on_recv != nullptr ) {
      on_recv( cr );
    } // else simply eat char - if not required - dont use this task
  }
}
