
#include "hwclktree_stm32.h"

void THwClkTree_stm32::updateClkTree()
{
  uint32_t tmp = regs->CR;
  switch((tmp & RCC_CR_MSIRANGE) >> RCC_CR_MSIRANGE_Pos)
  {
  case 0:  msiSpeed = 100000; break;
  case 1:  msiSpeed = 200000; break;
  case 2:  msiSpeed = 400000; break;
  case 3:  msiSpeed = 800000; break;
  case 4:  msiSpeed = 1000000; break;
  case 5:  msiSpeed = 2000000; break;
  case 6:  msiSpeed = 4000000; break;
  case 7:  msiSpeed = 8000000; break;
  case 8:  msiSpeed = 16000000; break;
  case 9:  msiSpeed = 24000000; break;
  case 10: msiSpeed = 32000000; break;
  case 11: msiSpeed = 48000000; break;
  }

  tmp = regs->CFGR;
  clkOutDiv = 1U << ((tmp & RCC_CFGR_MCOPRE) >> RCC_CFGR_MCOPRE_Pos);
  clkOutSource = static_cast<clkOutSource_t> ((tmp & RCC_CFGR_MCOSEL) >> RCC_CFGR_MCOSEL_Pos);

  if(tmp & RCC_CFGR_PPRE1_2)
  {
    pclk1div = 2U << ((tmp & (RCC_CFGR_PPRE1_0 | RCC_CFGR_PPRE1_1)) >> RCC_CFGR_PPRE1_Pos);
  }
  else
  {
    pclk1div = 1U;
  }

  if(tmp & RCC_CFGR_PPRE2_2)
  {
    pclk2div = 2U << ((tmp & (RCC_CFGR_PPRE2_0 | RCC_CFGR_PPRE2_1)) >> RCC_CFGR_PPRE2_Pos);
  }
  else
  {
    pclk2div = 1U;
  }

  switch ((tmp & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos)
  {
  case 1: hclk1div = 3;      break;
  case 2: hclk1div = 5;      break;
  case 5: hclk1div = 6;      break;
  case 6: hclk1div = 10;     break;
  case 7: hclk1div = 32;     break;
  case 8: hclk1div = 2;      break;
  case 9: hclk1div = 4;      break;
  case 10: hclk1div = 8;     break;
  case 11: hclk1div = 16;    break;
  case 12: hclk1div = 64;    break;
  case 13: hclk1div = 128;   break;
  case 14: hclk1div = 256;   break;
  case 15: hclk1div = 512;   break;
  default: hclk1div = 1;     break;
  }

  sysSource = static_cast<sysSource_t>((tmp & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos);
  sysStopSource = static_cast<sysSource_t>((tmp & RCC_CFGR_STOPWUCK) >> RCC_CFGR_STOPWUCK_Pos);

  tmp = regs->PLLCFGR;
  if(tmp & RCC_PLLCFGR_PLLREN)
  {
    mainPll.rDiv = ((tmp & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_Pos) + 1;
  }
  else
  {
    mainPll.rDiv = -1;
  }

  if(tmp & RCC_PLLCFGR_PLLQEN)
  {
    mainPll.qDiv = ((tmp & RCC_PLLCFGR_PLLQ) >> RCC_PLLCFGR_PLLQ_Pos) + 1;
  }
  else
  {
    mainPll.qDiv = -1;
  }

  if(tmp & RCC_PLLCFGR_PLLPEN)
  {
    mainPll.pDiv = ((tmp & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos) + 1;
  }
  else
  {
    mainPll.pDiv = -1;
  }

  mainPll.nMul = ((tmp & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);

  pllPreDiv = ((tmp & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos) + 1;

  pllSource = static_cast<pllSource_t>((tmp & RCC_PLLCFGR_PLLSRC) >> RCC_PLLCFGR_PLLSRC_Pos);

  tmp = regs->PLLSAI1CFGR;
  if(tmp & RCC_PLLSAI1CFGR_PLLREN)
  {
    saiPll.rDiv = ((tmp & RCC_PLLSAI1CFGR_PLLR) >> RCC_PLLSAI1CFGR_PLLR_Pos) + 1;
  }
  else
  {
    saiPll.rDiv = -1;
  }

  if(tmp & RCC_PLLSAI1CFGR_PLLQEN)
  {
    saiPll.qDiv = ((tmp & RCC_PLLSAI1CFGR_PLLQ) >> RCC_PLLSAI1CFGR_PLLQ_Pos) + 1;
  }
  else
  {
    saiPll.qDiv = -1;
  }

  if(tmp & RCC_PLLSAI1CFGR_PLLPEN)
  {
    saiPll.pDiv = ((tmp & RCC_PLLSAI1CFGR_PLLP) >> RCC_PLLSAI1CFGR_PLLP_Pos) + 1;
  }
  else
  {
    saiPll.pDiv = -1;
  }

  saiPll.nMul = ((tmp & RCC_PLLSAI1CFGR_PLLN) >> RCC_PLLSAI1CFGR_PLLN_Pos);

  tmp = regs->SMPSCR;
  smpsSource = static_cast<smpsSource_t>((tmp & RCC_SMPSCR_SMPSSWS) >> RCC_SMPSCR_SMPSSWS_Pos);

  tmp = regs->EXTCFGR;

  hclk5Source = static_cast<hclk5Source_t>((tmp & RCC_EXTCFGR_RFCSS) >> RCC_EXTCFGR_RFCSS_Pos);

  switch ((tmp & RCC_EXTCFGR_C2HPRE) >> RCC_EXTCFGR_C2HPRE_Pos)
  {
  case 1: hclk2div = 3;      break;
  case 2: hclk2div = 5;      break;
  case 5: hclk2div = 6;      break;
  case 6: hclk2div = 10;     break;
  case 7: hclk2div = 32;     break;
  case 8: hclk2div = 2;      break;
  case 9: hclk2div = 4;      break;
  case 10: hclk2div = 8;     break;
  case 11: hclk2div = 16;    break;
  case 12: hclk2div = 64;    break;
  case 13: hclk2div = 128;   break;
  case 14: hclk2div = 256;   break;
  case 15: hclk2div = 512;   break;
  default: hclk2div = 1;     break;
  }

  switch ((tmp & RCC_EXTCFGR_SHDHPRE) >> RCC_EXTCFGR_SHDHPRE_Pos)
  {
  case 1: hclk4div = 3;      break;
  case 2: hclk4div = 5;      break;
  case 5: hclk4div = 6;      break;
  case 6: hclk4div = 10;     break;
  case 7: hclk4div = 32;     break;
  case 8: hclk4div = 2;      break;
  case 9: hclk4div = 4;      break;
  case 10: hclk4div = 8;     break;
  case 11: hclk4div = 16;    break;
  case 12: hclk4div = 64;    break;
  case 13: hclk4div = 128;   break;
  case 14: hclk4div = 256;   break;
  case 15: hclk4div = 512;   break;
  default: hclk4div = 1;     break;
  }

  tmp = regs->BDCR;
  rtcSource = static_cast<rtcSource_t>((tmp & RCC_BDCR_RTCSEL) >> RCC_BDCR_RTCSEL_Pos);

  tmp = regs->CSR;
  if((tmp & RCC_CSR_LSI2RDY) && (rtcSource == RTC_LSI1))
  {
    rtcSource = RTC_LSI2;
  }
}

bool THwClkTree_stm32::setRtcClkSource(rtcSource_t aClkSource)
{
  // LSI clock must always enabled for correct watch dog operation
  // by default LSI2 is enabled but LSI1 is more accurate and need less power
  // to select LSI1 as LSI clock source LSI2 must disabled
  uint32_t tmp;

  if(aClkSource == RTC_LSI2)
  {
    tmp = regs->CSR;

    // enable LSI2
    if(!(tmp & RCC_CSR_LSI2RDY))
    {
      tmp |= RCC_CSR_LSI2ON;
      regs->CSR = tmp;

      // wait until LSI2 is enabled
      while(!(tmp & RCC_CSR_LSI2RDY))
        tmp = regs->CSR;
    }

    // disable LSI1
    if(tmp & RCC_CSR_LSI1RDY)
    {
      tmp &= ~RCC_CSR_LSI1ON;
      regs->CSR = tmp;

      // wait until LSI1 is disabled
      while ((tmp & RCC_CSR_LSI1RDY))
        tmp = regs->CSR;
    }
  }
  else
  {
    tmp = regs->CSR;

    // enable LSI1
    if(!(tmp & RCC_CSR_LSI1RDY))
    {
      tmp |= RCC_CSR_LSI1ON;
      regs->CSR = tmp;

      // wait until LSI1 is enabled
      while(!(tmp & RCC_CSR_LSI1RDY))
        tmp = regs->CSR;
    }

    // disable LSI2
    if(tmp & RCC_CSR_LSI2RDY)
    {
      tmp &= ~RCC_CSR_LSI2ON;
      regs->CSR = tmp;

      // wait until LSI2 is disabled
      while ((tmp & RCC_CSR_LSI2RDY))
        tmp = regs->CSR;
    }
  }

  switch (aClkSource)
  {
  case RTC_LSE:
    tmp = PWR->CR1;
    // enable backup domain access
    if (!(tmp & PWR_CR1_DBP))
    {
      tmp |= PWR_CR1_DBP;
      PWR->CR1 = tmp;
      // wait for backup domain access
      while(!(PWR->CR1 & PWR_CR1_DBP));
    }

    // enable LSE and set drive strange to low
    tmp = regs->BDCR;
    if(!(tmp & RCC_BDCR_LSERDY) || (tmp & RCC_BDCR_LSEDRV))
    {
      tmp &= ~RCC_BDCR_LSEDRV;
      tmp |= RCC_BDCR_LSEON;
      regs->BDCR = tmp;

      // wait until LSE is ready
      while (!(regs->BDCR & RCC_BDCR_LSERDY));
    }
    break;

  case RTC_LSI1:
  case RTC_LSI2:
    break;

  case RTC_HSE:
    enableHSE();
    break;
  }

  // switch RTC to required clock source
  tmp = regs->BDCR;
  tmp &= ~RCC_BDCR_RTCSEL;
  tmp |= (aClkSource << RCC_BDCR_RTCSEL_Pos) & RCC_BDCR_RTCSEL;
  regs->BDCR = tmp;

  return true;
}

bool THwClkTree_stm32::setOutClkSource(clkOutSource_t aClkSource, uint8_t preDiv)
{
  uint32_t tmp = regs->CFGR;
  uint32_t src = (uint32_t)aClkSource << RCC_CFGR_MCOSEL_Pos;
  uint32_t div = (32 - __CLZ(preDiv - 1)) << RCC_CFGR_MCOPRE_Pos;

  if((tmp & RCC_CFGR_MCOSEL) != src || (tmp & RCC_CFGR_MCOPRE) != div)
  {
    tmp &= ~(RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE);
    tmp |= (src | div);
    regs->CFGR = tmp;
  }
  return true;
}

bool THwClkTree_stm32::setSysClkSource(sysSource_t aClkSource)
{
  switch(aClkSource)
  {
  case SYS_HSE:
    enableHSE();
    break;
  case SYS_HSI16:
    enableHSI16();
    break;
  case SYS_MSI:
    enableMSI();
    break;
  case SYS_PLLRCLK:
    enableMainPll();
    enableMainPllR();
    break;
  }

  // switch to requested source
  uint32_t tmp = regs->CFGR;
  uint32_t src = (uint32_t)aClkSource << RCC_CFGR_SW_Pos;
  tmp &= ~RCC_CFGR_SW;
  tmp |= src;
  regs->CFGR = tmp;
  while((regs->CFGR & RCC_CFGR_SW) != src);

  return true;
}

bool THwClkTree_stm32::setSysStopClkSource(sysSource_t aClkSource)
{
  switch(aClkSource)
  {
  case SYS_HSI16:
    enableHSI16();
    break;
  case SYS_MSI:
    enableMSI();
    break;
  case SYS_HSE:
  case SYS_PLLRCLK:
    return false;
  }

  // switch to requested source
  uint32_t tmp = regs->CFGR;
  uint32_t src = (uint32_t)aClkSource << RCC_CFGR_STOPWUCK_Pos;
  tmp &= ~RCC_CFGR_STOPWUCK;
  tmp |= src;
  regs->CFGR = tmp;
  while((regs->CFGR & RCC_CFGR_SWS) != src);

  return true;
}

bool THwClkTree_stm32::setPllClkSource(pllSource_t aClkSource, uint8_t preDiv)
{
  uint32_t tmp1 = regs->PLLCFGR;

  uint32_t srcConf = (uint32_t) aClkSource << RCC_PLLCFGR_PLLSRC_Pos;
  srcConf |= (preDiv - 1) << RCC_PLLCFGR_PLLM_Pos;

  bool pllIsOn;
  bool pllIsSysClk;

  if((tmp1 & (RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM)) != srcConf)
  {
    // check if pll is on
    uint32_t tmp2 = regs->CR;
    pllIsOn = tmp2 & RCC_CR_PLLON;
    if(pllIsOn)
    {
      // pll is on, check if pll is used as sys clk source
      pllIsSysClk = (regs->CFGR & RCC_CFGR_SWS) == ((uint32_t)SYS_PLLRCLK << RCC_CFGR_SWS_Pos);
      if(pllIsSysClk)
      {
        // pll is system clock switch to HSI16
        setSysClkSource(SYS_HSI16);
      }

      // disable pll
      tmp2 &= ~RCC_CR_PLLON;
      regs->CR = tmp2;

      while(regs->CR & RCC_CR_PLLRDY);
    }

    tmp1 &= ~(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM);
    tmp1 |= srcConf;
    regs->PLLCFGR = tmp1;

    if(pllIsOn)
    {
      enableMainPll();
      if(pllIsSysClk)
      {
        setSysClkSource(SYS_PLLRCLK);
      }
    }
  }
  return true;
}

bool THwClkTree_stm32::confPllMain(uint8_t nMul, uint8_t pDiv, uint8_t rDiv, uint8_t qDiv)
{
  uint32_t tmp1 = regs->PLLCFGR;

  uint32_t conf = (uint32_t)nMul << RCC_PLLCFGR_PLLN_Pos;
  conf = (uint32_t)pDiv << RCC_PLLCFGR_PLLP_Pos;
  conf = (uint32_t)rDiv << RCC_PLLCFGR_PLLR_Pos;
  conf = (uint32_t)qDiv << RCC_PLLCFGR_PLLQ_Pos;

  bool pllIsOn;
  bool pllIsSysClk;

  if((tmp1 & (RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLR | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLP)) != conf )
  {
    // check if pll is on
    uint32_t tmp2 = regs->CR;
    pllIsOn = tmp2 & RCC_CR_PLLON;
    if(pllIsOn)
    {
      // pll is on, check if pll is used as sys clk source
      pllIsSysClk = (regs->CFGR & RCC_CFGR_SWS) == ((uint32_t)SYS_PLLRCLK << RCC_CFGR_SWS_Pos);
      if(pllIsSysClk)
      {
        // pll is system clock switch to HSI16
        setSysClkSource(SYS_HSI16);
      }

      // disable pll
      tmp2 &= ~RCC_CR_PLLON;
      regs->CR = tmp2;

      while(regs->CR & RCC_CR_PLLRDY);
    }

    tmp1 &= ~(RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLR | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLP);
    tmp1 |= conf;
    regs->PLLCFGR = tmp1;

    if(pllIsOn)
    {
      enableMainPll();
      if(pllIsSysClk)
      {
        setSysClkSource(SYS_PLLRCLK);
      }
    }
  }
  return true;
}

bool THwClkTree_stm32::confPllSai(uint8_t nMul, uint8_t pDiv, uint8_t rDiv, uint8_t qDiv)
{
  uint32_t tmp1 = regs->PLLSAI1CFGR;

  uint32_t conf = (uint32_t)nMul << RCC_PLLSAI1CFGR_PLLN_Pos;
  conf = (uint32_t)pDiv << RCC_PLLSAI1CFGR_PLLP_Pos;
  conf = (uint32_t)rDiv << RCC_PLLSAI1CFGR_PLLR_Pos;
  conf = (uint32_t)qDiv << RCC_PLLSAI1CFGR_PLLQ_Pos;

  bool pllIsOn;

  if((tmp1 & (RCC_PLLSAI1CFGR_PLLN | RCC_PLLSAI1CFGR_PLLR | RCC_PLLSAI1CFGR_PLLQ | RCC_PLLSAI1CFGR_PLLP)) != conf )
  {
    // check if pll is on
    uint32_t tmp2 = regs->CR;
    pllIsOn = tmp2 & RCC_CR_PLLSAI1ON;
    if(pllIsOn)
    {
      // disable pll
      tmp2 &= ~RCC_CR_PLLSAI1ON;
      regs->CR = tmp2;

      while(regs->CR & RCC_CR_PLLSAI1RDY);
    }

    tmp1 &= ~(RCC_PLLSAI1CFGR_PLLN | RCC_PLLSAI1CFGR_PLLR | RCC_PLLSAI1CFGR_PLLQ | RCC_PLLSAI1CFGR_PLLP);
    tmp1 |= conf;
    regs->PLLSAI1CFGR = tmp1;

    if(pllIsOn)
    {
      enableSai1Pll();
    }
  }
  return true;
}

bool THwClkTree_stm32::setSmpsClkSource(smpsSource_t aClkSource, bool fourMhz)
{
  switch(aClkSource)
  {
  case SMPS_HSE:
    enableHSE();
    break;
  case SMPS_HSI16:
    enableHSI16();
    break;
  case SMPS_MSI:
    enableMSI();
    break;
  }

  uint32_t tmp = regs->SMPSCR;
  tmp &= ~(RCC_SMPSCR_SMPSDIV | RCC_SMPSCR_SMPSSEL);
  if(fourMhz)
    tmp |= aClkSource << RCC_SMPSCR_SMPSDIV;
  else
    tmp |= (aClkSource << RCC_SMPSCR_SMPSDIV) | RCC_SMPSCR_SMPSDIV_0;
  regs->SMPSCR = tmp;

  return true;
}

bool THwClkTree_stm32::hseCapTune(uint32_t capVal)
{
  uint32_t conf = capVal << RCC_HSECR_HSETUNE_Pos;
  uint32_t tmp = regs->HSECR;

  bool isHseOn;
  bool hseIsSysClk;
  bool pllHseIsSysClk;

  if((tmp & RCC_HSECR_HSETUNE) != conf)
  {
    uint32_t tmp2 = regs->CR;
    isHseOn = tmp2 & RCC_CR_HSEON;
    if(isHseOn)
    {
      uint32_t tmp3 = regs->CFGR;
      uint32_t sws = (tmp3 & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos;

      hseIsSysClk = (sws == (uint32_t)SYS_HSE);
      if(hseIsSysClk)
      {
        // HSE is system clk, switch to hsi16
        pllHseIsSysClk = false;
        setSysClkSource(SYS_HSI16);

      } else {
        uint32_t tmp3 = regs->PLLCFGR;

        pllHseIsSysClk = (sws == (uint32_t)SYS_PLLRCLK) && ((tmp3 & RCC_PLLCFGR_PLLSRC) == (uint32_t)PLL_HSE << RCC_PLLCFGR_PLLSRC_Pos));
        if(pllHseIsSysClk)
        {
          // HSE is source of pll and pll is source of system clock
          // switch system clock to hsi16
          setSysClkSource(SYS_HSI16);

          // disable PLL
          tmp2 &= ~RCC_CR_PLLON;
          regs->CR = tmp2;

          while(tmp2 & RCC_CR_PLLRDY)
            tmp2 = regs->CR;
        }
      }

      // disable HSE
      tmp2 &= ~RCC_CR_HSEON;
      regs->CR = tmp2;
      while(tmp2 & RCC_CR_HSERDY)
        tmp2 = regs->CR;
    }

    tmp &= ~RCC_HSECR_HSETUNE;
    tmp |= conf;
    regs->HSECR = 0xCAFECAFE; // unlock register
    regs->HSECR = tmp;

    // switch on hse if it was on
    if(isHseOn)
    {
      enableHSE();
    }

    // switch back to HSE
    if(hseIsSysClk)
    {
      setSysClkSource(SYS_HSE);
    }

    if(pllHseIsSysClk)
    {
      enableMainPll();
      setSysClkSource(SYS_PLLRCLK);
    }
  }
}

inline void THwClkTree_stm32::enableHSE()
{
  uint32_t tmp = regs->CR;
  if(!(tmp | RCC_CR_HSERDY))
  {
    // enable HSE
    tmp |=  RCC_CR_HSEON;
    regs->CR = tmp;

    // wait until HSE is ready
    while (!(regs->CR & RCC_CR_HSERDY));
  }
}

inline void THwClkTree_stm32::enableHSI16()
{
  uint32_t tmp = regs->CR;
  if(!(tmp | RCC_CR_HSIRDY))
  {
    // enable HSI6
    tmp |=  RCC_CR_HSION;
    regs->CR = tmp;

    // wait until HSI16 is ready
    while (!(regs->CR & RCC_CR_HSIRDY));
  }
}

inline void THwClkTree_stm32::enableMSI()
{
  uint32_t tmp = regs->CR;
  if(!(tmp | RCC_CR_MSIRDY))
  {
    // enable MSE
    tmp |=  RCC_CR_MSION;
    regs->CR = tmp;

    // wait until MSE is ready
    while (!(regs->CR & RCC_CR_MSIRDY));
  }
}

inline void THwClkTree_stm32::enableMainPll()
{
  uint32_t tmp = regs->CR;
  if(!(tmp | RCC_CR_PLLRDY))
  {
    // enable main pll
    tmp |=  RCC_CR_PLLON;
    regs->CR = tmp;

    // wait until main pll is ready
    while (!(regs->CR & RCC_CR_PLLRDY));
  }
}

inline void THwClkTree_stm32::enableSai1Pll()
{
  uint32_t tmp = regs->CR;
  if(!(tmp | RCC_CR_PLLSAI1RDY))
  {
    // enable sai1 pll
    tmp |=  RCC_CR_PLLSAI1ON;
    regs->CR = tmp;

    // wait until sai1 pll is ready
    while (!(regs->CR & RCC_CR_PLLSAI1RDY));
  }
}

inline void THwClkTree_stm32::enableMainPllQ()
{
  uint32_t tmp = regs->PLLCFGR;
  if(!(tmp | RCC_PLLCFGR_PLLQEN))
  {
    // enable HSE
    tmp |=  RCC_PLLCFGR_PLLQEN;
    regs->PLLCFGR = tmp;
  }
}

inline void THwClkTree_stm32::enableSai1PllQ()
{
  uint32_t tmp = regs->PLLSAI1CFGR;
  if(!(tmp | RCC_PLLSAI1CFGR_PLLQEN))
  {
    // enable HSE
    tmp |=  RCC_PLLSAI1CFGR_PLLQEN;
    regs->PLLSAI1CFGR = tmp;
  }
}

inline void THwClkTree_stm32::enableMainPllR()
{
  uint32_t tmp = regs->PLLCFGR;
  if(!(tmp | RCC_PLLCFGR_PLLREN))
  {
    // enable HSE
    tmp |=  RCC_PLLCFGR_PLLREN;
    regs->PLLCFGR = tmp;
  }
}

inline void THwClkTree_stm32::enableSai1PllR()
{
  uint32_t tmp = regs->PLLSAI1CFGR;
  if(!(tmp | RCC_PLLSAI1CFGR_PLLREN))
  {
    // enable HSE
    tmp |=  RCC_PLLSAI1CFGR_PLLREN;
    regs->PLLSAI1CFGR = tmp;
  }
}

inline void THwClkTree_stm32::enableMainPllP()
{
  uint32_t tmp = regs->PLLCFGR;
  if(!(tmp | RCC_PLLCFGR_PLLPEN))
  {
    // enable HSE
    tmp |=  RCC_PLLCFGR_PLLPEN;
    regs->PLLCFGR = tmp;
  }
}

inline void THwClkTree_stm32::enableSai1PllP()
{
  uint32_t tmp = regs->PLLSAI1CFGR;
  if(!(tmp | RCC_PLLSAI1CFGR_PLLPEN))
  {
    // enable HSE
    tmp |=  RCC_PLLSAI1CFGR_PLLPEN;
    regs->PLLSAI1CFGR = tmp;
  }
}
