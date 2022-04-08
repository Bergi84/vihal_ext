/*
 * hwrtc_stm32.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWIPCC_STM32_H_
#define HWIPCC_STM32_H_

#define HWIPCC_PRE_ONLY
#include "hwipcc.h"

class THwIpcc_stm32 : public THwIpcc_pre
{
public:
  IPCC_TypeDef* regs;

  typedef struct {
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)();
      void (*pFunc)();
    };
  } cbRec_t;

  cbRec_t irqRxCbList[6];
  cbRec_t irqTxCbList[6];

  typedef enum {
    CH_1 = 1,
    CH_2 = 2,
    CH_3 = 4,
    CH_4 = 8,
    CH_5 = 16,
    CH_6 = 32,
  }channel_t;

  // enable clock and irq for ipcc
  bool init();

  bool enableRxChannel(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)());
  bool enableRxChannel(channel_t aChannel, void (*aPFunc)());
  inline bool enableRxChannel(channel_t aChannel)
  {
   regs->C1MR &= ~(((int32_t) aChannel) << IPCC_C1MR_CH1OM_Pos);
   return true;
  }

  bool setRxCb(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)());
  bool setRxCb(channel_t aChannel, void (*aPFunc)());

  inline bool disableRxChannel(channel_t aChannel)
  {
    regs->C1MR |= ((int32_t) aChannel) << IPCC_C1MR_CH1OM_Pos;
    return true;
  }

  bool enableTxChannel(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)());
  bool enableTxChannel(channel_t aChannel, void (*aPFunc)());
  inline bool enableTxChannel(channel_t aChannel)
  {
    regs->C1MR &= ~(((int32_t) aChannel) << IPCC_C1MR_CH1FM_Pos);
    return true;
  }
  bool setTxCb(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)());
  bool setTxCb(channel_t aChannel, void (*aPFunc)());

  inline bool diableTxChannel(channel_t aChannel)
  {
    regs->C1MR |= ((int32_t) aChannel) << IPCC_C1MR_CH1FM_Pos;
    return true;
  }

  // set transmit channel flag
  inline bool setTxFlag(channel_t aChannel)
  {
    regs->C1SCR = ((int32_t) aChannel) << IPCC_C1SCR_CH1S_Pos;
    return true;
  }

  // clears receive channel flag
  inline bool clearRxFlag(channel_t aChannel)
  {
    regs->C1SCR = ((int32_t) aChannel) << IPCC_C1SCR_CH1C_Pos;
    return true;
  }

  // get transmit occupied channel flag
  inline bool getTxActivFlag(channel_t aChannel)
  {
    return (regs->C1TOC2SR & (uint32_t) aChannel) == (uint32_t) aChannel;
  }

  inline bool getTxPending(uint32_t &channels)
  {
    channels = regs->C1TOC2SR & ((~regs->C1MR) >> IPCC_C1MR_CH1FM_Pos);
    return true;
  }

  // get receive pending channel flag
  inline bool getRxActivFlag(channel_t aChannel)
  {
    return (regs->C2TOC1SR & (uint32_t) aChannel) == (uint32_t) aChannel;
  }

  inline bool getRxPending(uint32_t &channels)
  {
    channels = regs->C2TOC1SR & ((~regs->C1MR) >> IPCC_C1MR_CH1OM_Pos);
    return true;
  }

  inline void rxIrqHandler()
  {
    uint32_t rxPendCh;
    getRxPending(rxPendCh);
    for(uint32_t i = (uint32_t)CH_1; i <= (uint32_t)CH_6; i << 1)
    {
      if(rxPendCh & i)
      {
        cbRec_t* cb = &irqRxCbList[31 - __CLZ((int32_t) i)];
        if(cb->pObj)
        {
          ((cb->pObj)->*(cb->pMFunc))();
        }
        else
        {
          cb->pFunc();
        }
      }
    }
  }

  inline void txIrqHandler()
  {
    uint32_t txPendCh;
    getTxPending(txPendCh);
    for(uint32_t i = (uint32_t)CH_1; i <= (uint32_t)CH_6; i << 1)
    {
      if(txPendCh & i)
      {
        cbRec_t* cb = &irqTxCbList[31 - __CLZ((int32_t) i)];
        if(cb->pObj)
        {
          ((cb->pObj)->*(cb->pMFunc))();
        }
        else
        {
          cb->pFunc();
        }
      }
    }
  }
};

#define HWIPCC_IMPL THwIpcc_stm32

#endif /* HWRTC_STM32_H_ */
