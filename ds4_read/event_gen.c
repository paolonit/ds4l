#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <signal.h>

void handler (int sig)
{
  printf ("\nexiting...(%d)\n", sig);
  exit (0);
}
 
void perror_exit (char *error)
{
  perror (error);
  handler (9);
}
 
int main (int argc, char *argv[])
{
	struct input_event ev[64];
	int fd, fd2, rd, value, size = sizeof (struct input_event);
	char *device = "/dev/input/by-id/usb-Sony_Computer_Entertainment_Wireless_Controller-event-joystick";
 
	/* Open DS4 input device */
	if ((fd = open (device, O_RDONLY)) == -1)
		printf ("%s is not a vaild device.\n", device);

	/* Generate input events */
	while (1) {
		if ((rd = read (fd, ev, size * 64)) < size)
          		perror_exit ("read()");
	}
 
	return 0;
}
