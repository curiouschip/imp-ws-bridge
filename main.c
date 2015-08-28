#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "mongoose.h"

#include "macros.h"
#include "types.h"

#include "globals.inc.c"
#include "serial.inc.c"
#include "imp.inc.c"
#include "web.inc.c"

const char *HTTP_PORT = "10111";

int main(int argc, char *argv[]) {

	// TODO(jwf): open serial port
	// imp_fd = serial_port_open("/dev/cu.usbserial", B115200, 0);
	// if (imp_fd < 0) {
	// 	//eprintf("error opening serial port");
	// 	return 1;
	// }
	
	int imp_status = imp_init(0);
	if (imp_status != 0) {
		ERROR("couldn't initialise IMP, error=%d", imp_status);
	}

	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = web_begin_request;
	callbacks.end_request = web_end_request;
	callbacks.websocket_connect = web_ws_connect;
	callbacks.websocket_data = web_ws_data;

	const char *server_options[] = {
		"listening_ports", HTTP_PORT,
		NULL
	};

	mg_start(&callbacks, 0, server_options);
	DLOG("HTTP listening on port %s", HTTP_PORT);

	while (1) {
		sleep(1);
	}

	return 0;
	
}