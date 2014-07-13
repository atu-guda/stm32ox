#include "stm32f10x_conf.h"
#include <string.h>

#include <ox_console.h>


int pr_d( int d )
{
  char b[INT_STR_SZ_DEC];
  return pr( i2dec( d, b ) );
}

int pr_h( uint32_t d )
{
  char b[INT_STR_SZ_HEX];
  return pr( word2hex( d, b ) );
}

int pr_sd( const char *s, int d )
{
  pr( s );
  pr_d( d );
  return pr( NL );
}

int pr_sh( const char *s, int d )
{
  pr( s );
  pr_h( d );
  return pr( NL );
}

int  default_microrl_exec( int argc, const char * const * argv )
{
  int rc = 0;
  CmdFun f = 0;
  const char *nm = "???";

  for( int i=0; i<CMDS_NMAX; ++i ) {
    if( global_cmds[i].name == 0 ) {
      break;
    }
    if( argv[0][1] == '\0'  &&  argv[0][0] == global_cmds[i].acr ) {
      f = global_cmds[i].fun;
      nm = global_cmds[i].name;
      break;
    }
    if( strcmp( global_cmds[i].name, argv[0])  == 0 ) {
      f = global_cmds[i].fun;
      nm = global_cmds[i].name;
      break;
    }
  }

  if( f != 0 ) {
    pr( NL "=== CMD: \"" ); pr( nm ); pr( "\"" NL );
    delay_ms( 50 );
    rc = f( argc, argv );
    pr_sdx( rc );
  } else {
    pr( "ERR:  Unknown command \"" );  pr( argv[0] );   pr( "\"" NL );
  }
  return 0;
}


