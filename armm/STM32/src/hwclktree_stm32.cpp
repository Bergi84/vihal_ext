
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
  rfwuSource = static_cast<rfwuSource_t>((tmp & RCC_CSR_RFWKPSEL) >> RCC_CSR_RFWKPSEL_Pos);

  getSysClkSpeed();
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
    enableLSE();
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

bool THwClkTree_stm32::setRfWakeupClkSource(rfwuSource_t aClkSource)
{
  switch(aClkSource)
  {
  case RFWU_OFF:
    break;

  case RFWU_LSE:
    enableLSE();
    break;

  case RFWU_HSE:
    enableHSE();
    break;
  }

  uint32_t tmp = regs->CSR;
  tmp &= ~RCC_CSR_RFWKPSEL;
  tmp |= aClkSource << RCC_CSR_RFWKPSEL_Pos;
  regs->CSR = tmp;

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
    enableMainPll(pllOutR);
    break;
  }

  // switch to requested source
  uint32_t tmp = regs->CFGR;
  tmp &= ~RCC_CFGR_SW;
  tmp |= ((uint32_t)aClkSource << RCC_CFGR_SW_Pos);
  regs->CFGR = tmp;
  while((regs->CFGR & RCC_CFGR_SWS) != ((uint32_t)aClkSource << RCC_CFGR_SWS_Pos));

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

  return true;
}

bool THwClkTree_stm32::setPllClkSource(pllSource_t aClkSource, uint8_t preDiv)
{
  uint32_t srcConf = (uint32_t) aClkSource << RCC_PLLCFGR_PLLSRC_Pos;
  srcConf |= (preDiv - 1) << RCC_PLLCFGR_PLLM_Pos;

  bool pllIsOn;
  bool pllIsSysClk;

  switch(aClkSource)
  {
  case PLL_HSE:
    enableHSE();
    break;
  case PLL_HSI16:
    enableHSI16();
    break;
  case PLL_MSI:
    enableMSI();
    break;
  }

  if((regs->PLLCFGR & (RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM)) != srcConf)
  {
    // check if pll is on
    pllIsOn = regs->CR & RCC_CR_PLLON;
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
      disableMainPll();
    }

    uint32_t tmp = regs->PLLCFGR;
    tmp &= ~(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM);
    tmp |= srcConf;
    regs->PLLCFGR = tmp;

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
  uint32_t conf = (uint32_t)nMul << RCC_PLLCFGR_PLLN_Pos;
  conf |= (uint32_t)(pDiv - 1) << RCC_PLLCFGR_PLLP_Pos;
  conf |= (uint32_t)(rDiv - 1) << RCC_PLLCFGR_PLLR_Pos;
  conf |= (uint32_t)(qDiv - 1) << RCC_PLLCFGR_PLLQ_Pos;

  bool pllIsOn;
  bool pllIsSysClk;

  if((regs->PLLCFGR & (RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLR | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLP)) != conf )
  {
    // check if pll is on
    pllIsOn = regs->CR & RCC_CR_PLLON;
    if(pllIsOn)
    {
      // pll is on, check if pll is used as sys clk source
      pllIsSysClk = (regs->CFGR & RCC_CFGR_SWS) == ((uint32_t)SYS_PLLRCLK << RCC_CFGR_SWS_Pos);
      if(pllIsSysClk)
      {
        // pll is system clock switch to HSI16
        setSysClkSource(SYS_HSI16);
      }

      disableMainPll();
    }

    uint32_t tmp = regs->PLLCFGR;
    tmp &= ~(RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLR | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLP);
    tmp |= conf;
    regs->PLLCFGR = tmp;

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
  uint32_t conf = (uint32_t)nMul << RCC_PLLSAI1CFGR_PLLN_Pos;
  conf |= (uint32_t)(pDiv - 1) << RCC_PLLSAI1CFGR_PLLP_Pos;
  conf |= (uint32_t)(rDiv - 1) << RCC_PLLSAI1CFGR_PLLR_Pos;
  conf |= (uint32_t)(qDiv - 1) << RCC_PLLSAI1CFGR_PLLQ_Pos;

  bool pllIsOn;

  if((regs->PLLSAI1CFGR & (RCC_PLLSAI1CFGR_PLLN | RCC_PLLSAI1CFGR_PLLR | RCC_PLLSAI1CFGR_PLLQ | RCC_PLLSAI1CFGR_PLLP)) != conf )
  {
    // check if pll is on
    pllIsOn = regs->CR & RCC_CR_PLLSAI1ON;
    if(pllIsOn)
    {
      disableSai1Pll();
    }

    uint32_t tmp = regs->PLLSAI1CFGR;
    tmp &= ~(RCC_PLLSAI1CFGR_PLLN | RCC_PLLSAI1CFGR_PLLR | RCC_PLLSAI1CFGR_PLLQ | RCC_PLLSAI1CFGR_PLLP);
    tmp |= conf;
    regs->PLLSAI1CFGR = tmp;

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
    tmp |= aClkSource << RCC_SMPSCR_SMPSDIV_Pos;
  else
    tmp |= (aClkSource << RCC_SMPSCR_SMPSDIV_Pos) | RCC_SMPSCR_SMPSDIV_0;
  regs->SMPSCR = tmp;

  return true;
}

bool THwClkTree_stm32::setMsiSpeed(uint32_t aSpeed)
{
  uint32_t msiRange;

  if(aSpeed < 1000000)
  {
    if(aSpeed < 200000)
    {
      msiRange = 0;
    }
    else
    {
      msiRange = (32 - __CLZ(aSpeed/100000)) >> 1;
    }
  }
  else if(aSpeed < 16000000)
  {
    msiRange = ((32 - __CLZ(aSpeed/1000000)) >> 1) + 4;
  }
  else
  {
    msiRange = ((aSpeed - 16000000)/8000000) + 8;
  }

  msiRange <<= RCC_CR_MSIRANGE_Pos;

  uint32_t tmp = regs->CR;
  if(tmp & RCC_CR_MSION)
  {
    while(!(tmp & RCC_CR_MSIRDY))
      tmp = regs->CR;
  }
  tmp &= ~RCC_CR_MSIRANGE;
  tmp |= msiRange;
  regs->CR = tmp;

  return true;
}

bool THwClkTree_stm32::confFlashForSpeed(uint32_t aClkSpeed)
{
  unsigned ws;
  if      (aClkSpeed <= 18000000)  ws = FLASH_ACR_LATENCY_0WS;
  else if (aClkSpeed <= 36000000)  ws = FLASH_ACR_LATENCY_1WS;
  else if (aClkSpeed <= 54000000)  ws = FLASH_ACR_LATENCY_2WS;
  else
  {
    ws = FLASH_ACR_LATENCY_3WS;
  }

  // enable caches, prefetching and set wait states
  uint32_t tmp = FLASH->ACR;
  tmp &= ~FLASH_ACR_LATENCY;
  tmp |= (ws << FLASH_ACR_LATENCY_Pos) | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
  FLASH->ACR = ws;

  return true;
}

bool THwClkTree_stm32::setCpu1SubTreeDiv(uint32_t aCpuDiv, uint32_t aApb1Div, uint32_t aApb2Div)
{
  uint32_t tmp = regs->CFGR;
  tmp &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);

  if(aApb1Div > 1)
  {
    tmp |= ((32 - __CLZ(aApb1Div >> 2)) << RCC_CFGR_PPRE1_Pos) | RCC_CFGR_PPRE1_2;
  }

  if(aApb2Div > 1)
  {
    tmp |= ((32 - __CLZ(aApb2Div >> 2)) << RCC_CFGR_PPRE2_Pos) | RCC_CFGR_PPRE2_2;
  }

  uint32_t encDiv;

  if(aCpuDiv < 10)
  switch(aCpuDiv)
  {
  case 1: encDiv = 0;     break;
  case 2: encDiv = 8;     break;
  case 3: encDiv = 1;     break;
  case 4: encDiv = 9;     break;
  case 5: encDiv = 2;     break;
  case 6:
  case 7: encDiv = 5;     break;
  case 8:
  case 9: encDiv = 10;    break;
  }
  else if (aCpuDiv < 16)
    encDiv = 6;     // div = 10
  else if (aCpuDiv < 32)
    encDiv = 11;    // div = 16
  else if (aCpuDiv < 64)
    encDiv = 7;    // div = 32
  else if (aCpuDiv < 128)
    encDiv = 12;    // div = 64
  else if (aCpuDiv < 256)
    encDiv = 13;    // div = 128
  else if (aCpuDiv < 512)
    encDiv = 14;    // div = 256
  else
    encDiv = 15;    // div = 512

  tmp |= encDiv << RCC_CFGR_HPRE_Pos;

  regs->CFGR = tmp;

  return true;
}

bool THwClkTree_stm32::setCpu2ClkDiv(uint32_t aDiv)
{
  uint32_t tmp = regs->EXTCFGR;
    tmp &= ~RCC_EXTCFGR_C2HPRE;

    uint32_t encDiv;

    if(aDiv < 10)
    switch(aDiv)
    {
    case 1: encDiv = 0;     break;
    case 2: encDiv = 8;     break;
    case 3: encDiv = 1;     break;
    case 4: encDiv = 9;     break;
    case 5: encDiv = 2;     break;
    case 6:
    case 7: encDiv = 5;     break;
    case 8:
    case 9: encDiv = 10;    break;
    }
    else if (aDiv < 16)
      encDiv = 6;     // div = 10
    else if (aDiv < 32)
      encDiv = 11;    // div = 16
    else if (aDiv < 64)
      encDiv = 7;    // div = 32
    else if (aDiv < 128)
      encDiv = 12;    // div = 64
    else if (aDiv < 256)
      encDiv = 13;    // div = 128
    else if (aDiv < 512)
      encDiv = 14;    // div = 256
    else
      encDiv = 15;    // div = 512

    tmp |= encDiv << RCC_EXTCFGR_C2HPRE_Pos;

    regs->EXTCFGR = tmp;

    return true;
}

bool THwClkTree_stm32::setFlashClkDiv(uint32_t aDiv)
{
  uint32_t tmp = regs->EXTCFGR;
    tmp &= ~RCC_EXTCFGR_SHDHPRE;

    uint32_t encDiv;

    if(aDiv < 10)
    switch(aDiv)
    {
    case 1: encDiv = 0;     break;
    case 2: encDiv = 8;     break;
    case 3: encDiv = 1;     break;
    case 4: encDiv = 9;     break;
    case 5: encDiv = 2;     break;
    case 6:
    case 7: encDiv = 5;     break;
    case 8:
    case 9: encDiv = 10;    break;
    }
    else if (aDiv < 16)
      encDiv = 6;     // div = 10
    else if (aDiv < 32)
      encDiv = 11;    // div = 16
    else if (aDiv < 64)
      encDiv = 7;    // div = 32
    else if (aDiv < 128)
      encDiv = 12;    // div = 64
    else if (aDiv < 256)
      encDiv = 13;    // div = 128
    else if (aDiv < 512)
      encDiv = 14;    // div = 256
    else
      encDiv = 15;    // div = 512

    tmp |= encDiv << RCC_EXTCFGR_SHDHPRE_Pos;

    regs->EXTCFGR = tmp;

    return true;
}

bool THwClkTree_stm32::setUsart1ClkSource(uartSource_t aClkSource)
{
  switch(aClkSource)
  {
  case UART_HSI16:
    enableHSI16();
    break;

  case UART_LSE:
    enableLSE();
    break;

  case UART_PCLK:
    break;

  case UART_SYSCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_USART1SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_USART1SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setLpusart1ClkSource(uartSource_t aClkSource)
{
  switch(aClkSource)
  {
  case UART_HSI16:
    enableHSI16();
    break;

  case UART_LSE:
    enableLSE();
    break;

  case UART_PCLK:
    break;

  case UART_SYSCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_LPUART1SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_LPUART1SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setI2c1ClkSource(i2cSource_t aClkSource)
{
  switch(aClkSource)
  {
  case I2C_HSI16:
    enableHSI16();
    break;

  case I2C_PCLK:
    break;

  case I2C_SYSCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_I2C1SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_I2C1SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setI2c3ClkSource(i2cSource_t aClkSource)
{
  switch(aClkSource)
  {
  case I2C_HSI16:
    enableHSI16();
    break;

  case I2C_PCLK:
    break;

  case I2C_SYSCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_I2C3SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_I2C3SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setLptim1ClkSource(lptimSource_t aClkSource)
{
  switch(aClkSource)
  {
  case LPTIM_HSI16:
    enableHSI16();
    break;

  case LPTIM_LSE:
    enableLSE();
    break;

  case LPTIM_LSI:
    break;

  case LPTIM_PCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_LPTIM1SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_LPTIM1SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setLptim2ClkSource(lptimSource_t aClkSource)
{
  switch(aClkSource)
  {
  case LPTIM_HSI16:
    enableHSI16();
    break;

  case LPTIM_LSE:
    enableLSE();
    break;

  case LPTIM_LSI:
    break;

  case LPTIM_PCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_LPTIM2SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_LPTIM2SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setSai1ClkSource(sai1Source_t aClkSource)
{
  switch(aClkSource)
  {
  case SAI1_HSI16:
    enableHSI16();
    break;

  case SAI1_MAINPLLP:
    enableMainPll(pllOutP);
    break;

  case SAI1_SAIEXT:
    break;

  case SAI1_SAIPLLP:
    enableSai1Pll(pllOutP);
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_SAI1SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_SAI1SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setUsbClkSource(usbSource_t aClkSource)
{
  switch(aClkSource)
  {
  case USB_MAINPLLQ:
    enableMainPll(pllOutQ);
    break;

  case USB_HSI48:
    enableHSI48();
    break;

  case USB_MSI:
    enableMSI();
    break;

  case USB_SAI1PLLQ:
    enableSai1Pll(pllOutQ);
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_CLK48SEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_CLK48SEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setAdcClkSource(adcSource_t aClkSource)
{
  switch(aClkSource)
  {
  case ADC_MAINPLLP:
    enableMainPll(pllOutP);
    break;

  case ADC_SAIPLLR:
    enableSai1Pll(pllOutR);
    break;

  case ADC_SYSCLK:
    break;
  }

  uint32_t tmp = regs->CCIPR;
  tmp &= ~RCC_CCIPR_ADCSEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_ADCSEL_Pos;
  regs->CCIPR = tmp;

  return true;
}

bool THwClkTree_stm32::setRngClkSource(rngSource_t aClkSource)
{
  uint32_t tmp = regs->CCIPR;
  switch(aClkSource)
  {
  case RNG_LSE:
    enableLSE();
    break;

  case RNG_LSI:
    break;

  case RNG_USB:
    // enable selected USB clock source
    setUsbClkSource(static_cast<usbSource_t>((tmp & RCC_CCIPR_CLK48SEL) >> RCC_CCIPR_CLK48SEL_Pos));
    break;
  }

  tmp &= ~RCC_CCIPR_RNGSEL;
  tmp |= (uint32_t)aClkSource << RCC_CCIPR_RNGSEL_Pos;
  regs->CCIPR = tmp;

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

        pllHseIsSysClk = (sws == (uint32_t)SYS_PLLRCLK) && ((tmp3 & RCC_PLLCFGR_PLLSRC) == (uint32_t)PLL_HSE << RCC_PLLCFGR_PLLSRC_Pos);
        if(pllHseIsSysClk)
        {
          // HSE is source of pll and pll is source of system clock
          // switch system clock to hsi16
          setSysClkSource(SYS_HSI16);
          disableMainPll();
        }
      }

      disableHSE();
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
  return true;
}

bool THwClkTree_stm32::loadHseTune()
{
  typedef struct PACKED__
  {
    uint8_t   bd_address[6];
    uint8_t   hse_tuning;
    uint8_t   id;
  } OTP_ID0_t;

  OTP_ID0_t *p_id = (OTP_ID0_t*)(OTP_AREA_END_ADDR - 7);
  uint8_t otpId = 0;

  // each OTP block have 8Byte, the OTP is filled from lowest address to top address
  // we want the most recent entry so we search from back to front
  // at the highest address of each block is used as OTP id, we search the OTP id 0 for
  // manufacture tuned hse cap values for STM32WB5MMG
  while( ( p_id->id != otpId) && ( p_id != (OTP_ID0_t*)OTP_AREA_BASE) )
  {
    p_id--;
  }

  if(p_id->id == otpId)
  {
    return hseCapTune(p_id->hse_tuning);
  }
  return false;
}

bool THwClkTree_stm32::disableUnusedClk()
{
  updateClkTree();
  bool hseUsed = false;
  bool hsi16Used = false;
  bool msiUsed = false;
  bool mainPllPUsed = false;
  bool mainPllQUsed = false;
  bool mainPllRUsed = false;
  bool sai1PllPUsed = false;
  bool sai1PllQUsed = false;
  bool sai1PllRUsed = false;
  bool hsi48Used = false;
  bool lseUsed = false;

  if(regs->BDCR & RCC_BDCR_RTCEN)
  {
    switch(rtcSource)
    {
    case RTC_HSE:
      hseUsed = true;
      break;
    case RTC_LSE:
      lseUsed = true;
      break;
    case RTC_LSI1:
      break;
    case RTC_LSI2:
       break;
    case RTC_OFF:
      break;
    }
  }

  switch(clkOutSource)
  {
  case OUT_HSE:
    hseUsed = true;
    break;
  case OUT_HSI16:
    hsi16Used = true;
    break;
  case OUT_HSI48:
    hsi48Used = true;
    break;
  case OUT_LSE:
    lseUsed = true;
    break;
  case OUT_LSI1:
    break;
  case OUT_LSI2:
    break;
  case OUT_MSI:
    msiUsed = true;
    break;
  case OUT_PLLRCLK:
    mainPllRUsed = true;
    break;
  case OUT_SYSCLK:
    break;
  case OUT_OFF:
    break;
  }

  switch(sysSource)
  {
  case SYS_HSE:
    hseUsed = true;
    break;
  case SYS_HSI16:
    hsi16Used = true;
    break;
  case SYS_MSI:
    msiUsed = true;
    break;
  case SYS_PLLRCLK:
    mainPllRUsed = true;
    break;
  }

  switch(sysStopSource)
  {
  case SYS_HSI16:
    hsi16Used = true;
    break;
  case SYS_MSI:
    msiUsed = true;
    break;
  }

  switch(smpsSource)
  {
  case SMPS_HSE:
    hseUsed = true;
    break;
  case SMPS_HSI16:
    hsi16Used = true;
    break;
  case SMPS_MSI:
    msiUsed = true;
    break;
  case SMPS_OFF:
    break;
  }

  switch(hclk5Source)
  {
  case HCLK5_HSE:
    hseUsed = true;
    break;
  case HCLK5_HSI16:
    hsi16Used = true;
    break;
  }

  switch(rfwuSource)
  {
  case RFWU_HSE:
    hseUsed = true;
    break;
  case RFWU_LSE:
    lseUsed = true;
    break;
  case RFWU_OFF:
    break;
  }

  uint32_t tmpAPB1_1 = regs->APB1ENR1;

  if(tmpAPB1_1 & RCC_APB1ENR1_I2C1EN)
  {
    switch(i2c1Source)
    {
    case I2C_HSI16:
      hsi16Used = true;
      break;
    case I2C_PCLK:
      break;
    case I2C_SYSCLK:
      break;
    }
  }

  if(tmpAPB1_1 & RCC_APB1ENR1_I2C3EN)
  {
    switch(i2c3Source)
    {
    case I2C_HSI16:
      hsi16Used = true;
      break;
    case I2C_PCLK:
      break;
    case I2C_SYSCLK:
      break;
    }
  }

  if(tmpAPB1_1 & RCC_APB1ENR1_LPTIM1EN)
  {
    switch(lptim1Source)
    {
    case LPTIM_HSI16:
      hsi16Used = true;
      break;
    case LPTIM_LSE:
      lseUsed = true;
      break;
    case LPTIM_LSI:
      break;
    case LPTIM_PCLK:
      break;
    }
  }

  uint32_t tmpAHB3 = regs->AHB3ENR;

  if((tmpAPB1_1 & RCC_APB1ENR1_USBEN) ||
      ((tmpAHB3 & RCC_AHB3ENR_RNGEN) && (rngSource == RNG_USB)))
  {
    switch(usbSource)
    {
    case USB_HSI48:
      hsi48Used = true;
      break;
    case USB_MAINPLLQ:
      mainPllQUsed = true;
      break;
    case USB_MSI:
      msiUsed = true;
      break;
    case USB_SAI1PLLQ:
      sai1PllQUsed = true;
      break;
    }
  }

  if(tmpAHB3 & RCC_AHB3ENR_RNGEN)
  {
    switch(rngSource)
    {
    case RNG_LSE:
      lseUsed = true;
      break;
    case RNG_LSI:
      break;
    case RNG_USB:
      break;
    }
  }

  uint32_t tmpAPB1_2 = regs->APB1ENR2;

  if(tmpAPB1_2 & RCC_APB1ENR2_LPUART1EN)
  {
    switch(lpusart1Source)
    {
    case UART_HSI16:
      hsi16Used = true;
      break;
    case UART_LSE:
      lseUsed = true;
      break;
    case UART_PCLK:
      break;
    case UART_SYSCLK:
      break;
    }
  }

  if(tmpAPB1_2 & RCC_APB1ENR2_LPTIM2EN)
  {
    switch(lptim2Source)
    {
    case LPTIM_HSI16:
      hsi16Used = true;
      break;
    case LPTIM_LSE:
      lseUsed = true;
      break;
    case LPTIM_LSI:
      break;
    case LPTIM_PCLK:
      break;
    }
  }

  uint32_t tmpAPB2 = regs->APB2ENR;

  if(tmpAPB2 & RCC_APB2ENR_USART1EN)
  {
    switch(usart1Source)
    {
    case UART_HSI16:
      hsi16Used = true;
      break;
    case UART_LSE:
      lseUsed = true;
      break;
    case UART_PCLK:
      break;
    case UART_SYSCLK:
      break;
    }
  }

  if(tmpAPB2 & RCC_APB2ENR_SAI1EN)
  {
    switch(sai1Source)
    {
    case SAI1_HSI16:
      hsi16Used = true;
      break;
    case SAI1_MAINPLLP:
      mainPllPUsed = true;
      break;
    case SAI1_SAIEXT:
      break;
    case SAI1_SAIPLLP:
      sai1PllPUsed = true;
      break;
    }
  }

  if(regs->AHB2ENR & RCC_AHB2ENR_ADCEN)
  {
    switch(adcSource)
    {
    case ADC_MAINPLLP:
      mainPllPUsed = true;
      break;
    case ADC_OFF:
      break;
    case ADC_SAIPLLR:
      sai1PllRUsed = true;
      break;
    case ADC_SYSCLK:
      break;
    }
  }

  if(mainPllPUsed || mainPllQUsed || mainPllRUsed ||
      sai1PllPUsed || sai1PllQUsed || sai1PllRUsed)
  {
    switch(pllSource)
    {
    case PLL_HSE:
      hseUsed = true;
      break;
    case PLL_HSI16:
      hsi16Used = true;
      break;
    case PLL_MSI:
      msiUsed = true;
      break;
    case PLL_OFF:
      break;
    }
  }

  if(!hseUsed)
  {
    disableHSE();
  }

  if(!hsi16Used)
  {
    disableHSI16();
  }

  if(!msiUsed)
  {
    disableMSI();
  }

  uint32_t tmpMainPllDis = 0;
  if(!mainPllPUsed)
  {
    tmpMainPllDis |= pllOutP;
  }

  if(!mainPllQUsed)
  {
    tmpMainPllDis |= pllOutQ;
  }

  if(!mainPllRUsed)
  {
    tmpMainPllDis |= pllOutR;
  }

  if(tmpMainPllDis)
  {
    disableMainPll(tmpMainPllDis);
  }

  uint32_t tmpSai1PllDis = 0;
  if(!sai1PllPUsed)
  {
    tmpSai1PllDis |= pllOutP;
  }

  if(!sai1PllQUsed)
  {
    tmpSai1PllDis |= pllOutQ;
  }

  if(!sai1PllRUsed)
  {
    tmpSai1PllDis |= pllOutR;
  }

  if(tmpSai1PllDis)
  {
    disableSai1Pll(tmpSai1PllDis);
  }

  if(!hsi48Used)
  {
    disableHSI48();
  }

  if(!lseUsed)
  {
    // todo: implent diable LSE
  }

  return true;
}

bool THwClkTree_stm32::getCpu1ClkSpeed(uint32_t &aClkSpeed)
{
  updateClkTree();

  aClkSpeed = sysSpeed / (uint32_t)hclk1div;

  return true;
}

bool THwClkTree_stm32::getCpu2ClkSpeed(uint32_t &aClkSpeed)
{
  updateClkTree();

  aClkSpeed = sysSpeed / (uint32_t)hclk2div;

  return true;
}

bool THwClkTree_stm32::getSysClkSpeed(uint32_t &aClkSpeed)
{
  updateClkTree();

  aClkSpeed = sysSpeed;

  return true;
}

bool THwClkTree_stm32::getFlashSpeed(uint32_t &aClkSpeed)
{
  updateClkTree();

  aClkSpeed = sysSpeed / (uint32_t)hclk4div;

  return true;
}

bool THwClkTree_stm32::getPeriClkSpeed(void * aBaseAdr, uint32_t &aSpeed)
{
  uint32_t adr = (uint32_t) aBaseAdr;
  if(adr >= PERIPH_BASE && adr <= QUADSPI_R_BASE)
  {
    if(adr < APB2PERIPH_BASE)
    {
      // APB1
      uint32_t apb1speed;
      getCpu1ClkSpeed(apb1speed);
      apb1speed /= pclk1div;

      switch(adr)
      {
      case  TIM2_BASE:
        if(pclk1div > 1)
          aSpeed = 2 * apb1speed;
        else
          aSpeed = apb1speed;
        break;

      case  LCD_BASE:
      case  RTC_BASE:
        switch (rtcSource)
        {
        case RTC_LSE:
          aSpeed = lseSpeed;
          break;

        case RTC_LSI1:
          aSpeed = lsi1Speed;
          break;
        case RTC_LSI2:
          aSpeed = lsi2Speed;
          break;

        case RTC_HSE:
          aSpeed = hseSpeed / 32;
          break;
        }
        break;

      case  IWDG_BASE:
        if(rtcSource == lsi2Speed)
          aSpeed = lsi2Speed;
        else
          aSpeed = lsi1Speed;
        break;

      case  I2C1_BASE:
        switch(i2c1Source)
        {
        case I2C_HSI16:
          aSpeed = hsi16Speed;
          break;

        case I2C_PCLK:
          aSpeed = apb1speed;
          break;

        case I2C_SYSCLK:
          aSpeed = sysSpeed;
          break;
        }
        break;

      case  I2C3_BASE:
        switch(i2c3Source)
        {
        case I2C_HSI16:
          aSpeed = hsi16Speed;
          break;

        case I2C_PCLK:
          aSpeed = apb1speed;
          break;

        case I2C_SYSCLK:
          aSpeed = sysSpeed;
          break;
        }
        break;

      case  USB1_BASE:
      case  USB1_PMAADDR:
        switch(usbSource)
        {
        case USB_MAINPLLQ:
          getMainPllClkSpeed(aSpeed, pllOutQ);
          break;

        case USB_HSI48:
          aSpeed = hsi48Speed;
          break;

        case USB_MSI:
          aSpeed = msiSpeed;
          break;

        case USB_SAI1PLLQ:
          getSai1PllClkSpeed(aSpeed, pllOutQ);
          break;
        }
        break;

      case  LPTIM1_BASE:
        switch(lptim1Source)
        {
        case LPTIM_HSI16:
          aSpeed = hsi16Speed;
          break;

        case LPTIM_LSE:
          aSpeed = lseSpeed;
          break;

        case LPTIM_LSI:
          if(rtcSource == lsi2Speed)
            aSpeed = lsi2Speed;
          else
            aSpeed = lsi1Speed;
          break;

        case LPTIM_PCLK:
          aSpeed = apb1speed;
          break;
        }
        break;

      case  LPUART1_BASE:
        switch(lpusart1Source)
        {
        case UART_HSI16:
          aSpeed = hsi16Speed;
          break;

        case UART_LSE:
          aSpeed = lseSpeed;
          break;

        case UART_PCLK:
          aSpeed = apb1speed;
          break;

        case UART_SYSCLK:
          aSpeed = sysSpeed;
          break;
        }
        break;

      case  LPTIM2_BASE:
        switch(lptim2Source)
        {
        case LPTIM_HSI16:
          aSpeed = hsi16Speed;
          break;

        case LPTIM_LSE:
          aSpeed = lseSpeed;
          break;

        case LPTIM_LSI:
          if(rtcSource == lsi2Speed)
            aSpeed = lsi2Speed;
          else
            aSpeed = lsi1Speed;
          break;

        case LPTIM_PCLK:
          aSpeed = apb1speed;
          break;
        }
        break;

      default:
      case  CRS_BASE:
      case  SPI2_BASE:
      case  WWDG_BASE:
        aSpeed = apb1speed;
        break;
      }

    }
    else if(adr < AHB1PERIPH_BASE)
    {
      // APB2

      uint32_t apb2speed;
      getCpu1ClkSpeed(apb2speed);
      apb2speed /= pclk1div;
      switch(adr)
      {
      case  TIM1_BASE:
      case  TIM16_BASE:
      case  TIM17_BASE:
        if(pclk1div > 1)
          aSpeed = 2 * apb2speed;
        else
          aSpeed = apb2speed;
        break;

      case  USART1_BASE:
        switch(usart1Source)
        {
        case UART_HSI16:
          aSpeed = hsi16Speed;
          break;

        case UART_LSE:
          aSpeed = lseSpeed;
          break;

        case UART_PCLK:
          aSpeed = apb2speed;
          break;

        case UART_SYSCLK:
          aSpeed = sysSpeed;
          break;
        }
        break;

      case  SAI1_BASE:
      case  SAI1_Block_A_BASE:
      case  SAI1_Block_B_BASE:
        switch(sai1Source)
        {
        case SAI1_HSI16:
          aSpeed = hsi16Speed;
          break;

        case SAI1_MAINPLLP:
          getMainPllClkSpeed(aSpeed, pllOutP);
          break;

        case SAI1_SAIEXT:
          aSpeed = -1;
          break;

        case SAI1_SAIPLLP:
          getSai1PllClkSpeed(aSpeed, pllOutP);
          break;
        }
        break;

      default:
      case  SPI1_BASE:
      case  SYSCFG_BASE:
      case  VREFBUF_BASE:
      case  COMP1_BASE:
      case  COMP2_BASE:
        aSpeed = apb2speed;
        break;
      }
    }
    else if(adr < AHB2PERIPH_BASE)
    {
      // AHB1

      // all peripheral have the same speed
      getCpu1ClkSpeed(aSpeed);
    }
    else if(adr < AHB4PERIPH_BASE)
    {
      // AHB2
      uint32_t ahb4speed;
      getCpu1ClkSpeed(ahb4speed);

      switch(adr)
      {
      case  ADC1_BASE:
      case  ADC1_COMMON_BASE:
        switch(adcSource)
        {
        case ADC_MAINPLLP:
          getMainPllClkSpeed(aSpeed, pllOutP);
          break;

        case ADC_SAIPLLR:
          getSai1PllClkSpeed(aSpeed, pllOutR);
          break;

        case ADC_SYSCLK:
          aSpeed = sysSpeed;
          break;
        }
        break;

      default:
      case  GPIOA_BASE:
      case  GPIOB_BASE:
      case  GPIOC_BASE:
      case  GPIOD_BASE:
      case  GPIOE_BASE:
      case  GPIOH_BASE:
      case  AES1_BASE:
        aSpeed = ahb4speed;
        break;
      }
    }
    else if(adr < APB3PERIPH_BASE)
    {
      // AHB4

      switch(adr)
      {
      case  RNG_BASE:
        switch(rngSource)
        {
        case RNG_LSE:
          aSpeed = lseSpeed;
          break;

        case RNG_LSI:
          if(rtcSource == lsi2Speed)
            aSpeed = lsi2Speed;
          else
            aSpeed = lsi1Speed;
          break;

        case RNG_USB:
        {
          uint32_t usbSpeed;
          switch(usbSource)
          {
          case USB_MAINPLLQ:
            getMainPllClkSpeed(usbSpeed, pllOutQ);
            break;

          case USB_HSI48:
            usbSpeed = hsi48Speed;
            break;

          case USB_MSI:
            usbSpeed = msiSpeed;
            break;

          case USB_SAI1PLLQ:
            getSai1PllClkSpeed(usbSpeed, pllOutQ);
            break;
          }

          aSpeed = usbSpeed / 3;
        }
          break;
        }
        break;
      default:
      case  RCC_BASE:
      case  PWR_BASE:
      case  EXTI_BASE:
      case  IPCC_BASE:
      case  HSEM_BASE:
      case  AES2_BASE:
      case  PKA_BASE:
      case  FLASH_REG_BASE:
        getCpu2ClkSpeed(aSpeed);
        break;
      }
    }
    else if(adr < AHB3PERIPH_BASE)
    {
      // APB3
      // radio
    }
    else
    {
      // AHB3

      // all peripheral have the same speed
      getCpu1ClkSpeed(aSpeed);
    }

    return true;
  }
  return false;
}

inline void THwClkTree_stm32::enableHSE()
{
  uint32_t tmp = regs->CR;
  if(!(tmp & RCC_CR_HSERDY))
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
  if(!(tmp & RCC_CR_HSIRDY))
  {
    // enable HSI16
    tmp |=  RCC_CR_HSION;
    regs->CR = tmp;

    // wait until HSI16 is ready
    while (!(regs->CR & RCC_CR_HSIRDY));
  }
}

inline void THwClkTree_stm32::enableMSI()
{
  uint32_t tmp = regs->CR;
  if(!(tmp & RCC_CR_MSIRDY))
  {
    // enable MSE
    tmp |=  RCC_CR_MSION;
    regs->CR = tmp;

    // wait until MSE is ready
    while (!(regs->CR & RCC_CR_MSIRDY));
  }
}

inline void THwClkTree_stm32::enableHSI48()
{
  uint32_t tmp = regs->CRRCR;
  if(!(tmp & RCC_CRRCR_HSI48RDY))
  {
    // enable HSI16
    tmp |=  RCC_CRRCR_HSI48ON;
    regs->CRRCR = tmp;

    // wait until HSI16 is ready
    while (!(regs->CRRCR & RCC_CRRCR_HSI48RDY));
  }
}

inline void THwClkTree_stm32::enableLSE()
{
  uint32_t tmp = PWR->CR1;
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
}

inline void THwClkTree_stm32::disableHSE()
{
  uint32_t tmp = regs->CR;
  tmp &= ~RCC_CR_HSEON;
  regs->CR = tmp;
  while(regs->CR & RCC_CR_HSERDY);
}

inline void THwClkTree_stm32::disableHSI16()
{
  uint32_t tmp = regs->CR;
  tmp &= ~RCC_CR_HSION;
  regs->CR = tmp;
  while(regs->CR & RCC_CR_HSIRDY);
}

inline void THwClkTree_stm32::disableMSI()
{
  uint32_t tmp = regs->CR;
  tmp &= ~RCC_CR_MSION;
  regs->CR = tmp;
  while(regs->CR & RCC_CR_MSIRDY);
}

inline void THwClkTree_stm32::disableHSI48()
{
  uint32_t tmp = regs->CRRCR;
  tmp &= ~RCC_CRRCR_HSI48ON;
  regs->CRRCR = tmp;
  while(regs->CRRCR & RCC_CRRCR_HSI48RDY);
}

inline void THwClkTree_stm32::disableLSE()
{
  uint32_t tmp = PWR->CR1;
  // enable backup domain access
  if (!(tmp & PWR_CR1_DBP))
  {
    tmp |= PWR_CR1_DBP;
    PWR->CR1 = tmp;
    // wait for backup domain access
    while(!(PWR->CR1 & PWR_CR1_DBP));
  }

  // disable LSE
  tmp = regs->BDCR;
  if(tmp & RCC_BDCR_LSEON)
  {
    tmp &= ~RCC_BDCR_LSEON;
    regs->BDCR = tmp;

    // wait until LSE is disabled
    while ((regs->BDCR & RCC_BDCR_LSERDY));
  }
}

void THwClkTree_stm32::enableMainPll(uint32_t pllOutEn)
{
  uint32_t tmp = regs->CR;
  if(!(tmp & RCC_CR_PLLRDY))
  {
    // enable main pll
    tmp |=  RCC_CR_PLLON;
    regs->CR = tmp;

    // wait until main pll is ready
    while (!(regs->CR & RCC_CR_PLLRDY));
  }

  tmp = regs->PLLCFGR;
  if(pllOutEn & pllOutQ)
  {
    // enable pllq output
    tmp |=  RCC_PLLCFGR_PLLQEN;
  }

  if(pllOutEn & pllOutR)
  {
    // enable pllr output
    tmp |=  RCC_PLLCFGR_PLLREN;
  }

  if(pllOutEn & pllOutP)
  {
    // enable pllp output
    tmp |=  RCC_PLLCFGR_PLLPEN;
  }
  regs->PLLCFGR = tmp;
}

void THwClkTree_stm32::enableSai1Pll(uint32_t pllOutEn)
{
  uint32_t tmp = regs->CR;
  if(!(tmp & RCC_CR_PLLSAI1RDY))
  {
    // enable sai1 pll
    tmp |=  RCC_CR_PLLSAI1ON;
    regs->CR = tmp;

    // wait until sai1 pll is ready
    while (!(regs->CR & RCC_CR_PLLSAI1RDY));
  }

  tmp = regs->PLLSAI1CFGR;
  if(pllOutEn & pllOutP)
  {
    // enable pllp output
    tmp |=  RCC_PLLSAI1CFGR_PLLPEN;
  }

  if(pllOutEn & pllOutQ)
  {
    // enable pllq output
    tmp |=  RCC_PLLSAI1CFGR_PLLQEN;
  }

  if(pllOutEn & pllOutR)
  {
    // enable pllr output
    tmp |=  RCC_PLLSAI1CFGR_PLLREN;
  }
  regs->PLLSAI1CFGR = tmp;
}

inline void THwClkTree_stm32::disableMainPll(uint32_t pllOutDis)
{
  uint32_t tmp = regs->PLLCFGR;
  if(pllOutDis & pllOutQ)
  {
    // enable pllq output
    tmp &= ~RCC_PLLCFGR_PLLQEN;
  }

  if(pllOutDis & pllOutR)
  {
    // enable pllr output
    tmp &= ~RCC_PLLCFGR_PLLREN;
  }

  if(pllOutDis & pllOutP)
  {
    // enable pllp output
    tmp &= ~RCC_PLLCFGR_PLLPEN;
  }
  regs->PLLCFGR = tmp;

  if(pllOutDis == 0 || (tmp & (RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLREN)) == 0)
  {
    tmp = regs->CR;
    if(tmp & RCC_CR_PLLON)
    {
      tmp &= ~RCC_CR_PLLON;
      regs->CR = tmp;

      while(regs->CR & RCC_CR_PLLRDY);
    }
  }
}


inline void THwClkTree_stm32::disableSai1Pll(uint32_t pllOutDis)
{
  uint32_t tmp = regs->PLLSAI1CFGR;
  if(pllOutDis & pllOutQ)
  {
    // enable pllq output
    tmp &= ~RCC_PLLSAI1CFGR_PLLQEN;
  }

  if(pllOutDis & pllOutR)
  {
    // enable pllr output
    tmp &= ~RCC_PLLSAI1CFGR_PLLREN;
  }

  if(pllOutDis & pllOutP)
  {
    // enable pllp output
    tmp &= ~RCC_PLLSAI1CFGR_PLLPEN;
  }
  regs->PLLSAI1CFGR = tmp;

  if(pllOutDis == 0 || (tmp & (RCC_PLLSAI1CFGR_PLLQEN | RCC_PLLSAI1CFGR_PLLPEN | RCC_PLLSAI1CFGR_PLLREN)) == 0)
  {
    tmp = regs->CR;
    if(tmp & RCC_CR_PLLSAI1ON)
    {
      tmp &= ~RCC_CR_PLLSAI1ON;
      regs->CR = tmp;

      while(regs->CR & RCC_CR_PLLSAI1RDY);
    }
  }
}

void THwClkTree_stm32::getSysClkSpeed()
{
  switch(sysSource)
  {
  case SYS_HSE:
    sysSpeed = hseSpeed;
    break;
  case SYS_HSI16:
    sysSpeed = hsi16Speed;
    break;
  case SYS_MSI:
    sysSpeed = msiSpeed;
    break;
  case SYS_PLLRCLK:
    getMainPllClkSpeed(sysSpeed, pllOutR);
    break;
  }
}

void THwClkTree_stm32::getMainPllClkSpeed(uint32_t & aSpeed, uint32_t pllOut)
{
  uint32_t clkSourceSpeed;
  switch(pllSource)
  {
  case PLL_HSE:
    clkSourceSpeed = hseSpeed;
    break;
  case PLL_HSI16:
    clkSourceSpeed = hsi16Speed;
    break;
  case PLL_MSI:
    clkSourceSpeed = msiSpeed;
    break;
  }

  clkSourceSpeed = clkSourceSpeed / (uint32_t)pllPreDiv;

  if(pllOut & pllOutP)
  {
    aSpeed = (clkSourceSpeed * (uint32_t)mainPll.nMul) / (uint32_t)mainPll.pDiv;
  }

  if(pllOut & pllOutR)
  {
    aSpeed = (clkSourceSpeed * (uint32_t)mainPll.nMul) / (uint32_t)mainPll.rDiv;
  }

  if(pllOut & pllOutQ)
  {
    aSpeed = (clkSourceSpeed * (uint32_t)mainPll.nMul) / (uint32_t)mainPll.qDiv;
  }
}

void THwClkTree_stm32::getSai1PllClkSpeed(uint32_t & aSpeed, uint32_t pllOut)
{
  uint32_t clkSourceSpeed;
  switch(pllSource)
  {
  case PLL_HSE:
    clkSourceSpeed = hseSpeed;
    break;
  case PLL_HSI16:
    clkSourceSpeed = hsi16Speed;
    break;
  case PLL_MSI:
    clkSourceSpeed = msiSpeed;
    break;
  }

  clkSourceSpeed = clkSourceSpeed / (uint32_t)pllPreDiv;

  if(pllOut & pllOutP)
  {
    aSpeed = (clkSourceSpeed * (uint32_t)saiPll.nMul) / (uint32_t)saiPll.pDiv;
  }

  if(pllOut & pllOutR)
  {
    aSpeed = (clkSourceSpeed * (uint32_t)saiPll.nMul) / (uint32_t)saiPll.rDiv;
  }

  if(pllOut & pllOutQ)
  {
    aSpeed = (clkSourceSpeed * (uint32_t)saiPll.nMul) / (uint32_t)saiPll.qDiv;
  }
}
