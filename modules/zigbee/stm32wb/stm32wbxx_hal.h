/*
 * stm32wbxx_hal.h
 *
 *  Created on: 10.04.2022
 *      Author: Klaus
 */

#ifndef STM32WBXX_HAL_H_
#define STM32WBXX_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

#ifdef __cplusplus
}
#endif

#endif /* STM32WBXX_HAL_H_ */
