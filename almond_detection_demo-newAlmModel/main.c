
#define S_MODULE_NAME "main"

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "mxc.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "camera.h"
#include "icc.h"
#include "rtc.h"
#include "lp.h"
#include "cnn.h"
#include "MAXCAM_Debug.h"
#include "facedetection.h"
#include "post_process.h"
#include "tft_utils.h"
#include "servoController.h"
#include "PCA9685.h"
#include "belt.h"
#include "stdlib.h"

ServoController sc;

#define CONSOLE_BAUD 115200
#define COMM_BAUD 9600
#define COMM_UART 3

#define I2C_MASTER MXC_I2C1 // SCL P0_16; SDA P0_17
#define I2C_SCL_PIN 16
#define I2C_SDA_PIN 17
#define PCA9685_I2C_SLAVE_ADDR 0x40
#define I2C_FREQ 100000

uint16_t valvePositions[9][2]={
    {400,200},
    {380,180},
    {400,200},
    {380,180},
    {390,190},
    {390,190},
    {380,180},
    {370,170},
    {400,200},
};

#if 1 // Custom camera settings
static const uint8_t camera_settings[][2] = {
    {0x13, 0xEA},
    {0x15, 0x03},
    {0x00, 0x00},
    {0x10, 0x0F},
    {0x0F, 0x07},
    {0x0e, 0x08}, // Sleep mode
    {0x69, 0x52}, // BLC window selection, BLC enable (default is 0x12)
    {0x1e, 0xb3}, // AddLT1F (default 0xb1)
    {0x48, 0x42},
    {0xff, 0x01}, // Select MIPI register bank
    {0xb5, 0x30},
    {0xff, 0x00}, // Select system control register bank
    {0x16, 0x03}, // (default)
    {0x62, 0x10}, // (default)
    {0x12, 0x01}, // Select Bayer RAW
    {0x17, 0x65}, // Horizontal Window Start Point Control (LSBs), default is 0x69
    {0x18, 0xa4}, // Horizontal sensor size (default)
    {0x19, 0x0c}, // Vertical Window Start Line Control (default)
    {0x1a, 0xf6}, // Vertical sensor size (default)
    {0x37, 0x04}, // PCLK is double system clock (default is 0x0c)
    {0x3e, 0x20}, // (default)
    {0x81, 0x3f}, // sde_en, uv_adj_en, scale_v_en, scale_h_en, uv_avg_en, cmx_en
    {0xcc, 0x02}, // High 2 bits of horizontal output size (default)
    {0xcd, 0x80}, // Low 8 bits of horizontal output size (default)
    {0xce, 0x01}, // Ninth bit of vertical output size (default)
    {0xcf, 0xe0}, // Low 8 bits of vertical output size (default)
    {0x82, 0x01}, // 01: Raw from CIP (default is 0x00)
    {0xc8, 0x02},
    {0xc9, 0x80},
    {0xca, 0x01},
    {0xcb, 0xe0},
    {0xd0, 0x28},
    {0x0e, 0x00}, // Normal mode (not sleep mode)
    {0x70, 0x00},
    {0x71, 0x34},
    {0x74, 0x28},
    {0x75, 0x98},
    {0x76, 0x00},
    {0x77, 0x64},
    {0x78, 0x01},
    {0x79, 0xc2},
    {0x7a, 0x4e},
    {0x7b, 0x1f},
    {0x7c, 0x00},
    {0x11, 0x01}, // CLKRC, Internal clock pre-scalar divide by 2 (default divide by 1)
    {0x20, 0x00}, // Banding filter (default)
    {0x21, 0x57}, // Banding filter (default is 0x44)
    {0x50, 0x4d},
    {0x51, 0x40}, // 60Hz Banding AEC 8 bits (default 0x80)
    {0x4c, 0x7d},
    {0x0e, 0x00},
    {0x80, 0x7f},
    {0x85, 0x00},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x2a},
    {0x8a, 0x22},
    {0x8b, 0x20},
    {0xbb, 0xab},
    {0xbc, 0x84},
    {0xbd, 0x27},
    {0xbe, 0x0e},
    {0xbf, 0xb8},
    {0xc0, 0xc5},
    {0xc1, 0x1e},
    {0xb7, 0x05},
    {0xb8, 0x09},
    {0xb9, 0x00},
    {0xba, 0x18},
    {0x5a, 0x1f},
    {0x5b, 0x9f},
    {0x5c, 0x69},
    {0x5d, 0x42},
    {0x24, 0x78}, // AGC/AEC
    {0x25, 0x68}, // AGC/AEC
    {0x26, 0xb3}, // AGC/AEC
    {0xa3, 0x0b},
    {0xa4, 0x15},
    {0xa5, 0x29},
    {0xa6, 0x4a},
    {0xa7, 0x58},
    {0xa8, 0x65},
    {0xa9, 0x70},
    {0xaa, 0x7b},
    {0xab, 0x85},
    {0xac, 0x8e},
    {0xad, 0xa0},
    {0xae, 0xb0},
    {0xaf, 0xcb},
    {0xb0, 0xe1},
    {0xb1, 0xf1},
    {0xb2, 0x14},
    {0x8e, 0x92},
    {0x96, 0xff},
    {0x97, 0x00},
    {0x14, 0x3b}, // AGC value, manual, set banding (default is 0x30)
    {0x0e, 0x00},
    {0x0c, 0xd6},
    {0x82, 0x3},
    {0x11, 0x00}, // Set clock prescaler
    {0x12, 0x6},
    {0x61, 0x0},
    {0x64, 0x11},
    {0xc3, 0x80},
    {0x81, 0x3f},
    {0x16, 0x3},
    {0x37, 0xc},
    {0x3e, 0x20},
    {0x5e, 0x0},
    {0xc4, 0x1},
    {0xc5, 0x80},
    {0xc6, 0x1},
    {0xc7, 0x80},
    {0xc8, 0x2},
    {0xc9, 0x80},
    {0xca, 0x1},
    {0xcb, 0xe0},
    {0xcc, 0x0},
    {0xcd, 0x40}, // Default to 64 line width
    {0xce, 0x0},
    {0xcf, 0x40}, // Default to 64 lines high
    {0x1c, 0x7f},
    {0x1d, 0xa2},
    {0xee, 0xee} // End of register list marker 0xee
};
#endif

mxc_uart_regs_t *CommUart;

// *****************************************************************************
void WUT_IRQHandler()
{
    MXC_WUT_IntClear();
}

uint32_t ticks_1;
uint32_t ticks_2;
mxc_wut_cfg_t cfg;


void openValve(pca9685_driver_t PCA9685,uint8_t num,int time){
    PCA9685.setPWM(num,0,valvePositions[num][0]);
    //MXC_Delay(MXC_DELAY_MSEC(time));
   // PCA9685.setPWM(num,0,valvePositions[num][1]);
   for(int i = 0; i < 10; i++){
    if(i != num){
        PCA9685.setPWM(i,0,valvePositions[i][0]);
    }
   }
}

int main(void)
{
    int ret = 0;
    int slaveAddress;
    int id;
    int dma_channel;
    mxc_uart_regs_t *ConsoleUart;

#ifdef BOARD_FTHR_REVA
    // Wait for PMIC 1.8V to become available, about 180ms after power up.
    MXC_Delay(200000);
#endif
    /* Enable cache */
    MXC_ICC_Enable(MXC_ICC0);

    /* Set system clock to 100 MHz */
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    ConsoleUart = MXC_UART_GET_UART(CONSOLE_UART);

    if ((ret = MXC_UART_Init(ConsoleUart, CONSOLE_BAUD, MXC_UART_IBRO_CLK)) != E_NO_ERROR)
    {
        PR_ERR("UART1 Init Error: %d\n", ret);
        return ret;
    }

    CommUart = MXC_UART_GET_UART(COMM_UART);
#if (COMM_UART == 3)
    if ((ret = MXC_UART_Init(CommUart, COMM_BAUD, MXC_UART_ERTCO_CLK)) != E_NO_ERROR)
    {
        PR_ERR("UART3 Init Error: %d\n", ret);
        return ret;
    }

    // Enable ERTCO clock for sleep mode
    MXC_MCR->ctrl |= MXC_F_MCR_CTRL_ERTCO_EN;
#endif

    // Enable peripheral, enable CNN interrupt, turn on CNN clock
    // CNN clock: 50 MHz div 1
    cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);
    cnn_init();         // Bring CNN state machine into consistent state
    cnn_load_weights(); // Load CNN kernels
    cnn_load_bias();    // Load CNN bias
    cnn_configure();    // Configure CNN state machine

    /* Initialize RTC */
    MXC_RTC_Init(0, 0);
    MXC_RTC_Start();

    // Initialize DMA for camera interface
    MXC_DMA_Init();
    dma_channel = MXC_DMA_AcquireChannel();

#ifdef BOARD_FTHR_REVA
    /* Enable camera power */
    Camera_Power(POWER_ON);
    MXC_Delay(300000);
    PR_DEBUG("\n\nFace Detection Feather Demo\n");
#endif

    // Initialize the camera driver.
    camera_init(CAMERA_FREQ);

    // Obtain the I2C slave address of the camera.
    slaveAddress = camera_get_slave_address();
    PR_DEBUG("Camera I2C slave address is %02x\n", slaveAddress);

    // Obtain the product ID of the camera.
    ret = camera_get_product_id(&id);

    if (ret != STATUS_OK)
    {
        PR_ERR("Error returned from reading camera id. Error %d\n", ret);
        return -1;
    }

    PR_DEBUG("Camera Product ID is %04x\n", id);

    // Obtain the manufacture ID of the camera.
    ret = camera_get_manufacture_id(&id);

    if (ret != STATUS_OK)
    {
        PR_ERR("Error returned from reading camera id. Error %d\n", ret);
        return -1;
    }

    PR_DEBUG("Camera Manufacture ID is %04x\n", id);

#if 1
    // set camera registers with custom values
    for (int i = 0; (camera_settings[i][0] != 0xee); i++)
    {
        camera_write_reg(camera_settings[i][0], camera_settings[i][1]);
    }
#endif

    // Setup the camera image dimensions, pixel format and data acquiring details.
    ret = camera_setup(IMAGE_XRES, IMAGE_YRES, PIXFORMAT_RGB565, FIFO_FOUR_BYTE, STREAMING_DMA,
                       dma_channel);
    if (ret != STATUS_OK)
    {
        PR_ERR("Error returned from setting up camera. Error %d\n", ret);
        return -1;
    }

#ifdef BOARD_FTHR_REVA
#ifdef ROTATE_FEATHER_BOARD
    // camera_set_vflip(0);
    camera_set_hmirror(0);
#else
    camera_set_vflip(0);
#endif
#endif
#ifdef BOARD_EVKIT_V1
    PR_DEBUG("CAM MIR");
    camera_set_vflip(0);
#endif
#ifdef TFT_ENABLE

#ifdef BOARD_FTHR_REVA
    /* Initialize TFT display */
    MXC_TFT_Init(MXC_SPI0, 1, NULL, NULL);
#ifdef ROTATE_FEATHER_BOARD
    MXC_TFT_SetRotation(ROTATE_0);
#else
    MXC_TFT_SetRotation(ROTATE_180);
#endif
    MXC_TFT_SetBackGroundColor(4);
    MXC_TFT_SetForeGroundColor(WHITE); // set font color to white
#endif
#endif

    int error = 0;
    //Setup the I2CM
    error = MXC_I2C_Init(I2C_MASTER, 1, 0);
    init_servo_controller(&sc);
    setController(&sc);
    if (error != E_NO_ERROR) {
        printf("-->Failed master\n");
        return error;
    } else {
        printf("\n-->I2C Master Initialization Complete\n");
    }
    MXC_I2C_SetFrequency(I2C_MASTER, I2C_FREQ);
    pca9685_driver_t PCA9685 = PCA9685_Open();
    PCA9685.init(I2C_MASTER,PCA9685_I2C_SLAVE_ADDR);
    PCA9685.setPWMFreq(50);
    
    initializeBelt();
    // moveBeltDistance(6,100);
    setBeltSpeed(5);
     for(int i = 0; i < 10; i++){
        PCA9685.setPWM(i,0,valvePositions[i][1]);
   }

    //Add some vals to belt
   /*  int num_queues_to_append_to = 1;
    float valuesx[3] = {.1, .5, .76};
    float valuesy[3] = {.25, .5, .8};
   for (int i = 0; i < num_queues_to_append_to; i++) {
        for (int j = 0; j < 3; j++) {
            add_Object_To_Queue(&sc,valuesx[j],valuesy[j],getBeltPosition());
        }
    }
*/
 
    // // Setup NMS algorithm memory
    nms_memory_init();

    draw_image_rectangle();




    while (1)
    {
        uint32_t count = getBeltPosition();
       // printf("Encoder Count: %u\n",count);
        int *indices;
        int numberOfServosOn=0;
        indices = check_for_Encoder_Event(&sc, count, &numberOfServosOn);
        //printf("In helper_function, %d queues have top values greater than %d:\n",numberOfServosOn, count);
       /* for (int k = 0; k < numberOfServosOn; k++) {
            printf("opening valve: %u\n",count);
            openValve(PCA9685,indices[k],250);
        }
        */
        free(indices);
        // face_detection();
        MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN);
        run_cnn(0,0);
    }

    return 0;
}
