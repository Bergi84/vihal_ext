/*
 * sequencer_armm.h
 *
 *  Created on: 14.04.2022
 *      Author: Klaus
 */

#ifndef SEQUENCER_ARMM_H_
#define SEQUENCER_ARMM_H_

#include "platform.h"

#ifndef SEQ_MAXTASKS
  #define SEQ_MAXTASKS 32
#endif

#ifndef SEQ_MAXSTACKS
  #define SEQ_MAXSTACKS 4
#endif

extern uint32_t __Main_Stack_Size;
extern uint32_t __stack;

#include "platform.h"

class TSequencer
{
private:
  typedef struct
  {
    void* sp;
    bool* event;
    uint8_t id;
  }
  stackRec_t;

  uint8_t* stackBaseAdr;
  uint32_t stackSize;
  stackRec_t stacks[SEQ_MAXSTACKS];
  volatile uint8_t aktivStackInd;

  typedef struct
  {
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)();
      void (*pFunc)();
    };
  }
  taskCbRec_t;

  taskCbRec_t tasks[SEQ_MAXTASKS];
  taskCbRec_t idleCb;

  static constexpr uint8_t invalidId = -1;
  static constexpr uint32_t arrayLen = ((SEQ_MAXTASKS - 1)/32 + 1);
  uint32_t usedId[arrayLen];
  uint32_t aktivTask[arrayLen];
  uint32_t queuedTask[arrayLen];

  uint8_t schedLastStackInd;
  uint8_t schedLastTaskId;

public:
  bool init();
  bool addTask(uint8_t &aSeqID, TCbClass* aPObj, void (TCbClass::*aPMFunc)());
  bool addTask(uint8_t &aSeqID, void (*aPFunc)());
  bool delTask(uint8_t aSeqID);
  bool killTask(uint8_t aSeqID);
  bool queueTask(uint8_t aSeqID);

  // puase calling Task until given bool becomes true
  void waitForEvent(bool* aEvt);

  bool setIdleFunc(TCbClass* aPObj, void (TCbClass::*aPMFunc)());
  bool setIdleFunc(void (*aPFunc)());

  // this function starts the idle loop and never returns
  void startIdle() { scheduler(); };

private:
  inline void startTask(uint8_t stackInd, uint8_t taskInd);
  inline void pauseTask(bool* evt);
  inline void resumeTask(uint8_t stackInd);
  void scheduler();
};



#endif /* SEQUENCER_ARMM_H_ */
