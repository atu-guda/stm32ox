PROJ_NAME=rtos_leds
MCTYPE = STM32F103

LINK=$(CXX)

USE_OX = y
USE_MICRORL = n
USE_FREERTOS = y
USE_USB_OTG = n
USE_USB_DEFAULT_CDC = n
NO_STDLIB = n

# TODO: copy to main location
OXDIR = /home/atu/proj/stm32/ox

CFLAGS_ADD := -DSTM32F10X_MD -DHSE_VALUE=8000000
#
LDSCRIPT = $(OXDIR)/ld/stm32f103rb_128k_20k_x.ld

SRCS = main.cpp
SRCS += clock_stm32f10x_cl_08HSE072.c
SRCS += startup_stm32f10x_md_x.s
SRCS += ox_base.cpp
SRCS += ox_dev.cpp
#SRCS += ox_console.cpp
#SRCS += ox_debug1.cpp
SRCS += ox_gpio.cpp
#SRCS += ox_i2c.cpp

# stm32lib sources
# SRCS += stm32f10x_adc.c
# SRCS += stm32f10x_bkp.c
# SRCS += stm32f10x_can.c
# SRCS += stm32f10x_cec.c
# SRCS += stm32f10x_crc.c
# SRCS += stm32f10x_dac.c
# SRCS += stm32f10x_dbgmcu.c
# SRCS += stm32f10x_dma.c
# SRCS += stm32f10x_exti.c
# SRCS += stm32f10x_flash.c
# SRCS += stm32f10x_fsmc.c
SRCS += stm32f10x_gpio.c
# SRCS += stm32f10x_i2c.c
# SRCS += stm32f10x_iwdg.c
# SRCS += stm32f10x_misc.c
# SRCS += stm32f10x_pwr.c
SRCS += stm32f10x_rcc.c
# SRCS += stm32f10x_rtc.c
# SRCS += stm32f10x_sdio.c
# SRCS += stm32f10x_spi.c
# SRCS += stm32f10x_tim.c
# SRCS += stm32f10x_usart.c
# SRCS += stm32f10x_wwdg.c

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

###################################################

include $(OXDIR)/mk/common_cortex.mk

include $(wildcard $(DEPSDIR)/*.d)
#

