#include "device_read.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <xdo.h>

char mapping[16][10];

int generate_mapping() {
	char line[20];
	char * key;
	int i;
	FILE * config = fopen("../key_map.config", "r");
	if(config == NULL) {
		fprintf(stderr, "Configuration file missing.");
		return(-1);
	}
	for(i = 0; i < 16; i++){
		fscanf(config, "%s", line);
		key = strtok(line, ":");
		key = strtok(NULL, ":");
		strcpy(mapping[i], key);
	}
	fclose(config);
	return(0);
}

void press_key(int face_buttons, int misc_buttons, xdo_t * x) {
	switch (face_buttons) {
		case UP_BTN: 
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[0], 0);
			break;
		case RIGHT_BTN: 
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[1], 0);
			break;
		case DOWN_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[2], 0);
			break;
		case LEFT_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[3], 0);
			break;
		case SQUARE_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[4], 0);
			break;
		case X_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[5], 0);
			break;
		case CIRCLE_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[6], 0);
			break;
		case TRIANGLE_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[7], 0);
			break;
	}

	switch (misc_buttons) {
		case L1_BTN: 
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[8], 0);
			break;
		case R1_BTN: 
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[9], 0);
			break;
		case L2_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[10], 0);
			break;
		case R2_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[11], 0);
			break;
		case SHARE_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[12], 0);
			break;
		case OPTIONS_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[13], 0);
			break;
		case L3_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[14], 0);
			break;
		case R3_BTN:
			xdo_send_keysequence_window(x, CURRENTWINDOW, mapping[15], 0);
			break;
	}
	
}

int main (void) {
	char buf[100];
	int fd;
	xdo_t * x = xdo_new(NULL);
	if(generate_mapping() != 0) {
		fprintf(stderr, "Error loading key configuration. Quitting...");
		return(-1);
	}
	while(1) {
		fd = open("/dev/ds4-device", O_RDONLY);
		read(fd, &buf, 7);
		if(buf[5] != 8 || buf[6] != 0) {
			press_key(buf[5], buf[6], x);
			//printf("\t");
			/*for(int i = 5; i < 7; i++) {
				for (int j = 0; j < 8; j++) {
					printf("%d", !!((buf[i] << j) & 0x80));
				}
				printf("\t");
			}*/
			while(buf[5] != 8 || buf[6] != 0){
				fd = open("/dev/ds4-device", O_RDONLY);
				read(fd, &buf, 7);
				close(fd);
			}
			
			//printf("\n");
			
		}
		
		
		close(fd);
		
	}
	xdo_free(x);
	return(0);
}




