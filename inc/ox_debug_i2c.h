#ifndef _OX_DEBUG_I2C_H
#define _OX_DEBUG_I2C_H

#include <ox_i2c.h>
#include <ox_console.h>
#include <ox_debug1.h>

extern DevI2C &i2c_dbg;


int cmd_i2c_scan( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_SCAN;
int cmd_i2c_send( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_SEND;
int cmd_i2c_send_r1( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_SEND_R1;
int cmd_i2c_send_r2( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_SEND_R2;
int cmd_i2c_recv( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_RECV;
int cmd_i2c_recv_r1( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_RECV_R1;
int cmd_i2c_recv_r2( int argc, const char * const * argv );
extern CmdInfo CMDINFO_I2C_RECV_R2;
#define DEBUG_I2C_CMDS \
  &CMDINFO_I2C_SCAN, \
  &CMDINFO_I2C_SEND, \
  &CMDINFO_I2C_SEND_R1, \
  &CMDINFO_I2C_SEND_R2, \
  &CMDINFO_I2C_RECV, \
  &CMDINFO_I2C_RECV_R1, \
  &CMDINFO_I2C_RECV_R2

void i2c_print_status( DevI2C &i2c );

#endif

