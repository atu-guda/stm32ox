#include <string.h>
#include <ox_usart.h>

// for debug
#include <ox_debug1.h>
#include <ox_gpio.h>

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
  usart->SR &= (uint16_t)~flg;
}

ITStatus Usart::getITStatus( uint16_t it )
{
  return USART_GetITStatus( usart, it );
}

void Usart::clearITPendingBit( uint16_t it )
{
  return USART_ClearITPendingBit( usart, it );
}


int Usart::sendStrLoop( const char* s )
{
  if( !s || !s[0] ) {
    return 0;
  }
  int l = strlen( s ); // long, but simple code
  return sendBlockLoop( s, l );
}

int Usart::sendBlockLoop( const char* d, int n )
{
  if( !d  ||  n < 1 ) {
    return 0;
  }

  int ns = 0;
  for( ; *d; ++d ) {
    while( checkFlag( USART_FLAG_TXE ) == RESET ) {
      taskYieldFun(); // TODO: limit
    }
    sendRaw( *d );
    ++ns;
  }
  return ns;
}

int Usart::sendStr( const char* s )
{
  if( !s || !s[0] ) {
    return 0;
  }
  int l = strlen( s ); // long, but simple code
  return sendBlock( s, l );

}

int Usart::sendBlock( const char* d, int n )
{
  if( !d  ||  n < 1 ) {
    return 0;
  }

  int ns = 0;

  // LOCK

  for( ; *d; ++d ) { // TODO: FIX to real
    int next_s = ( obuf_s + 1 ) % OBUF_SZ;
    if( next_s == obuf_e ) { // overrun
      break;
    }
    obuf[obuf_s] = *d;
    obuf_s = next_s;
    ++ns;
  }

  // UNLOCK
  // inform: start to send
  return ns;
}

void Usart::handleIRQ()
{
  char c;
  int n_work = 0;
  uint32_t status = usart->SR;
  leds.toggle( BIT3 );

  if( status & USART_FLAG_RXNE ) { // char recived
    ++n_work;
    c = recvRaw();
    leds.set( BIT2 );
    if( status & ( USART_FLAG_ORE | USART_FLAG_FE | USART_FLAG_LBD ) ) {
      sr_err = status;
    } else {
      int next_s = ( ibuf_s + 1 ) % IBUF_SZ; // no lock: int irq
      if( next_s != ibuf_e ) {
        ibuf[ibuf_s] = c;
        ibuf_s = next_s;
        // TODO: notify
      }
    }
    leds.reset( BIT2 );
    // leds_toggle( BIT2 ); // debug
    // xQueueSendFromISR( qh_USART_recv, &c, &wake  );
  }

  // TXE is keeps on after transmit
  if( on_transmit  &&  (status & USART_FLAG_TXE) ) {
    leds.set( BIT1 );
    ++n_work;
    obuf_e = ( obuf_e + 1 ) % OBUF_SZ;
    if( obuf_e != obuf_s ) {
      sendRaw( obuf[obuf_e] );
    } else {
      itConfig( USART_IT_TXE, DISABLE );
      on_transmit = false;
    }
    leds.reset( BIT1 );
  }

  if( status & USART_FLAG_TC ) {
    ++n_work;
    // leds_toggle( BIT0 ); // debug// debug TX
    usart->SR &= ~USART_FLAG_TC;
    // xSemaphoreGiveFromISR( sh_USART_send, &wake );
  }

  if( n_work == 0 ) { // unhandled
    // leds_toggle( BIT1 );
  }

}

void Usart::taskIO()
{
  // LOCK
  while( ibuf_s != ibuf_e ) {
    int next_e = ( ibuf_e + 1 ) % IBUF_SZ;
    if( on_recv != nullptr ) {
      on_recv( ibuf[ibuf_e] );
    }
    ibuf_e = next_e;
  }
  // UNLOCK

  if( on_transmit ) { // handle by IRQ
    return;
  }

  // LOCK ?
  if( obuf_s != obuf_e ) {
    on_transmit = true;
    sendRaw( obuf[obuf_e] );
    itConfig( USART_IT_TXE, ENABLE );
  }
  // UNLOCK


}
