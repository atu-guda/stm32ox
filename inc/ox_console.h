#ifndef _OX_CONSOLE_H
#define _OX_CONSOLE_H

#include <ox_dev.h>


#ifdef __cplusplus
 extern "C" {
#endif

#define NL "\r\n"
extern int idle_flag;
// define to transfer char to console master (microrl?)
void on_received_char( char c );
typedef int (*CmdFun)(int argc, const char * const * argv );

#define CMDS_NMAX 100
typedef struct _CmdInfo
{
  const char *name; //* full command name
  char acr;         //* acronym of command name, or 0
  CmdFun fun;       //* ptr to command
  const char *hint; //* help hint
} CmdInfo;
extern CmdInfo global_cmds[];

#ifdef MICRORL_USE_QUEUE
  #include <FreeRTOS.h>
  #include <queue.h>
  struct MicroRlCmd {
    CmdFun cmd;
    int argc;
    const char * const * argv; // conly one cmd ot once for now - so can use ptrs
    const char* nm;
  };
  extern QueueHandle_t microrl_cmd_queue;

  #define MICRORL_INIT_QUEUE microrl_cmd_queue = xQueueCreate( 1, sizeof(MicroRlCmd) );

  void task_microrl_cmd( void *prm );
  #define STD_MICRORL_CMD_TASK \
    void task_microrl_cmd( void *prm UNUSED ) { \
      struct MicroRlCmd cmd;  BaseType_t ts; int rc; \
      while(1) {  \
        ts = xQueueReceive( microrl_cmd_queue, &cmd, 2000 ); \
        if( ts == pdTRUE ) { \
          pr( NL "=== CMD: \"" ); pr( cmd.nm ); pr( "\"" NL ); \
          rc = cmd.cmd( cmd.argc, cmd.argv ); \
          delay_ms( 10 ); \
          pr_sdx( rc ); \
        } \
      } \
      vTaskDelete(0); \
    }
#endif

int pr( const char *s ); // redefine in main to current output
int prl( const char *s, int l ); // redefine in main to current output
int pr_d( int d );
int pr_h( uint32_t d );
#define pr_a(a) pr_h( (uint32_t)(a) )
int pr_sd( const char *s, int d );
int pr_sh( const char *s, int d );
#define pr_sdx(x) pr_sd( " " #x "= ", (uint32_t)(x) );
#define pr_shx(x) pr_sh( " " #x "= ", (uint32_t)(x) );

int  default_microrl_exec( int argc, const char * const * argv );


#ifdef __cplusplus
}
#endif

#endif

