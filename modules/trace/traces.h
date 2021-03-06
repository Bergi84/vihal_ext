/*
 *  file:     traces.h (uart)
 *  brief:    Trace message redirection to UART
 *  version:  1.00
 *  date:     2021-10-02
 *  authors:  nvitya
*/

#ifndef __Traces__h
#define __Traces__h

#include "hwuart.h"
#include "lowPowerManager.h"
#include "sequencer_armm.h"

#include "mp_printf.h"
#include <string.h>

extern THwUart conuart;  // console uart

#define TRACE(...) { gTrace.traceCpu1(__VA_ARGS__); }
#define TRACECPU1(...) { gTrace.traceCpu1(__VA_ARGS__); }
#define TRACECPU2(...) { gTrace.traceCpu2(__VA_ARGS__); }

#ifdef LTRACES
 #define LTRACE(...)  TRACE( __VA_ARGS__ )
#else
 #define LTRACE(...)
#endif

#undef LTRACES

// "\xhh" inserts a byte into string with 0xhh value, "0x1B" means ESC, "[" means CSI (Control Sequence Intoducer),
// m means Select Graphic Rendition, 0 means reset to normal, 30-37 means set foreground color
#define CC_NRM  "\x1B[0m"
#define CC_RED  "\x1B[31m"
#define CC_GRN  "\x1B[32m"
#define CC_YEL  "\x1B[33m"
#define CC_BLU  "\x1B[34m"
#define CC_MAG  "\x1B[35m"
#define CC_CYN  "\x1B[36m"
#define CC_WHT  "\x1B[37m"

class TTrace : public TCbClass
{
public:
  static constexpr uint32_t bufLength = 4096;

  THwUart* uart;
  TLowPowerManger* lpm;
  uint32_t lpmId;
  TSequencer* seq;
  uint8_t seqId;

  char buf[bufLength];
  uint32_t bufWInd;
  uint32_t bufRInd;

  static constexpr const char strCpu1[] = {"\x1B[0mCPU1: "};
  static constexpr const char strCpu2[] = {"\x1B[35mCPU2: "};
  static constexpr uint32_t strCpuIndent = 6;

  typedef enum {
   TA_NONE,
   TA_CPU1,
   TA_CPU2
  } aktiv_t;
  aktiv_t lastAktiv;
  bool intend;

  bool init(THwUart* aUart = 0, TLowPowerManger* aLpm = 0, TSequencer* aSeq = 0);

  void traceCpu1(const char* format, ...);
  void traceCpu2(const char* format, ...);

  void vprintf(aktiv_t aAktiv, const char* aFormat, va_list aVa);
  void vprintf_rn(aktiv_t aAktiv, const char* aFormat, va_list aVa);

  void printBuf(aktiv_t aAktiv, const char* aBuf, uint32_t aLen, bool addrn);

  void service();

  void flush();
};

extern TTrace gTrace;

#endif //!defined(Traces__h)

//--- End Of file --------------------------------------------------------------
