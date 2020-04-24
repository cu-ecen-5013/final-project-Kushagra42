// Exploring Beaglebone - Derek Molly for I2C configuration

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define SLAVE_ADDR 0x40
int fd;

void signal_handler(int signum)
{
  assert(0 == close(fd));
  exit(signum);
}

int main(){


  char *bus = "/dev/i2c-1"; 
  int addr = SLAVE_ADDR;    
  char buf[2] = {0};
  int temp;
  unsigned char MSB, LSB;

  float f,c;

  if ((fd = open(bus, O_RDWR)) < 0) {
    
    perror("Failed to open the i2c bus");
    exit(1);
  }

 if (ioctl(fd, I2C_SLAVE, addr) < 0) {
   perror("Failed to acquire bus access and/or talk to slave.\n");
  
   exit(1);
 }

 signal(SIGINT, signal_handler);

 while(1)
   {
     
     if (read(fd,buf,2) != 2) {
      
       perror("Failed to read from the i2c bus.\n");

   } else {

       MSB = buf[0];
       LSB = buf[1];

       temp = ((MSB << 8) | LSB) >> 4;

       c = temp*0.0625;
       f = (1.8 * c) + 32;

       printf("Temp Fahrenheit: %f Celsius: %f\n", f, c);
     }
     sleep(1);
   }



}
