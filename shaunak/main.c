#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "board.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "mxc_sys.h"
#include "nvic_table.h"

#include "PCA9685.h"
#include "belt.h"

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
void openValve(pca9685_driver_t PCA9685,uint8_t num,int time){
    PCA9685.setPWM(num,0,valvePositions[num][0]);
    MXC_Delay(MXC_DELAY_MSEC(time));
    PCA9685.setPWM(num,0,valvePositions[num][1]);
}

int main()
{
    int error = 0;
    //Setup the I2CM
    error = MXC_I2C_Init(I2C_MASTER, 1, 0);

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
    while (1)
    {
        for(uint8_t i=0;i<9;i++){
            openValve(PCA9685,i,250);
            // MXC_Delay(MXC_DELAY_MSEC(2000));
            uint32_t count=getBeltPosition();
            printf("Value: %u\n",count);

        }
    }
    
    return 0;
}
