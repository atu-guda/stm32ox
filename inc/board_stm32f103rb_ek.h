#ifndef _BOARD_STM32F103RB_EK_H
#define _BOARD_STM32F103RB_EK_H

// definition of resoures on atu first STM32F103RB based board
// headers must be included manualy in CPP file

// default LEDS is B8:B9 (inverted)
#define BOARD_N_LEDS 2

#ifdef DEFINED_PinsOutX
  #define BOARD_DEFINE_LEDS PinsOutX<DevGPIOB> leds( 8, BOARD_N_LEDS );
#else
  #define BOARD_DEFINE_LEDS PinsOut leds( &gpio_b, 8, BOARD_N_LEDS );
#endif

#endif
