#include <string.h>
#include <stdlib.h>

#include <ox_debug_i2c.h>

int cmd_i2c_scan( int argc, const char * const * argv )
{
  int n = 0;
  uint8_t addr_start = 2, addr_end = 127;
  long l;
  char *eptr;

  if( argc > 1 ) {
    l = strtol( argv[1], &eptr, 0 );
    if( eptr != argv[1] ) {
      addr_start = (uint8_t)(l);
    }
  }

  if( argc > 2 ) {
    l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      addr_end = (uint8_t)(l);
    }
  }

  if( addr_start < 2 ) {
    addr_start = 2;
  }
  if( addr_end <= addr_start ) {
    addr_end = addr_start+1;
  }
  pr( NL "I2C Scan in range [ " ); pr_d( addr_start );
  pr( " ; " ); pr_d( addr_end ); pr( " ] " NL );

  i2c_dbg.init();
  uint32_t ev;

  for( uint8_t addr=addr_start; addr < addr_end; ++addr ) {
    // init_i2c();
    delay_ms( 10 );
    n = i2c_dbg.ping( addr );
    ev = i2c_dbg.getLastEv();
    if( n ) {
      pr_d( addr ); pr( " (0x" ); pr_h( addr ); pr( ") " );
      pr(" : " );      pr_d( n ); pr( NL );
    } else {
      // pr_shx( ev );
    }
  }

  ev  = i2c_dbg.getEv();
  pr_shx( ev );

  pr( NL "I2C scan end." NL );

  return 0;
}

int cmd_i2c_send( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "error: value required" NL );
    return 1;
  }
  uint8_t v = 0;
  long l;
  char *eptr;

  l = strtol( argv[1], &eptr, 0 );
  if( eptr != argv[1] ) {
    v = (uint8_t)(l);
  }

  uint8_t addr = (uint8_t)(user_vars['p'-'a']);

  if( argc > 2 ) {
    l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      addr = (uint8_t)(l);
    }
  }

  pr( NL "I2C Send  " ); pr_d( v );
  pr( " to " ); pr_h( addr ); pr( NL );

  i2c_dbg.init();

  int n = i2c_dbg.send( addr, v );
  pr( "I2C Send Result  " ); pr_d( n ); pr( NL );

  i2c_print_status( i2c_dbg );

  return 0;
}

int cmd_i2c_send_r1( int argc, const char * const * argv )
{
  if( argc < 3 ) {
    pr( "** ERR: reg val required" NL );
    return -1;
  }
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  uint8_t reg = (uint8_t) strtol( argv[1], 0, 0 );
  uint8_t val = (uint8_t) strtol( argv[2], 0, 0 );

  pr( NL "I2C Send  " ); pr_d( val );
  pr( " to " ); pr_h( addr ); pr( ": " ); pr_h( reg ); pr( NL );

  i2c_dbg.init();

  int n = i2c_dbg.send_reg1( addr, reg, &val, 1 );
  pr( "I2C Send Result  r1:" ); pr_d( n ); pr( NL );

  i2c_print_status( i2c_dbg );

  return 0;
}

int cmd_i2c_send_r2( int argc, const char * const * argv )
{
  if( argc < 3 ) {
    pr( "** ERR: reg val required" NL );
    return -1;
  }
  uint8_t  addr = (uint8_t)(user_vars['p'-'a']);
  uint16_t reg = (uint8_t) strtol( argv[1], 0, 0 );
  uint8_t  val = (uint8_t) strtol( argv[2], 0, 0 );

  pr( NL "I2C Send  r2: " ); pr_d( val );
  pr( " to " ); pr_h( addr ); pr( ": " ); pr_h( reg ); pr( NL );

  i2c_dbg.init();

  int n = i2c_dbg.send_reg2( addr, reg, &val, 1 );
  pr( "I2C Send Result  " ); pr_d( n ); pr( NL );

  i2c_print_status( i2c_dbg );

  return 0;
}


int cmd_i2c_recv( int argc, const char * const * argv )
{
  // uint8_t v = 0;
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  long l;
  int r, nr = 1;
  char *eptr;

  if( argc > 1 ) {
    l = strtol( argv[1], &eptr, 0 );
    if( eptr != argv[1] ) {
      addr = (uint8_t)(l);
    }
  }

  if( argc > 2 ) {
    l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      nr = l;
    }
  }

  pr( NL "I2C Recv from " );  pr_h( addr );

  i2c_dbg.init();

  if( nr < 2 ) {
    r = i2c_dbg.recv( addr );
  } else {
    r = i2c_dbg.recv( addr, (uint8_t*)(gbuf_a), nr );
    dump8( gbuf_a, nr );
  }
  pr( "  Recv Result = " ); pr_d( r );
  pr( " 0x" ); pr_h( r ); pr( NL );

  i2c_print_status( i2c_dbg );

  return 0;
}

int cmd_i2c_recv_r1( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "** ERR: reg required" NL );
    return -1;
  }
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  uint8_t reg = (uint8_t) strtol( argv[1], 0, 0 );
  uint16_t n = 1;
  uint8_t val = 0;

  if( argc > 2 ) {
    char *eptr;
    long l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      n = (uint16_t)( l );
    }
    if( n > sizeof(gbuf_a) ) {
      n = sizeof(gbuf_a);
    }
  }

  pr( NL "I2C recv r1 from  " );
  pr_h( addr ); pr( ":" ); pr_h( reg ); pr( " n= " ); pr_d( n );

  i2c_dbg.init();

  int r = i2c_dbg.recv_reg1( addr, reg, (uint8_t*)(gbuf_a), n );
  pr_sdx( r );
  if( n == 1 ) {
    pr_h( val );
  } else {
    dump8( gbuf_a, n );
  }
  pr( NL );
  i2c_print_status( i2c_dbg );

  return 0;
}

void i2c_print_status( DevI2C &i2c )
{
  int nt, err, ev;
  err = i2c.getErr();    pr_sdx( err );
  nt  = i2c.getNTrans(); pr_sdx( nt );
  ev  = i2c.getLastEv(); pr_shx( ev );
}

int cmd_i2c_recv_r2( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "** ERR: reg required" NL );
    return -1;
  }
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  uint16_t reg  = (uint8_t) strtol( argv[1], 0, 0 );
  uint16_t n = 1;
  uint8_t val = 0;

  if( argc > 2 ) {
    char *eptr;
    long l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      n = (uint16_t)( l );
    }
    if( n > sizeof(gbuf_a) ) {
      n = sizeof(gbuf_a);
    }
  }

  pr( NL "I2C recv r2 from  " );
  pr_h( addr ); pr( ":" ); pr_h( reg ); pr( " n= " ); pr_d( n );

  i2c_dbg.init();

  int r = i2c_dbg.recv_reg2( addr, reg, (uint8_t*)(gbuf_a), n );
  pr_sdx( r );
  if( n == 1 ) {
    pr_h( val );
  } else {
    dump8( gbuf_a, n );
  }
  pr( NL );
  i2c_print_status( i2c_dbg );

  return 0;
}


