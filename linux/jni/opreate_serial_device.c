#include <jni.h>

#define LOG_TAG "com.ipaloma.scannerdemo"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__);
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__);

#define SCANNER_DEV_NOTE	"/dev/ttyMT1"
#define MAX_CODE_LENGHT		256

// receive tty data
JNIEXPORT jstring JNICALL Java_com_ipaloma_scannerdemo_MainActivity_receiveTTyData(JNIEnv * env, jobject jobj)
{
	int scanner_fd = open_serial(SCANNER_DEV_NOTE);
	if(scanner_fd < 0) {
		LOGE("%s:%d  open_serial(%s) failed:%s", __func__, __LINE__, SCANNER_DEV_NOTE, strerror(errno));
		return "null";
	}
	LOGD("%s:%d The scannser file id:%d", __func__, __LINE__, scanner_fd);
	int ret_val = setup_port(scanner_fd, 9600, 8, 'N', 1);
	if(ret_val < 0) {
		LOGE("setup_port() failed:%d", ret_val);
		goto CLOSE_FILE_TAG;
	}
	ioctl(scanner_fd, 121, 1);
	ioctl(scanner_fd, 121, 0);

	char code_buf[MAX_CODE_LENGHT];
	memset(code_buf, 0, sizeof(code_buf));
	int nread = 0, sum_read = 0;
	while((nread=read(scanner_fd, code_buf+sum_read, MAX_CODE_LENGHT-sum_read)) > 0 && sum_read < MAX_CODE_LENGHT ) {
		sum_read += nread;
		LOGD("GET CODE: nread=%d  %s", nread, code_buf);
	}
	if(sum_read > 0) {
		close_serial(scanner_fd);
		return (*env)->NewStringUTF(env, code_buf);
	}

CLOSE_FILE_TAG:
	close_serial(scanner_fd);
	return (*env)->NewStringUTF(env, "null");
}

int close_serial(int fd)
{
	// turn serial power off
	ioctl(fd, 220, 0);
	if(close(fd) != 0) {
		LOGE("close() failed:%s", strerror(errno));
		return -1;
	}
	return 0;
}

int open_serial(const char *dev_note)
{
	/*The O_NOCTTY flag tells UNIX that this program doesn't want to be the "controlling terminal" for that port.
	 *If you don't specify this then any input (such as keyboard abort signals and so forth) will affect your process.
	 *If Programs like getty(1M/8) use this feature when starting the login process, but normally a user program does not want this behavior.
	 *
	 *The O_NDELAY flag tells UNIX that this program doesn't care what state the DCD signal line is in - whether the other end of the port is up and running.
	 *If you do not specify this flag, your process will be put to sleep until the DCD signal line is the space voltage.
	 */
	LOGD("%s:%d open file:%s", __func__, __LINE__, dev_note);
	/* After test open should without O_NONBLOCK, because the scanner need little time to light on and then to read */
	int file_d = open(dev_note, O_RDWR | O_NOCTTY /*| O_NONBLOCK*/);
	if(file_d == -1) {
	    LOGE("%s:%d open(%s) failed:%s", __func__, __LINE__, dev_note, strerror(errno));
	    return -1;
	}
	// turn serial power on
	/* WARNING: the ioctl() for scanner dosen't return 0 when success
	 * 			so IGNORE the return value for ioctl().
	 */
	ioctl(file_d, 220, 1);
	// set block read/write
	//fcntl(file_d, F_SETFL, 0);

	return file_d;
}
/*
 * speed: 115200, 57600, 9600, 38400, 19200, 4800
 * data_bits: 8, 7
 * parity: N/n, E/e, O/o
 * stop_bits: 1, 2
 */
int setup_port(int fd, int speed, int data_bits, int parity, int stop_bits)
{
    int speed_arr[] = {B115200,B57600, B9600, B38400, B19200, B4800};
    int name_arr[] = {115200,57600, 9600, 38400, 19200, 4800};
    struct termios opt;
    int ret=-1;
    int i=0;
    int len=0;

    ret = tcgetattr(fd, &opt); /* get the port attr */ 
    if (ret < 0) {
        LOGD("Unable to get the attribute");
        return -1;
    }
     /* baud rate */ 
    len = sizeof(speed_arr) / sizeof(int);
    for (i = 0; i < len; i++) {
        if (speed == name_arr[i]) {
            cfsetispeed(&opt, speed_arr[i]);
            cfsetospeed(&opt, speed_arr[i]);
            break;
        }
    }
    if (i == len) {
        LOGD("Unsupported baud rate.");
		return -1;
	}
    /* enable the receiver, set local mode */
    opt.c_cflag |= (CLOCAL | CREAD);
    /* mask the character size bits*/
    opt.c_cflag &= ~CSIZE;
     /* data bits */ 
    switch (data_bits)
    {
    case 8:
        opt.c_cflag |= CS8;
        break;
    case 7:
        opt.c_cflag |= CS7;
        break;
    default:
        LOGD("Unsupported data bits.");
		return -2;
    }

    /* parity bits */
    /*
     * INPCK	Enable parity check
     * PARENB	Enable parity bit
     * ISTRIP	Strip parity bits
     * PARODD	Use odd parity instead of even
     *
     */
    switch (parity) {
    case 'N':
    case 'n':
    case 'S':
    case 's':
        opt.c_cflag &= ~PARENB;
        opt.c_cflag &= ~INPCK;
        break;
    case 'O':
    case 'o':
        opt.c_cflag |= (INPCK|ISTRIP);
        opt.c_cflag |= (PARODD | PARENB);
        break;
    case 'E':
    case 'e':
        opt.c_cflag|=(INPCK|ISTRIP);
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
        break;
    default:
        LOGD("Unsupported parity bits.");
		return -3;
    }
    /* stop bits */
    /*
     * CSTOPB	2 stop bits (1 otherwise)
     */
    switch (stop_bits) {
    case 1:
        opt.c_cflag &= ~CSTOPB;
        break;
    case 2:
        opt.c_cflag |= CSTOPB;
        break;
    default:
        LOGD("Unsupported stop bits.");
		return -4;
    }
    /*
     * ONLCR	Map NL to CR-NL
     * OCRNL	Map CR to NL
     * ICRNL	Map CR to NL
     * INLCR	Map NL to CR
     * IXON		Enable software flow control (outgoing)
     * IXOFF	Enable software flow control (incoming)
     * IXANY	Allow any character to start flow again
     */
    opt.c_oflag &= ~(ONLCR | OCRNL);
    opt.c_iflag &= ~(ICRNL | INLCR);
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* raw input */
    /*
     * ICANON	Enable canonical input (else raw)
     * ECHO		Enable echoing of input characters
     * ECHOE	Echo erase character as BS-SP-BS
     * ISIG		Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
     */
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* raw output */
    /*
     * OPOST	Postprocess output (not set = raw output)
     */
    opt.c_oflag &= ~OPOST;

    /*
     * VTIME specifies the amount of time to wait for incoming characters in tenths of seconds
     */
    opt.c_cc[VTIME] = 3; /* After testing if VTIME=0 the scanner will not light on */
    opt.c_cc[VMIN] = 0;  /* minimum number of characters to read */

    /* flushes the input and/or output queue. */
    tcflush(fd, TCIFLUSH);

    ret = tcsetattr(fd, TCSANOW, &opt);  /* update it now */ 
    if (ret < 0) {
        LOGD("Unable to setup the port.");
		return -5;
	}
    return 0;
}









