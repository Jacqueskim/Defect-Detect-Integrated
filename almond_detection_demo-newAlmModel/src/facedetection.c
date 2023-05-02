/******************************************************************************
 * Copyright (C) 2022 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/
#include <string.h>

#include "board.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "mxc.h"
#include "utils.h"
#include "camera.h"
#include "facedetection.h"
#include "post_process.h"
#include "utils.h"
#include "MAXCAM_Debug.h"
#include "cnn.h"
#ifdef BOARD_FTHR_REVA
#include "tft_ili9341.h"
#endif
#ifdef BOARD_EVKIT_V1
#include "tft_ssd2119.h"
#include "tsc2046.h"
#include "bitmap.h"
#endif
#include "led.h"
#include "lp.h"

#define S_MODULE_NAME "facedetection"


extern uint32_t ticks_1;
extern uint32_t ticks_2;
extern mxc_wut_cfg_t cfg;
extern uint32_t  camera_image[IMAGE_SIZE_X*IMAGE_SIZE_Y*2/4];

uint8_t parity = 0;
/************************************ VARIABLES ******************************/
volatile uint32_t cnn_time; // Stopwatch

void run_cnn(int x_offset, int y_offset);

#ifdef LP_MODE_ENABLE
static void ARM_low_power(int lp_mode);
#endif

uint8_t get_offset(void)
{
    return parity;
}


int face_detection(void)
{

#define PRINT_TIME 1
#if (PRINT_TIME == 1)
    /* Get current time */
    uint32_t process_time = utils_get_time_ms();
    uint32_t total_time   = utils_get_time_ms();
#endif

    while (1) { //Capture image and run CNN

        /* Check for received image */


        if (1){   

#if (PRINT_TIME == 1)
            process_time = utils_get_time_ms();
#endif

#ifdef IMAGE_TO_UART
            break;
#endif

            MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN); // Enable CNN clock

#ifdef LP_MODE_ENABLE
            /* Reinit CNN and reload weigths after UPM or Standby because CNN is powered off */
            if (LP_MODE > 2) {
                cnn_init();         // Bring state machine into consistent state
                cnn_load_weights(); // Reload CNN kernels
                cnn_load_bias();    // Reload CNN bias
                cnn_configure();    // Configure state machine
            }
#endif
            /* Run CNN on time on original and shifted images */
            run_cnn(0, 0);

#if (PRINT_TIME == 1)
            PR_INFO("Process Time Total : %dms", utils_get_time_ms() - process_time);
#endif

#ifdef LP_MODE_ENABLE
            cfg.cmp_cnt = ticks_1;
            /* Config WakeUp Timer */
            MXC_WUT_Config(&cfg);
            //Enable WUT
            MXC_WUT_Enable();

            LED_On(0); // green LED on
            /* Configure low power mode */
            ARM_low_power(LP_MODE);
            LED_Off(0); // green LED off

            // Camera startup delay (~100ms) after resuming XVCLK clock generated
            // by Pulse Train which is off during UPM/Standby mode
            if (LP_MODE > 2) {
                cfg.cmp_cnt = ticks_2;
                /* Config WakeUp Timer */
                MXC_WUT_Config(&cfg);
                //Enable WUT
                MXC_WUT_Enable();
                MXC_LP_EnterLowPowerMode();
            }

#endif

#if (PRINT_TIME == 1)
            PR_INFO("Capture Time : %dms", process_time - total_time);
            PR_INFO("Total Time : %dms", utils_get_time_ms() - total_time);
            total_time = utils_get_time_ms();
#endif
            /* Sleep until camera interrupt */
            //MXC_LP_EnterSleepMode(); // disable for streaming camera case
        }
    }

    return 0;
}

void run_cnn(int x_offset, int y_offset)
{
    uint32_t imgLen;
    uint32_t w, h;
    /* Get current time */
    uint32_t pass_time = 0;
    uint8_t* raw = NULL;
    int row;

    static int offset = 0;

     stream_stat_t *stat;
    camera_start_capture_image();

    // Get the details of the image from the camera driver.
    camera_get_image(&raw, &imgLen, &w, &h);

    offset ^= X_OFFSET; //Alternate left and right 224x168 image from 448x240 frame
    if (offset == 0){
        parity = 0;
    } else {
        parity = 1;
    }
    printf("w:%d: h%d  offset:%d\n",w,h,offset);

    // Get image line by line
    for (row = 0; row < h; row++) {    
        // Wait until camera streaming buffer is full
        while ((raw = get_camera_stream_buffer()) == NULL) {
            if (camera_is_image_rcv()) {
                break;
            }
        };

        // Copy image row to the buffer
        if (row<IMAGE_H)
            memcpy((uint8_t*)&camera_image[row*IMAGE_W*LCD_BYTE_PER_PIXEL/4], raw + offset*LCD_BYTE_PER_PIXEL, IMAGE_W*LCD_BYTE_PER_PIXEL);
       //MXC_TFT_ShowImageCameraRGB565(row + X_START, Y_START , raw, IMAGE_SIZE_Y, 1);

        // Release stream buffer
        release_camera_stream_buffer();
    }


// check for overflow
    stat = get_camera_stream_statistic();

    if (stat->overflow_count > 0) {
        printf("OVERFLOW = %d\n", stat->overflow_count);
        LED_On(LED2); // Turn on red LED if overflow detected

        while (1) {}
    }
    raw = (uint8_t*)&camera_image[0];

    #ifdef TFT_ENABLE
        #ifdef BOARD_FTHR_REVA
            MXC_TFT_ShowImageCameraRGB565(X_START, Y_START, raw, IMAGE_SIZE_Y, IMAGE_SIZE_X);
        #endif
        #ifdef BOARD_EVKIT_V1
            MXC_TFT_ShowImageCameraRGB565(X_START, Y_START, raw, IMAGE_SIZE_X, IMAGE_SIZE_Y);
        #endif
    #endif
    

    // Enable CNN clock
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);

    cnn_start();

    PR_INFO("CNN initialization time : %d", utils_get_time_ms() - pass_time);

    uint8_t* data = raw;

    pass_time = utils_get_time_ms();

#if 1
    for (int i = y_offset; i < HEIGHT + y_offset; i++) {

            data = raw;
            data += i * BYTE_PER_PIXEL;
            for (int j = x_offset; j < WIDTH + x_offset; j++) {
                uint8_t ur, ug, ub;
                int8_t r, g, b;
                uint32_t number;

                ub = (uint8_t)(data[j * BYTE_PER_PIXEL * IMAGE_W + 1] << 3);
                ug = (uint8_t)((data[j * BYTE_PER_PIXEL * IMAGE_W] << 5) |
                            ((data[j * BYTE_PER_PIXEL * IMAGE_W + 1] & 0xE0) >> 3));
                ur = (uint8_t)(data[j * BYTE_PER_PIXEL * IMAGE_W] & 0xF8);
                b = ub - 128;
                g = ug - 128;
                r = ur - 128;

                // Loading data into the CNN fifo
                while (((*((volatile uint32_t*)0x50000004) & 1)) != 0)
                    ; // Wait for FIFO 0

                number = 0x00FFFFFF & ((((uint8_t)b) << 16) | (((uint8_t)g) << 8) | ((uint8_t)r));

                *((volatile uint32_t*)0x50000008) = number; // Write FIFO 0
                
            }
        }
#else
    for (int i = y_offset; i < HEIGHT + y_offset; i++) {

        data = raw + ((IMAGE_H - (WIDTH)) / 2) * IMAGE_W * BYTE_PER_PIXEL;
        data += (((IMAGE_W - (HEIGHT)) / 2) + i) * BYTE_PER_PIXEL;
        for (int j = x_offset; j < WIDTH + x_offset; j++) {
            uint8_t ur, ug, ub;
            int8_t r, g, b;
            uint32_t number;

            ub = (uint8_t)(data[j * BYTE_PER_PIXEL * IMAGE_W + 1] << 3);
            ug = (uint8_t)((data[j * BYTE_PER_PIXEL * IMAGE_W] << 5) |
                           ((data[j * BYTE_PER_PIXEL * IMAGE_W + 1] & 0xE0) >> 3));
            ur = (uint8_t)(data[j * BYTE_PER_PIXEL * IMAGE_W] & 0xF8);
            b = ub - 128;
            g = ug - 128;
            r = ur - 128;

            // Loading data into the CNN fifo
            while (((*((volatile uint32_t*)0x50000004) & 1)) != 0)
                ; // Wait for FIFO 0

            number = 0x00FFFFFF & ((((uint8_t)b) << 16) | (((uint8_t)g) << 8) | ((uint8_t)r));

            *((volatile uint32_t*)0x50000008) = number; // Write FIFO 0
            
        }
    }
#endif
    //LED_Off(1);
    
    //int cnn_load_time = utils_get_time_ms() - pass_time;
    //PR_DEBUG("CNN load data time : %d", cnn_load_time);

    pass_time = utils_get_time_ms();

    // Disable Deep Sleep mode
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    // CNN interrupt wakes up CPU from sleep mode
    while (cnn_time == 0) {
        asm volatile("wfi"); // Sleep and wait for CNN interrupt
		//asm volatile("nop");
    }

    //PR_DEBUG("CNN wait time : %d", utils_get_time_ms() - pass_time);

    
    get_priors();
    localize_objects();
    
}

#ifdef LP_MODE_ENABLE
static void ARM_low_power(int lp_mode)
{
    switch (lp_mode) {
        case 0:
            PR_DEBUG("Active\n");
            break;

        case 1:
            PR_DEBUG("Enter SLEEP\n");
            MXC_LP_EnterSleepMode();
            PR_DEBUG("Exit SLEEP\n");
            break;

        case 2:
            PR_DEBUG("Enter LPM\n");
            MXC_LP_EnterLowPowerMode();
            PR_DEBUG("Exit LPM\n");
            break;

        case 3:
            PR_DEBUG("Enter UPM\n");
            MXC_LP_EnterMicroPowerMode();
            PR_DEBUG("Exit UPM\n");
            break;

        case 4:
            PR_DEBUG("Enter STANDBY\n");
            MXC_LP_EnterStandbyMode();
            PR_DEBUG("Exit STANDBY\n");
            break;

        case 5:
            PR_DEBUG("Enter BACKUP\n");
            MXC_LP_EnterBackupMode();
            PR_DEBUG("Exit BACKUP\n");
            break;

        case 6:
            PR_DEBUG("Enter POWERDOWN, disable WUT\n");
            MXC_WUT_Disable();
            MXC_Delay(SEC(2));
            MXC_LP_EnterPowerDownMode();
            PR_DEBUG("Exit SHUTDOWN\n");
            break;

        default:
            PR_DEBUG("Enter SLEEP\n");
            MXC_LP_EnterSleepMode();
            PR_DEBUG("Exit SLEEP\n");
            break;
    }
}
#endif
