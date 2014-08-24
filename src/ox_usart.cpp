#include <string.h>
#include <ox_usart.h>

// for debug
// #include <ox_debug1.h>
// #include <ox_gpio.h>

PinModeNum  UsartModeAsync_pins[] {
  pinMode_USART_TX, // TX
  pinMode_USART_RX  // RX
  // no CTS, no RTS, no CK
};

DevMode UsartModeAsync
{
  modeAsync,
  ARR_AND_SZ( UsartModeAsync_pins )
};

PinModeNum  UsartModeHWFC_pins[] {
  pinMode_USART_TX, // TX
  pinMode_USART_RX, // RX
  pinMode_USART_RX, // CTS
  pinMode_USART_TX  // RTS
  // no CK
};


DevMode UsartModeHWFC
{
  modeHWFC,
  ARR_AND_SZ( UsartModeHWFC_pins )
};



PinModeNum  UsartModeSync_pins[] {
  pinMode_USART_TX, // TX
  pinMode_USART_RX, // RX
  pinMode_NONE,     // no CTS
  pinMode_NONE,     // no RTS
  pinMode_USART_TX  // CK
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
  xQueueReset( ibuf );
  xQueueReset( obuf );
}

void Usart::deInit()
{
  // TODO: rewrite by hand
  USART_DeInit( usart );
  xQueueReset( ibuf );
  xQueueReset( obuf );
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

  int ns = 0, nl = 0;
  for( ; *d; ++d ) {
    while( checkFlag( USART_FLAG_TXE ) == RESET ) {
      taskYieldFun();
      ++nl;
      if( nl >= wait_tx ) {
        return ns;
      }
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

  int ns = 0, sst;

  for( int i=0; i<n; ++i ) { // TODO: FIX to real
    sst = xQueueSend( obuf, &(d[i]), wait_tx );
    if( sst != pdTRUE ) {
      break; // TODO: err status
    }
    ++ns;
  }
  if( ns ) {
    taskYieldFun();
  }

  return ns;
}

void Usart::handleIRQ()
{
  char cr, cs;
  int n_work = 0;
  BaseType_t wake = pdFALSE, qrec;
  uint32_t status = usart->SR;
  // leds.toggle( BIT3 ); // DEBUG

  if( status & USART_FLAG_RXNE ) { // char recived
    ++n_work;
    cr = recvRaw();
    // leds.set( BIT2 );
    if( status & ( USART_FLAG_ORE | USART_FLAG_FE | USART_FLAG_LBD ) ) {
      sr_err = status;
    } else {
      xQueueSendFromISR( ibuf, &cr, &wake  );
    }
    // leds.reset( BIT2 );
  }

  // TXE is keeps on after transmit
  if( on_transmit  &&  (status & USART_FLAG_TXE) ) {
    // leds.set( BIT1 );
    ++n_work;
    qrec = xQueueReceiveFromISR( obuf, &cs, &wake );
    if( qrec == pdTRUE ) {
      sendRaw( cs );
    } else {
      itConfig( USART_IT_TXE, DISABLE );
      on_transmit = false;
    }
    // leds.reset( BIT1 );
  }

  // if( status & USART_FLAG_TC ) {
  //   ++n_work;
  //   // leds_toggle( BIT0 ); // debug// debug TX
  //   usart->SR &= ~USART_FLAG_TC;
  //   // xSemaphoreGiveFromISR( sh_USART_send, &wake );
  // }

  if( n_work == 0 ) { // unhandled
    // leds_toggle( BIT1 );
  }

  portEND_SWITCHING_ISR( wake );

}

int  Usart::getChar( int wait_tick )
{
  char c;
  BaseType_t r = xQueueReceive( ibuf, &c, wait_tick );
  return ( r == pdTRUE ) ? c : -1;
}


void Usart::task_send()
{
  if( on_transmit ) { // handle by IRQ
    return;
  }

  char ct;
  BaseType_t ts = xQueueReceive( obuf, &ct, wait_tx );
  if( ts == pdTRUE ) {
    on_transmit = true;
    sendRaw( ct );
    itConfig( USART_IT_TXE, ENABLE );
  }

}

void Usart::task_recv()
{
  char cr;
  BaseType_t ts = xQueueReceive( ibuf, &cr, wait_rx );
  if( ts == pdTRUE ) {
    if( on_recv != nullptr ) {
      on_recv( cr );
    } // else simply eat char - if not required - dont use this task
  }
}
