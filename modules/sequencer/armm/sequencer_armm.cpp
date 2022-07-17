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
  highestTaskId = 0;

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
          if(highestTaskId < tmpID)
            highestTaskId = tmpID;

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
          if(highestTaskId < tmpID)
            highestTaskId = tmpID;

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

  uint8_t highestId = 0;

  for(int i = 0; i < arrayLen; i++)
  {
    if(usedId[i] != 0)
    {
      highestId = (31 - __CLZ(usedId[i])) + i*32;
    }
  }

  highestTaskId = highestId;

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
    TCriticalSection sec(true);

    queuedTask[i] |= mask;

    sec.leave();

    return true;
  }

  return false;
}

void TSequencer::waitForEvent(bool* aEvt)
{
  if(aEvt == 0)
    return;

  stackRec_t* stack = &stacks[aktivStackInd];
  stack->event = aEvt;

  if(switchTask(&stack->sp, true))
  {
    scheduler();
  }
}

uint32_t TSequencer::getAktivTask()
{
  return stacks[aktivStackInd].id;
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
  void *sp = (void*)(((uint32_t)stackBaseAdr) - stackSize * stackInd);
  uint32_t clobber;

  // switch to new stack
  // add a guard for stack overflow detection ad the bottom of each stack
  __asm volatile
  (
    // clear fpu used flag
    "MRS %0, CONTROL                \n"
    "bic %0, %0, 0x04               \n"
    "MSR CONTROL, %0                \n"

    // load new stack pointer
    "cpsid i                        \n"
    "MSR MSP, %1                    \n"
    "isb                            \n"
    "cpsie i                        \n"

    : "+&r" (clobber) // no output parameter
    : "r" (sp)
    : "memory"
  );

  aktivStackInd = stackInd;
  stackRec_t* stack = &stacks[stackInd];

  // store stack context
  stack->sp = stacks[stackInd].sp;
  stack->event = 0;
  stack->id = taskInd;

  // set task as active
  uint32_t i = taskInd >> 5;
  uint32_t j = taskInd & 0x1F;
  uint32_t mask = 1U << j;

  aktivTask[i] |= mask;

  // can be modified from interrupt context
  // and is not a atomic instruction
  TCriticalSection sec(true);

  queuedTask[i] &= ~mask;

  sec.leave();

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
  stack->sp = 0;
  stack->id = invalidId;

  // return is not allowed because the stack is not valid anymore
  // so we call the scheduler instead

  scheduler();
}


//// stores all register to stack and return stack pointer
//__attribute__((naked)) void TSequencer::storeTask(void **sp)
//{
//  uint32_t clobber1;
//  uint32_t clobber2;
//
//  __asm volatile
//  (
//      // check if FPU was used
//      "MRS %0, CONTROL                  \n" // store CONTROL to r1
//      "tst %0, #0x04                    \n" // and of r1 and 0x04, Z=1 if result is zero
//
//      // if fpu was used store all necessary fpu register
//      "ittt ne                          \n"
//      "vmrsne %1, fpscr                 \n"
//      "stmdbne sp!, {%1}                \n"
//      "vstmdbne sp!, {s0-s31}           \n"
//
//      "stmdb sp!, {%0}                  \n" // store CONTROL on stack for restore of task
//
//      // store core registers
//      "stmdb sp!, {r4-r11, r14}         \n"
//
//      // get stack pointer
//      "mrs %1, MSP                      \n"
//      "str %1, [%2]                     \n"
//
//      : "+&r" (clobber1), "+&r" (clobber2)
//      : "r" (sp)
//      : "memory"
//  );
//}
//
//
//__attribute__((naked)) void TSequencer::restoreTask(void **sp)
//{
//  uint32_t clobber1;
//  uint32_t clobber2;
//
//  __asm volatile
//  (
//      // set stack pointer
//      "cpsid i                          \n"
//      "ldr %1, [%2]                     \n"
//      "msr msp, %1                      \n"
//      "isb                              \n"
//      "cpsie i                          \n"
//
//      // load core registers
//      "ldmia sp!, {r4-r11, r14}         \n"
//      "ldmia sp!, {%0}                  \n"
//      "MSR CONTROL, %0                  \n" // restore CONTROL to r1
//      "tst %0, 0x04                     \n" // and 0x04, only updates N and Z flags
//
//      // check if FPU was used
//      // and restore fpu register in necessary
//      "ittt ne                          \n"
//      "vldmiane sp!, {s0-s31}           \n"
//      "ldmiane sp!, {%0}                \n"
//      "vmsrne fpscr, %0                 \n"
//
//      : "+&r" (clobber1), "+&r" (clobber1)
//      : "r" (sp)
//      : "memory"
//  );
//}

bool TSequencer::switchTask(void **sp, bool pause)
{
  uint32_t clobber1;
  uint32_t clobber2;

  if(pause)
  {
    __asm volatile
    (
        // check if FPU was used
        "MRS %0, CONTROL                  \n" // store CONTROL to r1
        "tst %0, #0x04                    \n" // and of r1 and 0x04, Z=1 if result is zero

        // if fpu was used store all necessary fpu register
        "ittt ne                          \n"
        "vmrsne %1, fpscr                 \n"
        "stmdbne sp!, {%1}                \n"
        "vstmdbne sp!, {s0-s31}           \n"

        "stmdb sp!, {%0}                  \n" // store CONTROL on stack for restore of task

        // store core registers
        "stmdb sp!, {r4-r11, r14}         \n"

        // get stack pointer
        "mrs %1, MSP                      \n"
        "str %1, [%2]                     \n"

        : "+&r" (clobber1), "+&r" (clobber2)
        : "r" (sp)
        : "memory"
    );
    return true;
  }
  else
  {
    __asm volatile
    (
        // set stack pointer
        "cpsid i                          \n"
        "ldr %1, [%2]                     \n"
        "msr msp, %1                      \n"
        "isb                              \n"
        "cpsie i                          \n"

        // load core registers
        "ldmia sp!, {r4-r11, r14}         \n"
        "ldmia sp!, {%0}                  \n"
        "MSR CONTROL, %0                  \n" // restore CONTROL to r1

        // check if FPU was used
        "tst %0, 0x04                     \n" // and 0x04, only updates N and Z flags

        // and restore fpu register if necessary
        "ittt ne                          \n"
        "vldmiane sp!, {s0-s31}           \n"
        "ldmiane sp!, {%0}                \n"
        "vmsrne fpscr, %0                 \n"

        : "+&r" (clobber1), "+&r" (clobber1)
        : "r" (sp)
        : "memory"
    );

    return false;
  }

}

void TSequencer::scheduler()
{
  while(1)
  {
    // check waiting tasks and search free stack for new task
    // waiting tasks have always a higher priority
    uint8_t freeInd = invalidId;
    uint8_t endInd = schedLastStackInd;
    uint8_t tmpId = endInd;

    do
    {
      tmpId = (tmpId == SEQ_MAXSTACKS - 1) ? 0 : tmpId + 1;

      if(stacks[tmpId].sp != 0)
      {
        if(stacks[tmpId].event != 0 && *stacks[tmpId].event)
        {
          // todo: check stack integrity
          stacks[tmpId].event = 0;
          schedLastStackInd = tmpId;
          aktivStackInd = tmpId;
          switchTask(&stacks[tmpId].sp, false); // this function never returns
        }
      }
      else
      {
        freeInd = tmpId;
      }
    }
    while(tmpId != endInd);

    // if a free stack is available
    // search queued task for execution
    if(freeInd != invalidId)
    {
      endInd = schedLastTaskId;
      tmpId = endInd;

      do
      {
        tmpId = (tmpId == highestTaskId) ? 0 : tmpId + 1;

        uint32_t i = tmpId >> 5;
        uint32_t j = tmpId & 0x1F;
        uint32_t mask = 1U << j;

        if(usedId[i] & queuedTask[i] & ~aktivTask[i] & mask)
        {
          schedLastTaskId = tmpId;
          startTask(freeInd, tmpId);  // this function never returns
        }

      }
      while (endInd != tmpId);
    }

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
