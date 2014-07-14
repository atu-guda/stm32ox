/**
  * @file    usbd_desc.c
  * @brief   This file provides the USBD descriptors and string formating method.
  */

#include "stm32f10x_conf.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_conf.h"
#include "usb_regs.h"

// STMicroelectronics
#define USBD_VID                        0x0483

// STM32F407
#define USBD_PID                        0x5740

#define USBD_LANGID_STRING              0x409
#define USBD_MANUFACTURER_STRING        "atuLabs"

#define USBD_PRODUCT_FS_STRING          "STM32 Virtual ComPort in FS Mode"
#define USBD_SERIALNUMBER_FS_STRING     "0.0.1"

#define USBD_CONFIGURATION_FS_STRING    "VCP Config"
#define USBD_INTERFACE_FS_STRING        "VCP Interface"

USBD_DEVICE USR_desc =
{
  USBD_USR_DeviceDescriptor,
  USBD_USR_LangIDStrDescriptor,
  USBD_USR_ManufacturerStrDescriptor,
  USBD_USR_ProductStrDescriptor,
  USBD_USR_SerialStrDescriptor,
  USBD_USR_ConfigStrDescriptor,
  USBD_USR_InterfaceStrDescriptor,

};

/* USB Standard Device Descriptor */
uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] =
{
    0x12,                       /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType  01 */
    0x00, 0x02,                 /* bcdUSB  = 2.0    02 */
    0x00,                       /* bDeviceClass     04 */
    0x00,                       /* bDeviceSubClass  05 */
    0x00,                       /* bDeviceProtocol  06 */
    USB_OTG_MAX_EP0_SIZE,       /* bMaxPacketSize   07 */
    LOBYTE( USBD_VID ),         /* idVendor         08 */
    HIBYTE( USBD_VID ),         /* idVendor         09 */
    LOBYTE( USBD_PID ),         /* idVendor         0A */
    HIBYTE( USBD_PID ),         /* idVendor         0B */
    0x00, 0x02,                 /* bcdDevice rel. 2.00               0C */
    USBD_IDX_MFC_STR,           /* Index of manufacturer  string (1) 0E */
    USBD_IDX_PRODUCT_STR,       /* Index of product string (2)       0F */
    USBD_IDX_SERIAL_STR,        /* Index of serial number string (3) 10 */
    USBD_CFG_MAX_NUM            /* bNumConfigurations (1)            11 */
}; /* USB_DeviceDescriptor */


/* USB Standard Device Descriptor. atu: sz=0x0A ONLY dual HS/FS? */
uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,  0x02,                  /* bcdUSB  = 2.0    02 */
  0x00,                         /* bDeviceClass     04 */
  0x00,                         /* bDeviceSubClass  05 */
  0x00,                         /* bDeviceProtocol  06 */
  0x40,                         /* max other seed sz 07 */
  0x01,                         /* max other seed num 08 */
  0x00                          /* 00 */
};

/* USB Standard Device Descriptor */
uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID] =
{
     USB_SIZ_STRING_LANGID,
     USB_DESC_TYPE_STRING,
     LOBYTE( USBD_LANGID_STRING ),
     HIBYTE( USBD_LANGID_STRING ),
};


/**
* @brief  USBD_USR_DeviceDescriptor
*         return the device descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_DeviceDescriptor( uint8_t speed, uint16_t *length )
{
  *length = sizeof( USBD_DeviceDesc );
  return USBD_DeviceDesc;
}

/**
* @brief  USBD_USR_LangIDStrDescriptor
*         return the LangID string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_LangIDStrDescriptor( uint8_t speed , uint16_t *length )
{
  *length =  sizeof( USBD_LangIDDesc );
  return USBD_LangIDDesc;
}


/**
* @brief  USBD_USR_ProductStrDescriptor
*         return the product string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_ProductStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( (uint8_t*)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_ManufacturerStrDescriptor
*         return the manufacturer string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_ManufacturerStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( (uint8_t*)USBD_MANUFACTURER_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_SerialStrDescriptor
*         return the serial number string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_SerialStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( (uint8_t*)USBD_SERIALNUMBER_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

/**
* @brief  USBD_USR_ConfigStrDescriptor
*         return the configuration string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_ConfigStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( (uint8_t*)USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}


/**
* @brief  USBD_USR_InterfaceStrDescriptor
*         return the interface string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t*  USBD_USR_InterfaceStrDescriptor( uint8_t speed, uint16_t *length )
{
  USBD_GetString( (uint8_t*)USBD_INTERFACE_FS_STRING, USBD_StrDesc, length );
  return USBD_StrDesc;
}

