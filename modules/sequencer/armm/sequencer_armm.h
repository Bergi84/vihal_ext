/*
 * sequencer_armm.h
 *
 *  Created on: 14.04.2022
 *      Author: Klaus
 */

#ifndef SEQUENCER_ARMM_H_
#define SEQUENCER_ARMM_H_

#ifndef SEQ_MAXTASKS
  #define SEQ_MAXTASKS 32
#endif

#ifndef SEQ_MAXSTACKS
  #define SEQ_MAXSTACKS 4
#endif

#include "platform.h"

class sequencer
{
private:
  typedef struct
  {
    uint32_t sp;
    bool* event;
  }
  stackRec_t;

  stackRec_t stacks[SEQ_MAXSTACKS];
  stackRec_t *aktivStack;

  typedef struct
  {
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)();
      void (*pFunc)();
    };
  }
  taskCbRec_t;

  taskCbRec_t taskCb[SEQ_MAXTASKS];

  static constexpr uint32_t arrayLen = ((SEQ_MAXTASKS - 1)/32 + 1);
  uint32_t usedId[arrayLen];
  uint32_t aktivTask[arrayLen];
  uint32_t queuedTask[arrayLen];

public:
  void init();
  void addTask(uint8_t &aSeqID, TCbClass* aPObj, void (TCbClass::*aPMFunc)());
  void delTaks(uint8_t aSeqID);

  void queueTask(uint8_t aSeqID);

  void waitForEvent(bool* aEvent);

private:
  void storeStack();
  void loadStack();
};



#endif /* SEQUENCER_ARMM_H_ */
