/*
 * sequencer_armm.cpp
 *
 *  Created on: 14.04.2022
 *      Author: Klaus
 */

#include "sequencer_armm.h"

bool TSequencer::init()
{
  stackBaseAdr = (uint8_t*) &__stack;
  stackSize = ((uint32_t)&__Main_Stack_Size)/SEQ_MAXSTACKS;

  for(int i = 0; i < arrayLen; i++)
  {
    usedId[i] = 0;
  }

  for(int i = 0; i < SEQ_MAXSTACKS; i++)
  {
    stacks[i].sp = 0;
    stacks[i].event = 0;
    stacks[i].id = invalidId;
  }

  schedLastStackInd = 0;
  schedLastTaskId = 0;

  idleCb.pObj = 0;
  idleCb.pFunc = 0;

  return true;
}

bool TSequencer::addTask(uint8_t &aSeqID, TCbClass* aPObj, void (TCbClass::*aPMFunc)())
{
  for(int i = 0; i < arrayLen; i++)
  {
    for(int j = 0; j < 32; j++)
    {
      uint32_t mask = (1U << j);
      if((usedId[i] & mask) == 0)
      {
        uint8_t tmpID = i*32 + j;
        if(tmpID >= SEQ_MAXTASKS)
        {
          aSeqID = invalidId;
          return false;
        }
        else
        {
          usedId[i] |= mask;
          aktivTask[i] &= ~mask;
          queuedTask[i] &= ~mask;
          tasks[tmpID].pObj = aPObj;
          tasks[tmpID].pMFunc = aPMFunc;
          aSeqID = tmpID;
          return true;
        }
      }
    }
  }
  return false;
}

bool TSequencer::addTask(uint8_t &aSeqID, void (*aPFunc)())
{
  for(int i = 0; i < arrayLen; i++)
  {
    for(int j = 0; j < 32; j++)
    {
      uint32_t mask = (1U << j);
      if((usedId[i] & mask) == 0)
      {
        uint8_t tmpID = i*32 + j;
        if(tmpID >= SEQ_MAXTASKS)
        {
          aSeqID = invalidId;
          return false;
        }
        else
        {
          usedId[i] |= mask;
          aktivTask[i] &= ~mask;
          queuedTask[i] &= ~mask;
          tasks[tmpID].pObj = 0;
          tasks[tmpID].pFunc = aPFunc;
          aSeqID = tmpID;
          return true;
        }
      }
    }
  }
  return false;
}

bool TSequencer::delTask(uint8_t aSeqID)
{
  uint32_t i = aSeqID >> 5;
  uint32_t j = aSeqID & 0x1F;

  usedId[i] &= ~(1U << j);

  return true;
}

bool TSequencer::killTask(uint8_t aSeqID)
{
  for(int i = 0; i < SEQ_MAXTASKS; i++)
  {
    if(__get_IPSR() & 0x01FF == 0)
    {
      if(stacks[i].id == aSeqID)
      {
        stacks[i].id = 0;
        stacks[i].event = 0;
        if(i == aktivStackInd)
        {
          scheduler();
        }
        return true;
      }
    }
    else
    {
      // todo: implement kill from irq
    }
  }
  return false;
}

bool TSequencer::queueTask(uint8_t aSeqID)
{
  uint32_t i = aSeqID >> 5;
  uint32_t j = aSeqID & 0x1F;
  uint32_t mask = 1U << j;

  if(usedId[i] & mask)
  {
    queuedTask[i] |= mask;
    return true;
  }

  return false;
}

void TSequencer::waitForEvent(bool* aEvt)
{
  pauseTask(aEvt);
}

bool TSequencer::setIdleFunc(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
{
  idleCb.pObj = aPObj;
  idleCb.pMFunc = aPMFunc;

  return true;
}

bool TSequencer::setIdleFunc(void (*aPFunc)())
{
  idleCb.pObj = 0;
  idleCb.pFunc = aPFunc;

  return true;
}

inline void TSequencer::startTask(uint8_t stackInd, uint8_t taskInd)
{
  void *sp = stacks[stackInd].sp;
  uint32_t clobber;

  // switch to new stack
  __asm volatile
  (
    // clear fpu used flag
    "MRS %1, CONTROL                \n"
    "bic %1, %1, 0x04               \n"
    "MSR CONTROL, %1                \n"

    // load new stack pointer
    "cpsid i                        \n"
    "MSR MSP, %0                    \n"
    "isb                            \n"
    "cpsie i                        \n"

    : "+&r" (clobber) // no output parameter
    : "r" (sp)
    : "memory"
  );

  aktivStackInd = stackInd;
  stackRec_t* stack = &stacks[stackInd];

  // store stack context
  stack->sp = (void*)(((uint32_t)stackBaseAdr) - stackSize * stackInd);
  stack->event = 0;
  stack->id = taskInd;

  // set task as active
  uint32_t i = taskInd >> 5;
  uint32_t j = taskInd & 0x1F;
  uint32_t mask = 1U << j;
  aktivTask[i] |= mask;
  queuedTask[i] &= ~mask;

  taskCbRec_t* task = &tasks[taskInd];

  // call task
  if(task->pObj != 0)
  {
    (task->pObj->*(task->pMFunc))();
  }
  else
  {
    task->pFunc();
  }

  // set task as inactive
  aktivTask[i] &= ~mask;

  // return is not allowed because the stack is not valid anymore
  // so we call the scheduler instead

  scheduler();
}

inline void TSequencer::pauseTask(bool* evt)
{
  void *sp;
  uint32_t clobber;

  __asm volatile
  (
      // check if FPU was used
      "MRS %0, CONTROL                  \n" // store CONTROL to r1
      "tst %0, 0x04                     \n" // and of r1 and 0x04, only updates N and Z flags

      // if fpu was used store all necessary fpu register
      "ittt eq                          \n"
      "vmrseq %1, fpscr                 \n"
      "stmdbeq sp!, {%1}                \n"
      "vstmdbeq sp!, {s0-s31}           \n"

      "stmdb sp!, {%0}                  \n" // store CONTROL on stack for restore of task

      // store core registers
      "stmdb sp!, {r4-r11, r14}         \n"

      // get stack pointer
      "MRS %0, MSP                      \n"

      : "+&r" (sp), "+&r" (clobber)
      :
      : "memory"
  );

  stackRec_t* stack = &stacks[aktivStackInd];
  stack->sp = sp;
  stack->event = evt;

  // return is not allowed because the stack holds the cpu and fpu register
  // so we call the scheduler instead
  scheduler();
}

inline void TSequencer::resumeTask(uint8_t stackInd)
{
  aktivStackInd = stackInd;
  stackRec_t* stack = &stacks[stackInd];
  stack->event = 0;
  void* sp = stack->sp;

  __asm volatile
  (
      // todo:
      // disable interrupts for following two instructions

      // set stack pointer
      "cpsid i                          \n"
      "msr msp, %0                      \n"
      "isb                              \n"
      "cpsie i                          \n"

      // load core registers
      "ldmia sp!, {r4-r11, r14}         \n"
      "ldmia sp!, {%0}                  \n"
      "MSR CONTROL, %0                  \n" // restore CONTROL to r1
      "tst %0, 0x04                     \n" // and of witch 0x04, only updates N and Z flags

      // check if FPU was used
      // and restore fpu register in necessary
      "ittt eq                          \n"
      "vldmiaeq sp!, {s0-s31}           \n"
      "ldmiaeq sp!, {%0}                \n"
      "vmsreq fpscr, %0                 \n"

      : "+&r" (sp)
      :
      : "memory"
  );
}

void TSequencer::scheduler()
{
  while(1)
  {
    // check waiting tasks and search free stack for new task
    // waiting tasks have always a higher priority
    uint8_t endInd = schedLastStackInd;
    uint8_t freeInd = invalidId;
    uint8_t tmpId = (endInd == SEQ_MAXSTACKS - 1) ? 0 : endInd + 1;
    while(tmpId != endInd)
    {
      if(stacks[tmpId].sp != 0)
      {
        if(stacks[tmpId].event != 0 && *stacks[tmpId].event)
        {
          schedLastStackInd = tmpId;
          resumeTask(tmpId);
          continue;
        }
      }
      else
      {
        freeInd = tmpId;
      }

      tmpId = (tmpId == SEQ_MAXSTACKS - 1) ? 0 : tmpId + 1;
    }
    schedLastStackInd = tmpId;

    // if a free stack is available
    // search queued task for execution
    if(freeInd != invalidId)
    {
      endInd = schedLastTaskId;
      tmpId = (endInd == SEQ_MAXTASKS - 1) ? 0 : endInd + 1;

      while(endInd != tmpId)
      {
        uint32_t i = tmpId >> 5;
        uint32_t j = tmpId & 0x1F;
        uint32_t mask = 1U << j;

        if(usedId[i] & queuedTask[i] & ~aktivTask[i] & mask)
        {
          schedLastTaskId = tmpId;
          startTask(freeInd, tmpId);
          continue;
        }

        tmpId = (tmpId == SEQ_MAXTASKS - 1) ? 0 : tmpId + 1;
      }

    }
    schedLastTaskId = tmpId;

    // all stacks used or nothing todo, goto low power mode
    if(idleCb.pObj != 0)
    {
      (idleCb.pObj->*idleCb.pMFunc)();
    }
    else
    {
      if(idleCb.pFunc != 0)
      {
        idleCb.pFunc();
      }
    }
  }
}
