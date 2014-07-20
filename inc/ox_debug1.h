#ifndef _OX_DEBUG1_H
#define _OX_DEBUG1_H

#include <ox_console.h>

#ifdef __cplusplus
 extern "C" {
#endif

// number of user one-char vars
#define N_USER_VARS  ('z' - 'a' + 1 )
// user vars
extern int user_vars[N_USER_VARS];

// general buffers
#define GBUF_SZ 256
extern char gbuf_a[GBUF_SZ];
extern char gbuf_b[GBUF_SZ];
extern int log_buf_idx; // gbuf_b in log place too
void log_add( const char *s );
void log_reset();
void log_print();

// helper function : converts some names and numbers to address, fail = -1 (BAD_ADDR)
char* str2addr( const char *str );

void dump8( const void *addr, int n );

void print_user_var( int idx );

// common commands:
int cmd_info( int argc, const char * const * argv );
int cmd_echo( int argc, const char * const * argv );
int cmd_help( int argc, const char * const * argv );
int cmd_dump( int argc, const char * const * argv );
int cmd_fill( int argc, const char * const * argv );
int cmd_pvar( int argc, const char * const * argv );
int cmd_svar( int argc, const char * const * argv );
int cmd_die( int argc, const char * const * argv );
int cmd_reboot( int argc, const char * const * argv );

#ifdef __cplusplus
}
#endif

#endif

