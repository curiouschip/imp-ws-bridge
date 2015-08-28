int serial_port_open(const char *port, speed_t speed, int parity) {
	int fd = open(port, O_RDWR);
	if (fd < 0) {
		return -1;
	}
	
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	
	if (tcgetattr(fd, &tty) != 0) {
		//eprintf("error %d from tcgetattr", errno);
		goto fail;
	}
	
	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		//eprintf ("error %d from tcsetattr", errno);
		goto fail;
	}
	
	return fd;

fail:
	close(fd);
	return -1;
}

int serial_write(int fd, void *data, size_t len) {
	return 0;
}