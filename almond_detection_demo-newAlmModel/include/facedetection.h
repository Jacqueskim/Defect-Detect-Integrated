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

#ifndef _FACEDETECTION_H_
#define _FACEDETECTION_H_

// Feather board default orientation: horizontal, camera at the top
#define ROTATE_FEATHER_BOARD	// rotate 180 degree, camera at the bottom

#define CAMERA_FREQ  (10 * 1000 * 1000)

//#define LP_MODE_ENABLE
#define LP_MODE 4 // 0:NO SLEEP, 1:SLEEP, 2:LPM, 3:UPM, 4:STANDBY, 5:BACKUP, 6:POWERDOWN

#define TFT_WIDTH  320
#define TFT_HEIGHT 240

#define CAMERA_IMAGE_XRES 300//224
#define CAMERA_IMAGE_YRES 168//168
#define IMAGE_SIZE_X 168
#define IMAGE_SIZE_Y 224

#define HEIGHT      224
#define WIDTH       168
#define THICKNESS   4
#define IMAGE_H     168
#define IMAGE_W     224
#define FRAME_COLOR 0x535A
#define X_OFFSET    CAMERA_IMAGE_XRES - IMAGE_W

#define LCD_BYTE_PER_PIXEL 2  //RGB565

#define X_START (TFT_HEIGHT - IMAGE_SIZE_X)/2 //72
#define Y_START (TFT_WIDTH - IMAGE_SIZE_Y)/2  //0


#define BYTE_PER_PIXEL 2

#define LOW_LIGHT_THRESHOLD 20

// Data input: HWC (little data): 160x120x3
#define DATA_SIZE_IN (224 * 168 * 3)


int face_detection(void);
uint8_t get_offset(void);
#endif // _FACEDETECTION_H_
