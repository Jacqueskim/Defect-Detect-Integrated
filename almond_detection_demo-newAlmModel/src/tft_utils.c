/*******************************************************************************
 * Copyright (C) 2022 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
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
 *******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "mxc_device.h"
#include "board.h"
#include "mxc_delay.h"
#include "rtc.h"
#include "uart.h"
#include "tft_utils.h"
#include "facedetection.h"

#define color_box ((255 & 0b11111000) << 8) | ((255 & 0b11111100) << 3) | (0 >> 3)

#define color_image_box ((0 & 0b11111000) << 8) | ((255 & 0b11111100) << 3) | (0 >> 3)
void draw_image_rectangle(void)
{
    #ifdef TFT_ENABLE
        #ifdef BOARD_FTHR_REVA
            MXC_TFT_Rectangle(X_START-2, Y_START-2, X_START+IMAGE_SIZE_X+2, Y_START+IMAGE_SIZE_Y+2, color_image_box);
        #endif
        #ifdef BOARD_EVKIT_V1
            int r = 0, g = 0, b = 0;
            int32_t color;
            int y1 = X_START-2;
            int x1 = Y_START-2;
            int y2 = X_START+IMAGE_SIZE_X+2;
            int x2 = Y_START+IMAGE_SIZE_Y+2;
            r = 253;
            g = 172;;
            b = 0;
            color = (0x01000100 | ((b & 0xF8) << 13) | ((g & 0x1C) << 19) | ((g & 0xE0) >> 5) | (r & 0xF8));
            for (int x = x1; x < x2; ++x) {
            MXC_TFT_WritePixel(x , y1 , 1, 1, color);
            MXC_TFT_WritePixel(x , y2 , 1, 1, color);
        }

        for (int y = y1; y < y2; ++y) {
            MXC_TFT_WritePixel(x1 , y , 1, 1, color);
            MXC_TFT_WritePixel(x2 , y , 1, 1, color);
        }
        #endif
    #endif
}

void draw_obj_rect(float* xy, int class_idx, uint32_t w, uint32_t h, uint8_t scale)
{
#if 0
#ifdef TFT_ENABLE
    int r = 0, g = 0, b = 0;
    uint32_t color;

    if (class_idx == 0) {
        r = 255;//253;
        g = 255;//172;
        b = 0;//83;
    } else if (class_idx == 1) {
        r = 155;
        g = 183;
        b = 212;
    } 

	if (xy[0] < 0 || xy[1] < 0 ||xy[2] < 0 ||xy[3] < 0)
	{
		printf("%d  %d  %d  %d \n",xy[0],xy[1],xy[2],xy[3]);
		while(1);
	}
    int y2 = w * (1-xy[0]) + X_START;
    int x1 = h * xy[1] + Y_START;
    int y1 = w * (1-xy[2]) + X_START;
    int x2 = h * xy[3] + Y_START;

    //int x, y;
#ifdef BOARD_EVKIT_V1
    color = (0x01000100 | ((b & 0xF8) << 13) | ((g & 0x1C) << 19) | ((g & 0xE0) >> 5) | (r & 0xF8));
#endif
#ifdef BOARD_FTHR_REVA
    // Convert to RGB565
    color = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
#endif
	
	// Draw rectangle around detected face
    for (int x = x1; x < x2; ++x) {
        MXC_TFT_WritePixel(x * scale, y1 * scale, scale, scale, color);
        MXC_TFT_WritePixel(x * scale, y2 * scale, scale, scale, color);
    }

    for (int y = y1; y < y2; ++y) {
        MXC_TFT_WritePixel(x1 * scale, y * scale, scale, scale, color);
        MXC_TFT_WritePixel(x2 * scale, y * scale, scale, scale, color);
    }

#endif
#else
#ifdef TFT_ENABLE
    int x1 = w * xy[0] + X_START;
    int y1 = h * xy[1] + Y_START;
    int x2 = w * xy[2] + X_START - 1;
    int y2 = h * xy[3] + Y_START - 1;

	// Draw rectangle around detected face
	MXC_TFT_Rectangle(x1, y1, x2, y2, color_box);
#endif

#endif
}
