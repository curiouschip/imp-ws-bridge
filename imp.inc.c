internal int imp_fd;
internal pthread_mutex_t imp_mutex;

internal const int IMP_BUFFER_SZ = 4096;
internal const int IMP_MAX_MSG_LEN = (IMP_BUFFER_SZ - 2) / 2;
internal char imp_tx_buffer[IMP_BUFFER_SZ];

#define IMP_SER_MSG_START	0x7F
#define IMP_SER_MSG_END		0x7E
#define IMP_SER_MSG_ESCAPE	0x7D
#define IMP_SER_MSG_XOR		0x20

int imp_write(char *data, size_t len) {
	if (len > IMP_MAX_MSG_LEN) {
		ERROR("(imp) maximum message length exceeded");
		return -1;
	}
	DLOG("(imp) writing %zu bytes...", len);
	int pos = 0;
	imp_tx_buffer[pos++] = IMP_SER_MSG_START;
	char *end = data + len;
	while (data < end) {
		if (*data == IMP_SER_MSG_START
			|| *data == IMP_SER_MSG_END
			|| *data == IMP_SER_MSG_ESCAPE) {
			imp_tx_buffer[pos++] = IMP_SER_MSG_ESCAPE;
			imp_tx_buffer[pos++] = *data ^ IMP_SER_MSG_XOR;
		} else {
			imp_tx_buffer[pos++] = *data;
		}
		data++;
	}
	imp_tx_buffer[pos++] = IMP_SER_MSG_END;
	while (pos > 0) {
		ssize_t bytes_written = write(imp_fd, imp_tx_buffer, pos);
		if (bytes_written < 0) {
			return -1;
		}
		pos -= bytes_written;
	}
	DLOG("(imp) write complete");
	return 0;
}

#define IMP_BUFFER_WRITE(ch) \
	if (pos < buffer_sz) { \
		buffer[pos++] = (ch); \
	} else { \
		error = 1; \
	}

int imp_read(char *buffer, size_t buffer_sz) {
	DLOG("(imp) reading...");
	int state = 0, error = 0, pos;
	while (1) {
		char ch;
		ssize_t rs = read(imp_fd, &ch, 1);
		if (rs < 0) {
			ERROR("(imp) read error");
			return -1;
		}
		if (ch == IMP_SER_MSG_START) {
			state = 1;
			pos = 0;
			continue;
		}
		switch (state) {
			case 0:
				// nothing to do
				break;
			case 1:
				if (ch == IMP_SER_MSG_END) {
					if (error) {
						ERROR("(imp) read buffer size exceeded");
						return -1;
					} else {
						DLOG("(imp) %d bytes read", pos);
						return pos;
					}
				} else if (ch == IMP_SER_MSG_ESCAPE) {
					state = 2;
				} else {
					IMP_BUFFER_WRITE(ch);
				}
				break;
			case 2:
				IMP_BUFFER_WRITE(ch ^ IMP_SER_MSG_ESCAPE);
				state = 1;
				break;
		}
	}
}

int imp_request(char *req, int req_len, char *res, int res_sz) {
	if (pthread_mutex_lock(&imp_mutex) != 0) {
		ERROR("failed to acquire mutex");
		return -1;
	}
	if (imp_write(req, req_len) < 0) {
		ERROR("(imp) write failed");
		return -1;
	}
	int res_len = imp_read(res, res_sz);
	if (res_len < 0) {
		ERROR("(imp) read failed");
		return -1;
	}
	pthread_mutex_unlock(&imp_mutex);
	return res_len;
}

/*
 * initialise IMP
 *
 * fd should point to a preconfigured serial port
 */
int imp_init(int fd) {
	if (pthread_mutex_init(&imp_mutex, 0) != 0) {
		return 1;
	}
	imp_fd = fd;
	return 0;
}