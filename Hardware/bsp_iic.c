#include "bsp_siic.h"
#include "board.h"

#define I2C_TIMEOUT_MS  (10)


void mspm0_get_clock_ms(unsigned long* time)
{
	*time = 0;
}

static int mspm0_i2c_disable(void)
{
    DL_I2C_reset(I2C_0_INST);
    DL_GPIO_initDigitalOutput(GPIO_I2C_0_IOMUX_SCL);
    DL_GPIO_initDigitalInputFeatures(GPIO_I2C_0_IOMUX_SDA,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_I2C_0_SCL_PORT, GPIO_I2C_0_SCL_PIN);
    DL_GPIO_enableOutput(GPIO_I2C_0_SCL_PORT, GPIO_I2C_0_SCL_PIN);
    return 0;
}

static int mspm0_i2c_enable(void)
{
    DL_I2C_reset(I2C_0_INST);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_0_IOMUX_SDA,
        GPIO_I2C_0_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_0_IOMUX_SCL,
        GPIO_I2C_0_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_0_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_0_IOMUX_SCL);
    DL_I2C_enablePower(I2C_0_INST);
    SYSCFG_DL_I2C_0_init();
    return 0;
}

void mpu6050_i2c_sda_unlock(void)
{
    uint8_t cycleCnt = 0;
    mspm0_i2c_disable();
    do
    {
        DL_GPIO_clearPins(GPIO_I2C_0_SCL_PORT, GPIO_I2C_0_SCL_PIN);
        delay_ms(1);
        DL_GPIO_setPins(GPIO_I2C_0_SCL_PORT, GPIO_I2C_0_SCL_PIN);
        delay_ms(1);

        if(DL_GPIO_readPins(GPIO_I2C_0_SDA_PORT, GPIO_I2C_0_SDA_PIN))
            break;
    }while(++cycleCnt < 100);
    mspm0_i2c_enable();
}

int mspm0_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data)
{
    unsigned int cnt = length;
    unsigned char const *ptr = data;
    unsigned long start, cur;

    if (!length)
        return 0;

    mspm0_get_clock_ms(&start);

    DL_I2C_transmitControllerData(I2C_0_INST, reg_addr);
    DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_startControllerTransfer(I2C_0_INST, slave_addr, DL_I2C_CONTROLLER_DIRECTION_TX, length+1);

    do {
        unsigned fillcnt;
        fillcnt = DL_I2C_fillControllerTXFIFO(I2C_0_INST, ptr, cnt);
        cnt -= fillcnt;
        ptr += fillcnt;

        mspm0_get_clock_ms(&cur);
        if(cur >= (start + I2C_TIMEOUT_MS))
        {
            mpu6050_i2c_sda_unlock();
            return -1;
        }
    } while (!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

    return 0;
}

int mspm0_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data)
{
    unsigned i = 0;
    unsigned long start, cur;

    if (!length)
        return -1;

    mspm0_get_clock_ms(&start);

    DL_I2C_transmitControllerData(I2C_0_INST, reg_addr);
    I2C_0_INST->MASTER.MCTR = I2C_MCTR_RD_ON_TXEMPTY_ENABLE;
    DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);

    while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_startControllerTransfer(I2C_0_INST, slave_addr, DL_I2C_CONTROLLER_DIRECTION_RX, length);

    do {
        if (!DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST))
        {
            uint8_t c;
            c = DL_I2C_receiveControllerData(I2C_0_INST);
            if (i < length)
            {
                data[i] = c;
                ++i;
            }
        }

    } while(!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE));

    while( !DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST) )
    {
        uint8_t c;
        c = DL_I2C_receiveControllerData(I2C_0_INST);
        if (i < length)
        {
            data[i] = c;
            ++i;
        }
    }

    I2C_0_INST->MASTER.MCTR = 0;
    DL_I2C_flushControllerTXFIFO(I2C_0_INST);

    if(i == length)
        return 0;
    else
        return -2;
        
}


static void HW_iic_init(void)
{
    if(DL_I2C_getSDAStatus(I2C_0_INST) == DL_I2C_CONTROLLER_SDA_LOW)
        mpu6050_i2c_sda_unlock();
}

static IIC_Status_t HW_IIC_Master_Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	return IIC_OK;
}

static IIC_Status_t HW_IIC_Master_Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    return IIC_OK;
}


static IIC_Status_t HW_IIC_Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint8_t state = 1;
	state = mspm0_i2c_write(DevAddress>>1,MemAddress,Size,pData);

    if( state==0 ) return IIC_OK;

	return IIC_ERR;
}


static IIC_Status_t HW_IIC_Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    int state = 1;
	state = mspm0_i2c_read(DevAddress>>1,MemAddress,Size,pData);
    
    if( state==0 ) return IIC_OK;

	return IIC_ERR;
}


static void HW_iic_delayms(uint16_t ms)
{
    delay_ms(ms);
}

//挂载驱动
IICInterface_t User_sIICDev = {
	.init = HW_iic_init,
	.write = HW_IIC_Master_Transmit , 
	.read = HW_IIC_Master_Receive ,
	.write_reg = HW_IIC_Mem_Write ,
	.read_reg = HW_IIC_Mem_Read ,
	.delay_ms = HW_iic_delayms
};
