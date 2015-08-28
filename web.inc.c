internal const int WEB_RESPONSE_BUFFER_SIZE = 1024;
internal const char *web_error_404 = 
	"HTTP/1.1 404 Not Found\r\n"
	"Content-Type: text/plain\r\n"
	"Content-Length: %d\r\n"
	"\r\n"
	"%s";

int web_begin_request(struct mg_connection *conn) {
	struct mg_request_info *req = mg_get_request_info(conn);
	req->conn_data = 0;
	
	if (strcmp("/socket", req->uri) != 0 || !mg_is_websocket_request(conn)) {
		goto not_found;
	}
	
	return 0;

not_found:
	{
		const char *reply = "Not Found";
		const int reply_len = strlen(reply);
		mg_printf(conn, web_error_404, reply_len, reply);	
		return 1;
	}
}

void web_end_request(const struct mg_connection *conn, int reply_status_code) {
	DLOG("request ended");
	struct mg_request_info *req = mg_get_request_info((struct mg_connection*)conn);
	if (req->conn_data) {
		DLOG("freeing connection data");
		free(req->conn_data);
	}
}

int web_ws_connect(const struct mg_connection *conn) {
	struct mg_request_info *req = mg_get_request_info((struct mg_connection*)conn);
	struct client_state *state;
	state = malloc(sizeof(struct client_state) + WEB_RESPONSE_BUFFER_SIZE);
	if (state == 0) {
		DLOG("couldn't allocate memory for client");
		return 1;
	} else {
		state->response_buffer_sz = WEB_RESPONSE_BUFFER_SIZE;	
		req->conn_data = state;
		return 0;
	}
}

int web_ws_data(struct mg_connection *conn, int bits, char *data, size_t data_len) {
	int opcode = bits & 0x0F;
	if (opcode == WEBSOCKET_OPCODE_BINARY) {
		struct mg_request_info *req = mg_get_request_info(conn);
		struct client_state *state = (struct client_state*)req->conn_data;
		int response_len = imp_request(data, data_len, state->response_buffer, state->response_buffer_sz);
		if (response_len < 0) {
			// TODO: send error
		} else {
			mg_websocket_write(conn, WEBSOCKET_OPCODE_BINARY, state->response_buffer, response_len);
		}
	} else if (opcode == WEBSOCKET_OPCODE_PING) {
		DLOG("ping!");
		mg_websocket_write(conn, WEBSOCKET_OPCODE_PONG, NULL, 0);
	} else {
 		WARN("non-binary data received, ignored");
 	}
	return 1;
}