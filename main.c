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

#include "serial.inc.c"
#include "imp.inc.c"
#include "web.inc.c"

const char *DEFAULT_HTTP_PORT = "10111";

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "usage: imp-bridge serial-device [http-port]\n");
        return 1;
    }

    const char *serial_device = argv[1];
    const char *http_port = (argc > 2) ? argv[2] : DEFAULT_HTTP_PORT;

	int fd = serial_port_open(serial_device, B115200, 0);
	if (fd < 0) {
		ERROR("error initialising serial port");
		return 1;
	}
	
	int imp_status = imp_init(fd);
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
		"listening_ports", http_port,
		NULL
	};

	mg_start(&callbacks, 0, server_options);
	DLOG("HTTP listening on port %s", http_port);

	while (1) {
		sleep(1);
	}

	return 0;
	
}
