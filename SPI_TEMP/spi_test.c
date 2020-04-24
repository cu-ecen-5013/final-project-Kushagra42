/*

Pin Configuration for temperature sensor:

P9_22 (SPI0_SCLK => SCLK) outputs a serial clock signal to synchronize communications.

P9_18 (SPI0_D1 => MOSI) outputs data to the SPI peripheral device.

P9_21 (SPIO_DO => MISO) receives data from the SPI peripheral device.

P9_17 (SPI0_CSO => CE0) enables one SPI peripheral device.

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "SPI.h"

/* Enable SPI Debug */
#define SPI_DBG

extern unsigned char Tx_spi[SPIDEV_BYTES_NUM];
extern unsigned char RX_spi[SPIDEV_BYTES_NUM];
int main()
{ int i = 0;
  unsigned char received_bytes=0;
    memset(Tx_spi, 0, sizeof(Tx_spi));
    memset(RX_spi, 0, sizeof(RX_spi));
    Tx_spi[0] = 0x05;
    Tx_spi[1] = 0x00;

    if (SPI_DEV1_init(SPIDEV_BYTES_NUM, SPIDEV1_BUS_SPEED_HZ, SPI_SS_LOW,
                      SPIDEV_DELAY_US, SPIDEV_DATA_BITS_NUM,
                      SPI_MODE0) == -1)
        printf("(Main)spidev1.0 initialization failed\r\n");

    else
        printf("(Main)spidev1.0 initialized - READY\r\n");

#ifdef SPI_DBG
    while (1)

    {
        if (SPIDEV1_transfer(Tx_spi, RX_spi, NO_OF_BYTES) == 0)
            printf("(Main)spidev1.0: Transaction Complete\r\n");

        else
            printf("(Main)spidev1.0: Transaction Failed\r\n");
	
	received_bytes = (SPIDEV1_single_transfer(Tx_spi, RX_spi, NO_OF_BYTES) == 0)
	if(received_bytes != 0)
	{
		printf("Transaction failed");
	}
	else
	{
		printf("Received bytes are: %s", received_bytes);
	}
            
        usleep(100000);
        printf("%d\r\n", ++i);
    }
#endif
}
