PROJ_NAME=bmp085
MCTYPE = STM32F407

LINK=$(CXX)

USE_OX = y
USE_MICRORL = y
USE_FREERTOS = y
USE_USB_OTG = y
USE_USB_DEFAULT_CDC = y
NO_STDLIB = n

# TODO: copy to main location
OXDIR = /home/atu/proj/stm32/ox

CFLAGS_ADD := -DSTM32F40_41xxx -DHSE_VALUE=8000000
#
LDSCRIPT = $(OXDIR)/ld/stm32f407ve_512k_192k_x.ld

SRCS = main.cpp
#SRCS += system_stm32f4xx.c
SRCS += clock_stm32f4xx_08HSE168.c
SRCS += startup_stm32f40xx_x.s
SRCS += ox_base.cpp
SRCS += ox_dev.cpp
SRCS += ox_console.cpp
SRCS += ox_debug1.cpp
SRCS += ox_gpio.cpp
# SRCS += ox_usart.cpp
# SRCS += ox_stm32f4xx_usartcfg.cpp
SRCS += ox_usbotgfs.cpp
SRCS += ox_stm32f4xx_usbotgfscfg.cpp
SRCS += ox_i2c.cpp
SRCS += ox_debug_i2c.cpp
SRCS += ox_stm32f4xx_i2ccfg.cpp
SRCS += ox_bmp085.cpp

# stm32lib sources
# SRCS += stm32f4xx_adc.c
# SRCS += stm32f4xx_can.c
# SRCS += stm32f4xx_crc.c
# SRCS += stm32f4xx_cryp_aes.c
# SRCS += stm32f4xx_cryp.c
# SRCS += stm32f4xx_cryp_des.c
# SRCS += stm32f4xx_cryp_tdes.c
# SRCS += stm32f4xx_dac.c
# SRCS += stm32f4xx_dbgmcu.c
# SRCS += stm32f4xx_dcmi.c
# SRCS += stm32f4xx_dma2d.c
# SRCS += stm32f4xx_dma.c
# SRCS += stm32f4xx_exti.c
# SRCS += stm32f4xx_flash.c
# SRCS += stm32f4xx_fmc.c
# SRCS += stm32f4xx_fsmc.c
SRCS += stm32f4xx_gpio.c
# SRCS += stm32f4xx_hash.c
# SRCS += stm32f4xx_hash_md5.c
# SRCS += stm32f4xx_hash_sha1.c
SRCS += stm32f4xx_i2c.c
# SRCS += stm32f4xx_iwdg.c
# SRCS += stm32f4xx_ltdc.c
SRCS += stm32f4xx_misc.c
# SRCS += stm32f4xx_pwr.c
SRCS += stm32f4xx_rcc.c
# SRCS += stm32f4xx_rng.c
# SRCS += stm32f4xx_rtc.c
# SRCS += stm32f4xx_sai.c
# SRCS += stm32f4xx_sdio.c
# SRCS += stm32f4xx_spi.c
# SRCS += stm32f4xx_syscfg.c
# SRCS += stm32f4xx_tim.c
# SRCS += stm32f4xx_usart.c
# SRCS += stm32f4xx_wwdg.c
# SRCS += stm32f4x7_eth.c

# FreeRTOS: rtos/Source -> /usr/share/FreeRTOS/Source
#SRCS += croutine.c
SRCS += list.c
SRCS += queue.c
SRCS += tasks.c
#SRCS += timers.c
# symlink to Source/portable/GCC/ARM_CM3/port.c <<< change
SRCS += heap_2.c
# symlink to Source/portable/GCC/ARM_CM4F/port.c <<< change
SRCS += port.c
# Beware: portmacro.h is symlink to
#   rtos/Source/portable/GCC/ARM_CM3/portmacro.h or CM4F

# console microrl
SRCS += microrl.c

###################################################

include $(OXDIR)/mk/common_cortex.mk

include $(wildcard $(DEPSDIR)/*.d)
#

