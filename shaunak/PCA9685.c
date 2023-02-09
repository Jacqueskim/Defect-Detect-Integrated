#include "PCA9685.h"
/******************************* Globals *******************************/
static mxc_i2c_req_t req; ///< I2C request

/******************************* Functions *******************************/
static int i2c_transfer(mxc_i2c_req_t *req, uint8_t *txData, int txSize, uint8_t *rxData,
                        int rxSize)
{
    req->tx_buf = txData; // Write data buffer
    req->tx_len = txSize; // Number of bytes to write
    req->rx_buf = rxData; // Read data buffer
    req->rx_len = rxSize; // Number of bytes to read
    return MXC_I2C_MasterTransaction(req);
}

static int i2c_write(mxc_i2c_req_t *req, uint8_t *txData, int txSize)
{
    return i2c_transfer(req, txData, txSize, NULL, 0); // Create I2C write request
}

static int i2c_read(mxc_i2c_req_t *req, uint8_t *txData, uint8_t *rxData, int rxSize)
{
    return i2c_transfer(req, txData, 1, rxData, rxSize); // Create I2C read request
}

static void write8(mxc_i2c_req_t *req, uint8_t addr, uint8_t d) {
    uint8_t reg[]={addr,d};
    i2c_write(req,reg,2);
}
static uint8_t read8(mxc_i2c_req_t *req,uint8_t addr) {
    uint8_t reg;
    i2c_read(req,&addr,&reg,1);
    return reg;
}
static int pca9685_setPWMFreq(float freq){
    // Range output modulation frequency is dependant on oscillator
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

    float prescaleval = ((27000000 / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    uint8_t oldmode = read8(&req,PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    write8(&req,PCA9685_MODE1, newmode);                             // go to sleep
    write8(&req,PCA9685_PRESCALE, prescale); // set the prescaler
    write8(&req,PCA9685_MODE1, oldmode);
    MXC_Delay(MXC_DELAY_MSEC(5));
    // This sets the MODE1 register to turn on auto increment.
    // printf("Old %x\n", oldmode | MODE1_RESTART | MODE1_AI);
    write8(&req,PCA9685_MODE1, 0x20);
}
static int pca9685_setPWM(uint8_t num, uint16_t on,uint16_t off) {
    uint8_t bytes[5]={PCA9685_LED0_ON_L+4*num,on,on>>8,off,off>>8};
    i2c_write(&req,&bytes,5);
}
static int pca9685_init(mxc_i2c_regs_t *i2c,uint8_t addr){
    req.i2c = i2c;
    req.addr = addr;
    req.tx_buf = NULL;
    req.tx_len = 0;
    req.rx_buf = NULL;
    req.rx_len = 0;
    req.restart = 0;
    req.callback = NULL;
    // for(uint8_t i=0;i<255;i++){
    //     uint8_t regval;
    //     printf("\nValue of reg %d: %d\n",i,read8(&req,i));
    //     MXC_Delay(MXC_DELAY_MSEC(20));
    // }
}

pca9685_driver_t PCA9685_Open(void){
    pca9685_driver_t SD;
    SD.init = pca9685_init;
    SD.setPWM=pca9685_setPWM;
    SD.setPWMFreq=pca9685_setPWMFreq;
    return SD;
}