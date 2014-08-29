#ifndef _OX_I2C_H
#define _OX_I2C_H

#if defined STM32F1
 #include <stm32f10x_i2c.h>
#elif defined STM32F2
 #include <stm32f2xx_i2c.h>
#elif defined STM32F3
 #include <stm32f30x_i2c.h>
#elif defined STM32F4
 #include <stm32f4xx_i2c.h>
#else
 #error "Undefined MC type"
#endif // STM32Fxxx

// #include <stm32f10x_gpio.h>

#include <ox_dev.h>

extern DevConfig I2C1Conf1;
extern DevConfig I2C1Conf2;
extern DevConfig I2C2Conf1;
extern DevConfig I2C2Conf2;
extern DevConfig I2C3Conf1;
extern DevConfig I2C3Conf2;

extern DevMode I2CModeI2C_Master;
extern DevMode I2CModeSMB_Master;

#define I2C_FLAG_Mask ((uint32_t)0x00FFFFFF)

class DevI2C : public DevBase {
  public:
   enum flags {
     CR1_PE    = 0x0001,
     CR1_START = 0x0100,
     CR1_STOP  = 0x0200,
     CR1_ACK   = 0x0400,
     CR1_ENGC  = 0x0040,
     CR1_SWRST = 0x8000,

   };
   DevI2C( const DevConfig *dconf, const DevMode *dmode,
       uint8_t a_own_addr = 1, uint32_t a_speed = 100000 )
     : DevBase( dconf, dmode ),
       own_addr( a_own_addr), speed( a_speed ),
       i2c( (I2C_TypeDef*)(dconf->base))
    {
    }
   void init();
   void deInit() { I2C_DeInit( i2c ); };
   void initHW();
   I2C_TypeDef* getDev() { return i2c; };
   void enable()     { i2c->CR1 |= (uint16_t)(CR1_PE); };
   void disable()    { i2c->CR1 &= (uint16_t)(~CR1_PE); };
   void genSTART()   { i2c->CR1 |= (uint16_t)(CR1_START); };
   void noSTART()    { i2c->CR1 &= (uint16_t)(~CR1_START); };
   void genSTOP()    { i2c->CR1 |= (uint16_t)(CR1_STOP); };
   void noSTOP()     { i2c->CR1 &= (uint16_t)(~CR1_STOP); };
   void asknEnable() { i2c->CR1 |= (uint16_t)(CR1_ACK); };
   void asknDisable(){ i2c->CR1 &= (uint16_t)(~CR1_ACK); };
   ErrorStatus checkEvent( uint32_t ev ) { return I2C_CheckEvent( i2c, ev ); }
   uint32_t lastEvent() { return I2C_GetLastEvent( i2c ); }
   int prep( uint8_t addr, bool is_transmit, bool noWait = false ); // return 0=Err 1=Ok
   int ping( uint8_t addr );
   int send_pure( const uint8_t *ds, int ns ); // return 0=Err 1=Ok
   // returns: >0 = N of send/recv bytes, <0 - error
   int  send( uint8_t addr, uint8_t ds );
   int  send( uint8_t addr, const uint8_t *ds, int ns );
   int  send_reg_n( uint8_t addr, uint32_t reg, uint8_t reglen,  const uint8_t *ds, int ns );
   int  send_reg1( uint8_t addr, uint8_t reg,  const uint8_t *ds, int ns );
   int  send_reg2( uint8_t addr, uint16_t reg, const uint8_t *ds, int ns );
   int  recv_pure( uint8_t *dd, int nd );
   int  recv( uint8_t addr );
   int  recv( uint8_t addr, uint8_t *dd, int nd );
   int  recv_reg_n( uint8_t addr, int32_t reg, uint8_t reglen,  uint8_t *dd, int nd );
   int  recv_reg1( uint8_t addr, int8_t reg,  uint8_t *dd, int nd );
   int  recv_reg2( uint8_t addr, int16_t reg, uint8_t *dd, int nd );
   int  send_recv( uint8_t addr, const uint8_t *ds, int ns, uint8_t *dd, int nd );
   void setMaxWait( uint32_t mv ) { maxWait = mv; }
   int getWaitCount() const { return wait_count; }
   uint16_t getNTrans() const { return n_trans; };
   uint16_t getErr() const { return err; };
   uint16_t getLastEv() const { return lastEv; };
   uint32_t getEv() {
     uint32_t f1 = i2c->SR1, f2 = i2c->SR2;
     f2 <<=16;
     lastEv = f1 | f2;
     return lastEv;
   };
   int checkEv( uint32_t ev ) {
     getEv();
     return ( ( lastEv & I2C_FLAG_Mask & ev ) == ev );
   }
   FlagStatus flagStatus( uint32_t flg ) {
     return I2C_GetFlagStatus( i2c, flg );
   }
   int waitForEv( uint32_t ev, int errcode ); // return 0 - failed to wait, != 0 - Ok
   int waitNoBusy(); // same

  protected:
   uint8_t own_addr;
   uint32_t speed;
   I2C_TypeDef *i2c;
   int maxWait = 5000;
   void (*wait_fun)() = taskYieldFun;
   int wait_count = 0;
   int n_trans = 0;
   int err = 0;
   uint32_t lastEv = 0;

};



#endif
