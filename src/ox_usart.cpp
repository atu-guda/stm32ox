#include <ox_usart.h>

PinModeNum  UsartModeAsync_pins[] {
  pinMode_AF_PP, // TX
  pinMode_IPU    // RX
  // no CTS, no RTS, no CK
};

DevMode UsartModeAsync
{
  modeAsync,
  ARR_AND_SZ( UsartModeAsync_pins )
};

PinModeNum  UsartModeHWFC_pins[] {
  pinMode_AF_PP, // TX
  pinMode_IPU,   // RX
  pinMode_IPU,   // CTS
  pinMode_AF_PP  // RTS
  // no CK
};


DevMode UsartModeHWFC
{
  modeHWFC,
  ARR_AND_SZ( UsartModeHWFC_pins )
};



PinModeNum  UsartModeSync_pins[] {
  pinMode_AF_PP, // TX
  pinMode_IPU,   // RX
  pinMode_NONE,  // no CTS
  pinMode_NONE,  // no RTS
  pinMode_AF_PP  // CK
};

DevMode UsartModeSync
{
  modeSync,
  ARR_AND_SZ( UsartModeSync_pins )
};

// ---------------------------------------------------------------------------

void Usart::init()
{
  // TODO: rewrite by hand
  USART_Init( usart, &uitd );
  // TODO: status

  // check
  // CR2
  // CR1
  // CR3
  // BRR
}

void Usart::deInit()
{
  // TODO: rewrite by hand
  USART_DeInit( usart );
}

void Usart::initHW()
{
  DevBase::initHW();
}

void Usart::itConfig( uint16_t it, FunctionalState en )
{
  // TODO: rewrite by hand
  USART_ITConfig( usart, it, en );
}

bool Usart::checkFlag( uint16_t flg )
{
  return ( usart->SR & flg );
}

void Usart::clearFlag( uint16_t flg )
{
  usart->SR = (uint16_t)~flg;
}

ITStatus Usart::getITStatus( uint16_t it )
{
  return USART_GetITStatus( usart, it );
}

void Usart::clearITPendingBit( uint16_t it )
{
  return USART_ClearITPendingBit( usart, it );
}

