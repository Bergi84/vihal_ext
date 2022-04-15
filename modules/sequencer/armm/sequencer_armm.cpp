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

bool TSequencer::addTask(uint8_t &aSeqID, void (*aMFunc)())
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
          tasks[tmpID].pObj = 0;
          tasks[tmpID].pFunc = aMFunc;
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

void TSequencer::storeStack(void* oldSp, void* newSp)
{
  __asm volatile
  (
      // store MSP (Main stack pointer to r0
      "MRS r0, MSP                      \n"

      // check if FPU was used
      "MRS r1, CONTROL                  \n" // store CONTROL to r1
      "stmdb r0!, {r1}                    \n" // store CONTROL on stack for restore of task
      "tst r1, 0x04                     \n" // and of r1 and 0x04, only updates N and Z flags

      // if fpu was used store all necessary fpu register
      "ittt eq                          \n"
      "vmrseq r2, fpscr                 \n"
      "stmdbeq r0!, {r2}                  \n"
      "vstmdbeq r0!, {s0-s31}           \n"

      // store core registers
      "stmdb r0!, {r4-r11, r14}         \n"

      // store old stack pointer to oldSp
      "str r0, [%0]                     \n"

      // clear fpu used flag
      "bic r1, r1, 0x04                 \n"
      "MSR CONTROL, r1                  \n"

      // load new stack pointer to newSp
      "ldr r0, [%1]                     \n"
      "MSR MSP, r0                      \n"
      "isb                              \n"

      : // no output register
      : "r" (oldSp), "r" (newSp)
      :
  );
}

void TSequencer::startTask(uint8_t stackInd, uint8_t taskInd)
{
  aktivStackInd = stackInd;
  stackRec_t* stack = &stacks[stackInd];
  taskCbRec_t* task = &tasks[taskInd];

  stack->sp = (void*)(((uint32_t)stackBaseAdr) - stackSize * stackInd);
  stack->event = 0;
  stack->id = taskInd;

  uint32_t i = taskInd >> 5;
  uint32_t j = taskInd & 0x1F;
  aktivTask[i] |= 1U << j;

  // todo: switch to new stack

  stacks[aktivStackInd].sp = 0;


  // return is not allowed because the stack is not valid anymore
  // so we call the scheduler instead

  scheduler();
}

void TSequencer::pauseTask()
{

  // return is not allowed because the stack holds the cpu and fpu register
  // so we call the scheduler instead
  scheduler();
}

void TSequencer::resumeTask(uint8_t stackInd)
{
  aktivStackInd = stackInd;
  stacks[stackInd].event = 0;

  // todo: restore task

}

void TSequencer::scheduler()
{
  while(1)
  {
    // check waiting tasks and search free stack for new task
    // waiting tasks have always a higher priority
    uint8_t endInd = schedLastStackInd;
    uint8_t freeInd = invalidId;
    schedLastStackInd = (schedLastStackInd == SEQ_MAXSTACKS - 1) ? 0 : schedLastStackInd + 1;
    while(schedLastStackInd != endInd)
    {
      if(stacks[schedLastStackInd].sp != 0)
      {
        if(stacks[schedLastStackInd].event != 0 && *stacks[schedLastStackInd].event)
          resumeTask(&stacks[schedLastStackInd]);
      }
      else
      {
        freeInd = schedLastStackInd;
      }

      schedLastStackInd = (schedLastStackInd == SEQ_MAXSTACKS - 1) ? 0 : schedLastStackInd + 1;
    }

    if(schedLastStackInd != invalidId)
    {
      // find queued task for execution

    }

    // all stacks used or nothing todo, goto low power mode
  }
}
