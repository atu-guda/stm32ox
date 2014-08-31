#ifndef _OX_DEBUG_I2C_H
#define _OX_DEBUG_I2C_H

#include <ox_i2c.h>
#include <ox_console.h>
#include <ox_debug1.h>

extern DevI2C &i2c_dbg;


int cmd_i2c_scan( int argc, const char * const * argv );
int cmd_i2c_send( int argc, const char * const * argv );
int cmd_i2c_send_r1( int argc, const char * const * argv );
int cmd_i2c_send_r2( int argc, const char * const * argv );
int cmd_i2c_recv( int argc, const char * const * argv );
int cmd_i2c_recv_r1( int argc, const char * const * argv );
void i2c_print_status( DevI2C &i2c );
int cmd_i2c_recv_r2( int argc, const char * const * argv );


#endif

