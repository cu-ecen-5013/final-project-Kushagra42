//https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=44972&p=372182#p372182
//https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=7500&sid=7c55b988e36afcaf2a01db466aaec9f7&start=25
//https://www.raspberrypi.org/forums/viewtopic.php?t=7500&p=93257
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int main(int argc, char ** argv) 
{
  int fd;
  // Open the Port. We want read/write, no "controlling tty" status, and open it no matter what state DCD is in
  fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
    perror("open_port: Unable to open /dev/ttyAMA0 - ");
    return(-1);
  }

  // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
  fcntl(fd, F_SETFL, 0);


  struct termios options;
  tcgetattr(fd, &options);

      /* input flags */
    options.c_iflag &= ~ IGNBRK;                       /* enable ignoring break */
    options.c_iflag &= ~(IGNPAR | PARMRK);             /* disable parity checks */
    options.c_iflag &= ~ INPCK;                        /* disable parity checking */
    options.c_iflag &= ~ ISTRIP;                       /* disable stripping 8th bit */
    options.c_iflag &= ~(INLCR | ICRNL);               /* disable translating NL <-> CR */
    options.c_iflag &= ~ IGNCR;                        /* disable ignoring CR */
    options.c_iflag &= ~(IXON | IXOFF);                /* disable XON/XOFF flow control */
    /* output flags */
    options.c_oflag  &= ~ OPOST;                        /* disable output processing */
    options.c_oflag  &= ~(ONLCR | OCRNL);               /* disable translating NL <-> CR */
    /* not for FreeBSD */
    options.c_oflag &= ~ OFILL;                        /* disable fill characters */
    /* control flags */
    options.c_cflag  |=   CLOCAL;                       /* prevent changing ownership */
    options.c_cflag  |=   CREAD;                        /* enable reciever */
    options.c_cflag  &= ~ PARENB;                       /* disable parity */
    options.c_cflag  &= ~ CSTOPB;                       /* disable 2 stop bits */
    options.c_cflag  &= ~ CSIZE;                        /* remove size flag... */
    options.c_cflag  |=   CS8;                          /* ...enable 8 bit characters */
    options.c_cflag  |=   HUPCL;                        /* enable lower control lines on close - hang up */
        /* local flags */
    options.c_lflag &= ~ ISIG;                         /* disable generating signals */
    options.c_lflag &= ~ ICANON;                       /* disable canonical mode - line by line */
    options.c_lflag &= ~ ECHO;                         /* disable echoing characters */
    options.c_lflag &= ~ ECHONL;                       /* ??? */
    options.c_lflag &= ~ NOFLSH;                       /* disable flushing on SIGINT */
    options.c_lflag &= ~ IEXTEN;                       /* disable input processing */

        /* control characters */
    memset(options.c_cc,0,sizeof(options.c_cc));
  int baud_in=cfsetispeed(&options, B115200);
  if (baud_in == -1)
  {
    perror("Error in setting input required baud rate\n ");
    return(-1);
  }
  int baud_out=cfsetospeed(&options, B115200);
  if (baud_out==-1)
  {
    perror("Error in setting output required baud rate\n ");
    return(-1);
  }  

  tcsetattr(fd, TCSANOW, &options);
  // Write to the port
  unsigned char tx_buffer[20];
  unsigned char *p_tx_buffer;
	
  p_tx_buffer = &tx_buffer[0];
  *p_tx_buffer++ = 'H';
  *p_tx_buffer++ = 'e';
  *p_tx_buffer++ = 'l';
  *p_tx_buffer++ = 'l';
  *p_tx_buffer++ = 'o';
//  int n = write("Hi testing uart functionality for raspi\n");
  int n = write(fd,&tx_buffer[0],(p_tx_buffer - &tx_buffer[0]));
  if (n < 0) 
  {
    perror("Write failure ");
    return -1;
  }

  // Read up to 255 characters from the port if they are there
  char buf[256];
  n = read(fd, (void*)buf, 255);
  if (n < 0) 
  {
    perror("Read failure ");
    return -1;
  }
  else if (n == 0) 
	printf("No data on port\n");
  else 
  {
    buf[n] = '\0';
    printf("%i bytes read : %s\n", n, buf);
  }

  // Don't forget to clean up
  close(fd);
  printf("End of uart test");
  return 0;
}
