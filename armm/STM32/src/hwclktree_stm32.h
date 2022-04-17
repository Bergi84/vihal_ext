/*
 * hwclk_stm32wb.h.h
 *
 *  Created on: Oct 21, 2021
 *      Author: bergi
 */

#ifndef HWCLKTREE_STM32_H_
#define HWCLKTREE_STM32_H_

#define HWCLKTREE_PRE_ONLY
#include "hwclktree.h"

class THwClkTree_stm32 : public THwClkTree_pre
{
public:
  RCC_TypeDef* regs;

  typedef enum {
    RTC_OFF = 0,
    RTC_LSE = 1,
    RTC_LSI1 = 2,
    RTC_HSE = 3,
    RTC_LSI2 = 6,
  } rtcSource_t;

  typedef enum {
    OUT_OFF = 0,
    OUT_SYSCLK = 1,
    OUT_MSI = 2,
    OUT_HSI16 = 3,
    OUT_HSE = 4,
    OUT_PLLRCLK = 5,
    OUT_LSI1 = 6,
    OUT_LSI2 = 7,
    OUT_LSE = 8,
    OUT_HSI48 = 9
  } clkOutSource_t;

  typedef enum {
    SYS_MSI = 0,
    SYS_HSI16 = 1,
    SYS_HSE = 2,
    SYS_PLLRCLK = 3,
  } sysSource_t;

  typedef enum {
    PLL_OFF = 0,
    PLL_MSI = 1,
    PLL_HSI16 = 2,
    PLL_HSE = 3
  } pllSource_t;

  typedef enum {
    SMPS_HSI16 = 0,
    SMPS_MSI = 1,
    SMPS_HSE = 2,
    SMPS_OFF = 3
  } smpsSource_t;

  typedef enum {
    HCLK5_HSI16 = 0,
    HCLK5_HSE = 1
  } hclk5Source_t;

  typedef enum {
    UART_PCLK = 0,
    UART_SYSCLK = 1,
    UART_HSI16 = 2,
    UART_LSE = 3
  } uartSource_t;

  typedef enum {
    I2C_PCLK = 0,
    I2C_SYSCLK = 1,
    I2C_HSI16 = 2
  } i2cSource_t;

  typedef enum {
    LPTIM_PCLK = 0,
    LPTIM_LSI = 1,
    LPTIM_HSI16 = 2,
    LPTIM_LSE = 3
  } lptimSource_t;

  typedef enum {
    ADC_OFF = 0,
    ADC_SAIPLLR = 1,
    ADC_MAINPLLP = 2,
    ADC_SYSCLK = 3
  } adcSource_t;

  typedef enum {
    SAI1_SAIPLLP = 0,
    SAI1_MAINPLLP = 1,
    SAI1_HSI16 = 2,
    SAI1_SAIEXT = 3
  } sai1Source_t;

  typedef enum {
    USB_HSI48 = 0,
    USB_SAI1PLLQ = 1,
    USB_MAINPLLQ = 2,
    USB_MSI = 3
  }usbSource_t;

  typedef enum {
    RNG_USB = 0,
    RNG_LSI = 1,
    RNG_LSE = 2
  }rngSource_t;

  typedef enum {
    RFWU_OFF = 0,
    RFWU_LSE = 1,
    RFWU_HSE = 3,
  }rfwuSource_t;

  static constexpr uint32_t lsi1Speed = 32000;
  static constexpr uint32_t lsi2Speed = 32000;
  static constexpr uint32_t lseSpeed = 32768;
  static constexpr uint32_t hseSpeed = 32000000;
  static constexpr uint32_t hsi16Speed = 16000000;
  static constexpr uint32_t hsi48Speed = 48000000;
  uint32_t msiSpeed;
  uint32_t sysSpeed;

  rtcSource_t rtcSource;
  clkOutSource_t clkOutSource;
  uint8_t clkOutDiv;
  sysSource_t sysSource;
  sysSource_t sysStopSource;
  uint16_t hclk1div;
  uint16_t hclk2div;
  uint16_t hclk4div;
  pllSource_t pllSource;
  uint8_t pllPreDiv;
  struct {
    uint8_t nMul;
    uint8_t pDiv;
    uint8_t rDiv;
    uint8_t qDiv;
  } mainPll;
  struct {
    uint8_t nMul;
    uint8_t pDiv;
    uint8_t rDiv;
    uint8_t qDiv;
  } saiPll;
  smpsSource_t smpsSource;
  uint8_t pclk1div;
  uint8_t pclk2div;
  hclk5Source_t hclk5Source;
  rfwuSource_t rfwuSource;

  // Peripheries with selectable clk source
  uartSource_t usart1Source;
  uartSource_t lpusart1Source;
  i2cSource_t i2c1Source;
  i2cSource_t i2c3Source;
  lptimSource_t lptim1Source;
  lptimSource_t lptim2Source;
  sai1Source_t sai1Source;
  usbSource_t usbSource;
  adcSource_t adcSource;
  rngSource_t rngSource;

  inline bool init()
  {
    regs = RCC;

    return true;
  }

  // enable and select clock source for RTC
  // LSI2 or LSI2 clock direct used by watch dog so one LSI must always run
  // LSI1: low power LSI, used in combination with LSE and HSE to supply watch dog
  // or stand alone if selected
  // LSI2: low drift, factory calibration clock
  bool setRtcClkSource(rtcSource_t aClkSource);

  // select clock source for clock output
  // if does not enable clocks if a disabled clock source is selected
  // preDiv can a value between 1 and 32, only exponents of 2 are supported
  bool setOutClkSource(clkOutSource_t aClkSource, uint8_t preDiv);

  // enable and select clock source for system clock
  bool setSysClkSource(sysSource_t aClkSource);

  // enable and select clock source for stop mode
  // only HSI and MSI allowed
  bool setSysStopClkSource(sysSource_t aClkSource);

  // selects for both PLL's the clock input
  // input frequency must between 2,66Mhz and 16Mhz
  // preDiv can a value between 1 and 8
  bool setPllClkSource(pllSource_t aClkSource, uint8_t preDiv);

  // configures PLL
  // nMul multiplies input frequency and generates VCO clk
  // VCO must have value between 96Mhz and 344Mhz in range 1
  // nMul can value have a value between 6 and 127
  // pDiv, rDiv and qDiv divide VCO to generate corresponding clk outputs PLLP, PLLR and PLLQ
  // pDiv can have value between 2 and 32
  // rDiv and qDiv can a have value between 2 and 8
  bool confPllMain(uint8_t nMul, uint8_t pDiv, uint8_t rDiv, uint8_t qDiv);
  bool confPllSai(uint8_t nMul, uint8_t pDiv, uint8_t rDiv, uint8_t qDiv);

  // Selecting SMPS clk source
  // SMPS supports 4Mhz (fourMhz = true) and 8Mhz (fourMhz = false)
  // the STM32WB5MMG with integrated passive components is designed for 4Mhz
  bool setSmpsClkSource(smpsSource_t aClkSource, bool fourMhz);

  // selects the next lower possible clock speed
  // supported clock speeds: 100kHz, 200kHz, 400kHz, 800kHz
  // 1Mhz, 2Mhz, 4Mhz, 8Mhz, 16Mhz, 24Mhz, 32Mhz, 48Mhz

  bool setMsiSpeed(uint32_t aSpeed);

  // this function configure flash wait state for requested speed
  // and enables prefetching and caches
  bool confFlashForSpeed(uint32_t aClkSpeed);

  // set divider CPU1, APB1 and APB2
  // supported divider for APB1 and APB2: 1, 2, 4, 8, 16, 32
  // supported divider for CPU: 1, 2, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512
  bool setCpu1SubTreeDiv(uint32_t aCpuDiv, uint32_t aApb1Div, uint32_t aApb2Div);

  // set divider for CPU2
  // supported divider for CPU: 1, 2, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512
  bool setCpu2ClkDiv(uint32_t aDiv);

  // set divider for Flash and SRAM2
  // supported divider for CPU: 1, 2, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512
  bool setFlashClkDiv(uint32_t aDiv);

  // enable and select clock source for periphery
  bool setUsart1ClkSource(uartSource_t aClkSource);
  bool setLpusart1ClkSource(uartSource_t aClkSource);
  bool setI2c1ClkSource(i2cSource_t aClkSource);
  bool setI2c3ClkSource(i2cSource_t aClkSource);
  bool setLptim1ClkSource(lptimSource_t aClkSource);
  bool setLptim2ClkSource(lptimSource_t aClkSource);
  bool setSai1ClkSource(sai1Source_t aClkSource);
  bool setUsbClkSource(usbSource_t aClkSource);
  bool setAdcClkSource(adcSource_t aClkSource);
  bool setRngClkSource(rngSource_t aClkSource);

  bool hseCapTune(uint32_t capVal);
  bool loadHseTune();

  // disable all currently unused clock sources
  bool disableUnusedClk();

  bool getCpu1ClkSpeed(uint32_t &aClkSpeed);
  bool getCpu2ClkSpeed(uint32_t &aClkSpeed);
  bool getSysClkSpeed(uint32_t &aClkSpeed);
  bool getFlashSpeed(uint32_t &aClkSpeed);
  bool getPeriClkSpeed(void * aBaseAdr, uint32_t &aSpeed);

private:
  // updates all clock divider and clock sources
  // for clock speed calculations
  void updateClkTree();

  inline void enableHSE();
  inline void enableHSI16();
  inline void enableMSI();
  inline void enableHSI48();
  inline void enableLSE();

  inline void disableHSE();
  inline void disableHSI16();
  inline void disableMSI();
  inline void disableHSI48();
  inline void disableLSE();

  static constexpr uint32_t pllOutQ = 1;
  static constexpr uint32_t pllOutP = 2;
  static constexpr uint32_t pllOutR = 4;

  inline void enableMainPll(uint32_t pllOutEn = 0);
  inline void enableSai1Pll(uint32_t pllOutEn = 0);

  inline void disableMainPll(uint32_t pllOutDis = 0);
  inline void disableSai1Pll(uint32_t pllOutDis = 0);

  void getSysClkSpeed();
  void getMainPllClkSpeed(uint32_t & aSpeed, uint32_t pllOut);
  void getSai1PllClkSpeed(uint32_t & aSpeed, uint32_t pllOut);
};

#define HWCLKTREE_IMPL THwClkTree_stm32

#endif /* HWCLKTREE_STM32_H_ */
