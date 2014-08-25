#ifndef _OX_USBOTGFS_H
#define _OX_USBOTGFS_H

// #if defined STM32F1
//  #include <stm32f10x.h>
// #elif defined STM32F2
//  #include <stm32f2xx.h>
// #elif defined STM32F3
//  #include <stm32f30x.h>
// #elif defined STM32F4
//  #include <stm32f4xx.h>
// #else
//  #error "Undefined MC type"
// #endif // STM32Fxxx

#include <ox_dev.h>
#include <usb_core.h>
#include <usbd_core.h>
#include <usb_dcd_int.h>
// #include <usb_conf.h>

#include <FreeRTOS.h>
#include <queue.h>

extern DevConfig UsbOtgFsConf1;

extern DevMode UsbOtgFsModeOTG;
extern DevMode UsbOtgFsModeVbus;
extern DevMode UsbOtgFsModeDev;


class UsbOtgFs : public DevBase {
  enum {
    IBUF_SZ = 256
  };
  public:
   UsbOtgFs( const DevConfig *dcfg, const DevMode *dmode )
     : DevBase( dcfg, dmode ),
       ibuf( xQueueCreate( IBUF_SZ, 1 ) )
    {
    }
   void initHW();
   void init( USBD_DEVICE *pDevice, USBD_Class_cb_TypeDef *class_cb, USBD_Usr_cb_TypeDef *usr_cb );
   void deInit();
   USB_OTG_CORE_HANDLE* getDev() { return &udev; };

   int sendStr( const char* s );
   int sendBlock( const char* d, int n );
   void recvCallback( uint8_t* buf, uint32_t len );

   //* set callback function, called on every char
   void setOnRecv( void (*new_fun)(char) ) { on_recv = new_fun; }
   //* wait and receive char, make sense, only if "on_recv" is not set.
   int  getChar( int wait_tick = 500 );
   void handleIRQ();
   void task_recv(); // to be called from task (optional)

  protected:
   USB_OTG_CORE_HANDLE udev;
   void (*on_recv)( char c ) = nullptr;
   QueueHandle_t ibuf;
   int wait_rx = 500;
};
extern UsbOtgFs *usbfs_main;

// common declarations
extern "C" {
  void OTG_FS_IRQHandler(void);
  void task_usbotgfscdc_recv( void *prm UNUSED );
}

#define STD_OTG_FS_IRQ( obj ) void OTG_FS_IRQHandler(void) { obj.handleIRQ(); }


#define STD_OTG_FS_RECV_TASK( obj ) \
  void task_usbotgfscdc_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }

#endif
// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc,ox/inc/usb_cdc
