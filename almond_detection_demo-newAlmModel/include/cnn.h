/**************************************************************************************************
* Copyright (C) 2019-2021 Maxim Integrated Products, Inc. All Rights Reserved.
*
* Maxim Integrated Products, Inc. Default Copyright Notice:
* https://www.maximintegrated.com/en/aboutus/legal/copyrights.html
**************************************************************************************************/

/*
 * This header file was automatically generated for the AlmModel_0_3_ALM65_10k_blur_randScale1.2-0.8 network from a template.
 * Please do not edit; instead, edit the template and regenerate.
 */

#ifndef __CNN_H__
#define __CNN_H__

#include <stdint.h>
typedef int32_t q31_t;
typedef int16_t q15_t;

/* Return codes */
#define CNN_FAIL 0
#define CNN_OK 1

/*
  SUMMARY OF OPS
  Hardware: 85,240,092 ops (85,009,968 macc; 230,124 comp; 0 add; 0 mul; 0 bitwise)
    Layer 0: 629,740 ops (591,408 macc; 38,332 comp; 0 add; 0 mul; 0 bitwise)
    Layer 1: 1,524,096 ops (1,492,992 macc; 31,104 comp; 0 add; 0 mul; 0 bitwise)
    Layer 2: 5,992,704 ops (5,971,968 macc; 20,736 comp; 0 add; 0 mul; 0 bitwise)
    Layer 3: 11,964,672 ops (11,943,936 macc; 20,736 comp; 0 add; 0 mul; 0 bitwise)
    Layer 4: 11,964,672 ops (11,943,936 macc; 20,736 comp; 0 add; 0 mul; 0 bitwise)
    Layer 5: 11,964,672 ops (11,943,936 macc; 20,736 comp; 0 add; 0 mul; 0 bitwise)
    Layer 6: 23,929,344 ops (23,887,872 macc; 41,472 comp; 0 add; 0 mul; 0 bitwise)
    Layer 7 (backbone_conv8): 11,954,304 ops (11,943,936 macc; 10,368 comp; 0 add; 0 mul; 0 bitwise)
    Layer 8 (backbone_conv9): 2,996,352 ops (2,985,984 macc; 10,368 comp; 0 add; 0 mul; 0 bitwise)
    Layer 9 (backbone_conv10): 759,456 ops (746,496 macc; 12,960 comp; 0 add; 0 mul; 0 bitwise)
    Layer 10 (conv12_1): 374,544 ops (373,248 macc; 1,296 comp; 0 add; 0 mul; 0 bitwise)
    Layer 11 (conv12_2): 38,144 ops (36,864 macc; 1,280 comp; 0 add; 0 mul; 0 bitwise)
    Layer 12: 746,496 ops (746,496 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 13: 18,432 ops (18,432 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 14: 373,248 ops (373,248 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 15: 9,216 ops (9,216 macc; 0 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 275,184 bytes out of 442,368 bytes total (62%)
  Bias memory:   536 bytes out of 2,048 bytes total (26%)
*/

/* Number of outputs for this network */
#define CNN_NUM_OUTPUTS 64

/* Use this timer to time the inference */
//#define CNN_INFERENCE_TIMER MXC_TMR0

/* Port pin actions used to signal that processing is active */

#define CNN_START LED_On(1)
#define CNN_COMPLETE LED_Off(1)
#define SYS_START LED_On(0)
#define SYS_COMPLETE LED_Off(0)

/* Run software SoftMax on unloaded data */
void softmax_q17p14_q15(const q31_t * vec_in, const uint16_t dim_vec, q15_t * p_out);
/* Shift the input, then calculate SoftMax */
void softmax_shift_q17p14_q15(q31_t * vec_in, const uint16_t dim_vec, uint8_t in_shift, q15_t * p_out);

/* Stopwatch - holds the runtime when accelerator finishes */
extern volatile uint32_t cnn_time;

/* Custom memcopy routines used for weights and data */
void memcpy32(uint32_t *dst, const uint32_t *src, int n);
void memcpy32_const(uint32_t *dst, int n);

/* Enable clocks and power to accelerator, enable interrupt */
int cnn_enable(uint32_t clock_source, uint32_t clock_divider);

/* Disable clocks and power to accelerator */
int cnn_disable(void);

/* Perform minimum accelerator initialization so it can be configured */
int cnn_init(void);

/* Configure accelerator for the given network */
int cnn_configure(void);

/* Load accelerator weights */
int cnn_load_weights(void);

/* Verify accelerator weights (debug only) */
int cnn_verify_weights(void);

/* Load accelerator bias values (if needed) */
int cnn_load_bias(void);

/* Start accelerator processing */
int cnn_start(void);

/* Force stop accelerator */
int cnn_stop(void);

/* Continue accelerator after stop */
int cnn_continue(void);

/* Unload results from accelerator */
int cnn_unload(uint32_t *out_buf);

/* Turn on the boost circuit */
int cnn_boost_enable(mxc_gpio_regs_t *port, uint32_t pin);

/* Turn off the boost circuit */
int cnn_boost_disable(mxc_gpio_regs_t *port, uint32_t pin);

#endif // __CNN_H__
