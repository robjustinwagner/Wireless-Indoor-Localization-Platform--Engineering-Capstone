/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * SENSORS.C
 */

/* INCLUDES */
#include "gen_lib.h"
#include "sensors.h"
#include "main.h"
#include "uart.h"

/* DEFINITIONS */


/* VARIABLES */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned char data1 = 0;
static unsigned int i = 0;
//static int acc_divider = 2048;

uint8_t response[100];
float accelerometer_data[3];

/*
 * main.c
 */
/*int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	UART_Init();
	SPI_Init();
	MPU_Init(0, 0);
	whoami();
	UART_Print("Init Complete: ", "y", true);


	while(1){
		//spi_mpu_read(MPUREG_ACCEL_CONFIG_2);
		read_acc(); // read Accelerometer data
		UART_Print("Read Val: ", &data, true);
		for(i = 0; i < 10000; i++);
	}

	return 0;
}*/

/*
 * SPI Init
 */

void spi_mpu_select() {
	P3OUT &= ~BIT0;
	__delay_cycles(200);
}

void spi_mpu_release() {
	P3OUT |= BIT0;
	__delay_cycles(200);
}

void MPU_Init(int sample_rate_div, int low_pass_filter){
#define MPU_InitRegNum 20

	uint8_t i = 0;
	uint8_t MPU_Init_Data[MPU_InitRegNum][2] =
	{
			{0x80, MPUREG_PWR_MGMT_1},     // Reset Device
			{0x01, MPUREG_PWR_MGMT_1},     // Clock Source
			{0x07, MPUREG_PWR_MGMT_2},     // Enable Acc & Gyro *Changed

			//{low_pass_filter, MPUREG_CONFIG},         // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
			{0x18, MPUREG_GYRO_CONFIG},    // +-2000dps
			{0x08, MPUREG_ACCEL_CONFIG},   // +-4G
			{0x09, MPUREG_ACCEL_CONFIG_2}, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz

			{0x30, MPUREG_INT_PIN_CFG},    //

			{0x40, MPUREG_INT_ENABLE}, // added
			{0xC0, MPUREG_MOT_DETECT_CTRL}, // added
			{0x06,MPUREG_MOT_THR}, // ??
			{0x04,MPUREG_LP_ACCEL_ODR}, // ??

			//{0x40, MPUREG_I2C_MST_CTRL},   // I2C Speed 348 kHz
			//{0x20, MPUREG_USER_CTRL},      // Enable AUX
			{0x20, MPUREG_USER_CTRL},       // I2C Master mode
			{0x0D, MPUREG_I2C_MST_CTRL}, //  I2C configuration multi-master  IIC 400KHz

			{AK8963_I2C_ADDR, MPUREG_I2C_SLV0_ADDR},  //Set the I2C slave addres of AK8963 and set for write.
			//{0x09, MPUREG_I2C_SLV4_CTRL},
			//{0x81, MPUREG_I2C_MST_DELAY_CTRL}, //Enable I2C delay

			{AK8963_CNTL2, MPUREG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
			{0x01, MPUREG_I2C_SLV0_DO}, // Reset AK8963
			{0x81, MPUREG_I2C_SLV0_CTRL},  //Enable I2C and set 1 byte

			{AK8963_CNTL1, MPUREG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
			{0x12, MPUREG_I2C_SLV0_DO}, // Register value to continuous measurement in 16bit
			{0x81, MPUREG_I2C_SLV0_CTRL}  //Enable I2C and set 1 byte

			//{0x20, MPUREG_PWR_MGMT_1} // added, CYCLE = 1
	};

	for(i=0; i < MPU_InitRegNum; i++)
	{
		spi_mpu_write(MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
		uint8_t j;
		for(j = 20; j > 0; j--);  //I2C must slow down the write speed, otherwise it won't work
	}

	set_acc_scale(BITS_FS_16G); // returns scale

	return;
}

/*-----------------------------------------------------------------------------------------------
                                ACCELEROMETER SCALE
usage: call this function at startup, after initialization, to set the right range for the
accelerometers. Suitable ranges are:
BITS_FS_2G
BITS_FS_4G
BITS_FS_8G
BITS_FS_16G
returns the range set (2,4,8 or 16)
-----------------------------------------------------------------------------------------------*/
unsigned int set_acc_scale(int scale)
{
    unsigned int temp_scale;
    spi_mpu_write(MPUREG_ACCEL_CONFIG, scale);

    switch (scale)
    {
        case BITS_FS_2G:
            acc_divider=16384;
        break;
        case BITS_FS_4G:
            acc_divider=8192;
        break;
        case BITS_FS_8G:
            acc_divider=4096;
        break;
        case BITS_FS_16G:
            acc_divider=2048;
        break;
    }

    temp_scale=spi_mpu_write(MPUREG_ACCEL_CONFIG|READ_FLAG, 0x00);
    switch (temp_scale)
    {
        case BITS_FS_2G:
            temp_scale=2;
        break;
        case BITS_FS_4G:
            temp_scale=4;
        break;
        case BITS_FS_8G:
            temp_scale=8;
        break;
        case BITS_FS_16G:
            temp_scale=16;
        break;
    }
    return temp_scale;
}

unsigned int whoami()
{
    return spi_mpu_write(MPUREG_WHOAMI|READ_FLAG, 0x00);
}

/*void SPI_Init(){

	int i;

	if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	{
		while(1);                               // do not load, trap CPU!!
	}
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;
	for(i=2100;i>0;i--);                      // Wait for DCO to stabilize.


	// Select SPI
	P2DIR |= BIT6; // SS_1_SEL - output (EEPROM)
	P3DIR |= BIT0; // SS_2_SEL - output
	P2OUT |= BIT6; // SS_1_SEL = high
	P3OUT |= BIT0; // SS_2_SEL = high

	P3SEL |= 0x0E;                            // P3.3,2,1 option select
	UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2;                     // SMCLK
	UCB0BR0 |= 0x02;                          // /2
	UCB0BR1 = 0;                              //
	IE2 &= (~UCB0TXIE | UCB0TXIE); // disable TX int
	UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	//IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

}*/
/*
 * (Originally spi_exchg)
 * Writes One Byte
 *
 * dataOut - data to be transmitted via SPI
 * return: dataIn - returns value from slave device
 * 		(Write dummy byte to receive data from slave)
 */
unsigned char spi_exchg(unsigned char dataOut)
{
	unsigned char dataIn = 0;

	while (!(IFG2 & UCB0TXIFG));
	UCB0TXBUF = dataOut;

	while (!(IFG2 & UCB0RXIFG));
	dataIn = UCB0RXBUF;

	return dataIn;
}

/*
 * MPU 9250 Write - Single byte
 * addr - Hex value for address of MPU to write
 * value - Hex value of DATA to write to MPU
 */
uint8_t spi_mpu_write(uint8_t addr, unsigned char value){

	uint8_t tempData;

	// Write Value (RD/~WR = [7], ADDR = [6:0])
	spi_mpu_select(); // CS Low
	spi_exchg(addr);                     // Transmit address [6:0]
	tempData = spi_exchg(value);						// Data [7:0]
	spi_mpu_release(); // CS high

	// wait
	unsigned int j;
	for(j = 50; j > 0; j--);

	return tempData;
}

/*
 * MPU 9250 READ - Single byte
 * addr - Hex value for address of EEPROM to write
 */
void spi_mpu_read(uint8_t addr){

	addr = addr | READ_FLAG; // READ FLAG = 0x80
	// READ
	spi_mpu_select(); // ~CS Low
	spi_exchg(addr);                     // Transmit address
	data1 = spi_exchg(0); // dummy byte, used to retrieve READ data
	spi_mpu_release(); // ~CS high

	// wait
	unsigned int j;
	for(j = 50; j > 0; j--);
}

/*
 * MPU 9250 READ - Multiple bytes
 * addr - Hex value for address of EEPROM to write
 */
void spi_mpu_mRead(uint8_t addr, uint8_t* buffer, unsigned int bytes){

	addr = addr | READ_FLAG; // READ FLAG = 0x80
	// READ
	spi_mpu_select(); // ~CS Low
	spi_exchg(addr);                     // Transmit address
	for(i = 0; i < bytes; i++)
	{
		buffer[i] = spi_exchg(0); // dummy byte, used to retrieve READ data
	}
	spi_mpu_release(); // ~CS high

	// wait
	unsigned int j;
	for(j = 50; j > 0; j--);
}

void read_acc()
{
    uint8_t response[6];
    int16_t bit_data;
    float data_temp;
    spi_mpu_mRead(MPUREG_ACCEL_XOUT_H, response, 6);
    for(i = 0; i < 3; i++) {
        bit_data=((int16_t)response[i*2]<<8)|response[i*2+1];
        data_temp=(float)bit_data;
        accelerometer_data[i]=data_temp/acc_divider;
    }
}

void collectSensorData(float* sensorData[])
{
	const static uint32_t wait_time = 1000000/SAMPLES_PER_DURATION;	// 1MHz CLK

	/* BEGIN MODIFIED VERSION OF read_acc() TO POPULATE sensorData[][]
		uint8_t response[6];
	    int16_t bit_data;
	    float data_temp;

	    unsigned int r;
	    unsigned int c;
	    for(r = 0; r < ROWS; r++) {
	    	spi_mpu_mRead(MPUREG_ACCEL_XOUT_H, response, 6);
		    for(c = 0; c < COLS; c++) {
		        bit_data=((int16_t)response[c*2]<<8)|response[c*2+1];
		        data_temp=(float)bit_data;
		        sensorData[r][c]=data_temp/acc_divider;
		    }
	    	for(c=wait_time;c>0;c--);	//wait in between samples
	    }
	 END MODIFIED VERSION OF read_acc() */
		unsigned int r;
		    unsigned int c;
		    for(r = 0; r < ROWS; r++) {
		    	read_acc();
			    for(c = 0; c < COLS; c++) {
			        sensorData[r][c]=accelerometer_data[c];
			    }
		    	for(c=wait_time;c>0;c--);	//wait in between samples
		    }

}
