internal int imp_fd;
internal pthread_mutex_t imp_mutex;

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

int imp_request(char *req, int req_len, char *res, int res_sz) {
	if (pthread_mutex_lock(&imp_mutex) != 0) {
		return -1;
	}
	for (int i = 0; i < req_len; ++i) {
		res[req_len-i-1] = req[i];
	}
	pthread_mutex_unlock(&imp_mutex);
	return req_len;
}