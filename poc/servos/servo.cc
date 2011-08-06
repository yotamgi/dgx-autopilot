#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>
#include <time.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

void put(int servo, int angle);   // Moves servo (0-5) to angle (500-5500)
void waitMS (int ms); // This allows for a set system instruction pause. Note: 1000ms = 1 sec.
void neutral(); // This sets up all the servos neutral positions before use.

typedef unsigned int DWORD;
int mainfd = 0;   /* File descriptor for the port */

void waitMS (int ms) {
   clock_t endwait;
   endwait = clock() + ms * CLOCKS_PER_SEC/1000;
   while (clock() < endwait);
}
/*
* 'open_port()' - Open the serial port.
* Returns the file descriptor on success or -1 on error.
*/
int open_port (char portName[]) {
   int fd;   /* File descriptor for the port */
   fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);

   if (fd == -1) {  /* Could not open the port */
      fprintf(stderr, "\nopen port: Unable to open %s - %s\n\n", portName, strerror(errno));
      exit(EXIT_FAILURE); // EXIT
   }
   else {
      printf("\nopen port: port %s has been opened correctly.\n\n", portName);
      printf("fd = %i\n\n", fd);
   }

   return (fd);
}

void config_port(int mainfd) {
   struct termios options;

   // Get the current settings of the serial port.
   tcgetattr (mainfd, &options);

   // Set the read and write speed to 19200 BAUD.
   // All speeds can be prefixed with B as a settings.
   cfsetispeed (&options, B9600);
   cfsetospeed (&options, B9600);

   // Now to set the other settings. Here we use the no parity example. Both will assumme 8-bit words.

   // PARENB is enabled parity bit. This disables the parity bit.
   options.c_cflag &= ~PARENB;

   // CSTOPB means 2 stop bits, otherwise (in this case) only one stop bit.
   options.c_cflag &= ~CSTOPB;

   // CSIZE is a mask for all the data size bits, so anding with the negation clears out the current data size setting.
   options.c_cflag &= ~CSIZE;

   // CS8 means 8-bits per work
   options.c_cflag |= CS8;
}

void close_port(int mainfd, char portName[]) {
   /* Close the serial port */
   if (close(mainfd) == -1) {
      fprintf(stderr, "\n\nclose port: Unable to close %s - %s\n\n", portName, strerror(errno));
      exit(EXIT_FAILURE); // EXIT
   }
   else {
      printf("\n\nclose port: The port %s has been closed correctly.\n\n", portName);
      exit(EXIT_SUCCESS);
   }
}


int main(int argc, char* argv[]){
  if (argc != 2) {
      printf("USAGE: %s PORT. i.e. %s /dev/ttyACM1.\n", argv[0], argv[0]);
      exit(EXIT_FAILURE); // EXIT
   }

   mainfd = open_port(argv[1]);
   config_port(mainfd);

    waitMS(2000);   // Wait one second

    unsigned char serialBytes[5];
    int channel = 5;
    int target = 4000;

  int a;

	while(1) {
		for(target=4000; target < 8001; target++){
				printf("target is %d\n", target);
				for (int i=0; i<1000; i++ ) {
						int shaked_target = target + (((i*13)%500) - 250);
						//oscanf("%d", &target);
						//target = (target==4000)?8000:4000;

						serialBytes[0] = 0x40; // Command byte: Set Target.
						serialBytes[1] = 0x84; // Command byte: Set Target.
						serialBytes[2] = channel; // First data byte holds channel number.
						serialBytes[3] = 0x7f&shaked_target; // Second byte holds the lower 7 bits of target.
						serialBytes[4] = 0x7f&(shaked_target>>7);   // Third data byte holds the bits 7-13 of target.

						a = write(mainfd,serialBytes,5);
						//waitMS(1000);
				}
		}
	}


    close_port(mainfd, argv[1]);

}
