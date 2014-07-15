#ifndef _BOARD_STM32F107_ATU_X1_H
#define _BOARD_STM32F107_ATU_X1_H

// definition of resoures on atu first STM32F107RB based board
// headers must be included manualy in CPP file

// default LEDS is C0:C3
#define BOARD_N_LEDS 4

#ifdef DEFINED_PinsOutX
  #define BOARD_DEFINE_LEDS PinsOutX<DevGPIOC> leds( 0, BOARD_N_LEDS );
#else
  #define BOARD_DEFINE_LEDS PinsOut leds( &gpio_c, 0, BOARD_N_LEDS );
#endif

#endif
