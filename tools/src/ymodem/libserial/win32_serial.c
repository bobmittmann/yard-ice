/*
 * @file	win32_serial.c
 * @brief	Wind32 library serial wrapper
 * @author	Robinson Mittmann (bobmittmann@gmail.com)
 *
 */

#if defined(_WIN32) || defined(__CYGWIN__)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include <windows.h>

#include "serial.h"
#include "debug.h"

#define SERIAL_DEV_RX_BUF_LEN 128

/* termios serial device */
struct win_serial_drv {
	HANDLE hComm;
	DCB dcb;
	DCB save_dcb;
	COMMTIMEOUTS timeouts;
	COMMTIMEOUTS save_timeouts;
	struct {
		unsigned int pos;
		unsigned int cnt;
		uint8_t buf[SERIAL_DEV_RX_BUF_LEN];
	} rx;
};

int win_serial_send(struct win_serial_drv * drv, 
					const void * buf, unsigned int len)
{
	OVERLAPPED osWrite = {0};
	DWORD dwWritten;
	DWORD dwToWrite = len;
	DWORD dwRes;
	BOOL fRes;

	assert(drv != NULL);
	assert(buf != NULL);


	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
		// error creating overlapped event handle
		return FALSE;

	// Issue write.
	if (!WriteFile(drv->hComm, buf, dwToWrite, &dwWritten, &osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) { 
			// WriteFile failed, but isn't delayed. Report error and abort.
			fRes = FALSE;
		} else {
			// Write is pending.
			dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
			switch(dwRes) {
			// OVERLAPPED structure's event has been signaled. 
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(drv->hComm, &osWrite, 
										 &dwWritten, FALSE))
					fRes = FALSE;
				else
					// Write operation completed successfully.
					fRes = TRUE;
				break;
			default:
				// An error has occurred in WaitForSingleObject.
				// This usually indicates a problem with the
				// OVERLAPPED structure's event handle.
				fRes = FALSE;
				break;
			}
		}
	} else {
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);

	return fRes ? len : -1;
}

int win_serial_recv(struct win_serial_drv * drv, char * buf, 
					unsigned int max, unsigned int tmo_msec)
{
	OVERLAPPED osReader = {0};
	DWORD dwRead;
	DWORD dwRes;
	BOOL fRes;
	int cnt;
	int i;

	if (drv->rx.cnt == 0) {
		if (drv->timeouts.ReadTotalTimeoutConstant != tmo_msec) {
			drv->timeouts.ReadTotalTimeoutConstant = tmo_msec;

			if (!SetCommTimeouts(drv->hComm, &drv->timeouts)) {
				// Error setting time-outs.
				fprintf(stderr, "%s: SetCommTimeouts() failed!\n", __func__);
				fflush(stderr);
				return -1;
			}
		}


		// Create the overlapped event. Must be closed before exiting
		// to avoid a handle leak.
		osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (osReader.hEvent == NULL) {
			fprintf(stderr, "%s: CreateEvent() failed!\n", __func__);
			fflush(stderr);
			return -1;
			// Error creating overlapped event; abort.
		}

		// Issue read operation.
		if (!ReadFile(drv->hComm, drv->rx.buf, SERIAL_DEV_RX_BUF_LEN, 
					  &dwRead, &osReader)) {
			// Error in communications; report it.
			if (GetLastError() != ERROR_IO_PENDING) {
				// read not delayed?
				fprintf(stderr, "%s: not ERROR_IO_PENDING!\n", __func__);
				fflush(stderr);
				fRes = FALSE;
			} else {
				// Write is pending.
				dwRes = WaitForSingleObject(osReader.hEvent, INFINITE);
				switch(dwRes) {
					// OVERLAPPED structure's event has been signaled. 
				case WAIT_OBJECT_0:
					if (!GetOverlappedResult(drv->hComm, &osReader, 
											 &dwRead, FALSE)) {
						fprintf(stderr, "%s: GetOverlappedResult() failed!\n",
								__func__);
						fflush(stderr);
						fRes = FALSE;
					} else {
						// Read operation completed successfully.
						fRes = TRUE;
					}
					break;
				default:
					// An error has occurred in WaitForSingleObject.
					// This usually indicates a problem with the
					// OVERLAPPED structure's event handle.
					fRes = FALSE;
					fprintf(stderr, "%s: WaitForSingleObject() failed!\n", 
							__func__);
					fflush(stderr);
					break;
				}
			}
		} else {
			// WriteFile completed immediately.
			fRes = TRUE;
		}

		CloseHandle(osReader.hEvent);

		if (!fRes)
			return -1;

		if (dwRead == 0) {
			/* timeout */
			return 0;
		}

		drv->rx.cnt = dwRead;
		drv->rx.pos = 0;

	//	DBG_DUMP(DBG_TRACE, drv->rx.buf, dwRead);
	}

	cnt = drv->rx.cnt;
	if (cnt > max)
		cnt = max;

	for (i = 0; i < cnt; ++i)
		buf[i] = drv->rx.buf[drv->rx.pos++];

	if (drv->rx.pos == drv->rx.cnt)
		drv->rx.cnt = 0;

	return cnt;
}

int win_serial_drain(struct win_serial_drv * drv)
{
	int ret;

	assert(drv != NULL);

	if (!FlushFileBuffers(drv->hComm)) {
		if (GetLastError() != ERROR_IO_PENDING) { 
			DBG(DBG_WARNING, "FlushFileBuffers() failed with code: %d.", 
				(int)GetLastError());
		} else {
			DBG(DBG_TRACE, "I/O is pending ...");
			// WriteFile failed, but isn't delayed. Report error and abort.
		}
		ret = -1;
	} else {
		ret = 0;
	}

	return ret;
}


int win_serial_close(struct win_serial_drv * drv)
{
	assert(drv != NULL);

	if (!SetCommTimeouts(drv->hComm, &drv->save_timeouts)) {
	   // Error setting time-outs.
		fprintf(stderr, "%s: SetCommTimeouts() failed.\n", __func__);
		fflush(stderr);
	}

	if (!SetCommState(drv->hComm, &drv->save_dcb)) {
		// Error in SetCommState. Possibly a problem with the communications 
		// port handle or a problem with the DCB structure itself.
		fprintf(stderr, "%s: SetCommState() failed!\n", __func__);
		fflush(stderr);
	}

	CloseHandle(drv->hComm); 
	free(drv);

	return 0;
}

static int win_serial_conf_set(struct win_serial_drv * drv, 
							   const struct serial_config * cfg)
{
	DCB dcb;

	assert(drv != NULL);
	assert(cfg != NULL);

	dcb = drv->dcb;

	dcb.fAbortOnError = FALSE;
	dcb.fBinary = TRUE;
	dcb.fNull = FALSE;

	dcb.BaudRate = cfg->baudrate;
	dcb.ByteSize = cfg->databits;

	switch(cfg->parity) {
		case SERIAL_PARITY_NONE: 
			dcb.fParity = FALSE;
			dcb.Parity = NOPARITY;
			break;
		case SERIAL_PARITY_ODD: 
			dcb.fParity = TRUE;
			dcb.Parity = ODDPARITY;
			break;
		case SERIAL_PARITY_EVEN: 
			dcb.fParity = TRUE;
			dcb.Parity = EVENPARITY;
			break;
	}
	
	switch (cfg->stopbits) {
		case 2:
			dcb.StopBits = TWOSTOPBITS;
			break;
		default:
			dcb.StopBits = ONESTOPBIT;
			break;
	}

//	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
//	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	switch (cfg->flowctrl) {
		case SERIAL_FLOWCTRL_XONXOFF:
			break;
		case SERIAL_FLOWCTRL_RTSCTS:
			break;
		default:
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
	}


	if (!SetCommState(drv->hComm, &dcb)) {
		// Error in SetCommState. Possibly a problem with the communications 
		// port handle or a problem with the DCB structure itself.
		fprintf(stderr, "%s: SetCommState() failed!\n", __func__);
		fflush(stderr);
		return -1;
	}

	drv->dcb = dcb;

	if (!ClearCommError(drv->hComm, NULL, NULL)) {
		fprintf(stderr, "%s: ClearCommError() failed!\n", __func__);
		fflush(stderr);
		return -1;
	}

	return 0;
}

int win_serial_ioctl(struct win_serial_drv * drv, int opt, 
					 uintptr_t arg1, uintptr_t arg2)
{
	switch (opt) {
	case SERIAL_IOCTL_ENABLE:
		break;

	case SERIAL_IOCTL_DISABLE:
		break;

	case SERIAL_IOCTL_DRAIN:
		win_serial_drain(drv);
		break;

	case SERIAL_IOCTL_RESET:
		break;

	case SERIAL_IOCTL_STAT_GET: 
		break;

	case SERIAL_IOCTL_CONF_SET: 
		win_serial_conf_set(drv, (struct serial_config *)arg1);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

const struct serial_op win_serial_op = {
	.send = (void *)win_serial_send,
	.recv = (void *)win_serial_recv,
	.drain = (void *)win_serial_drain,
	.close = (void *)win_serial_close,
	.ioctl = (void *)win_serial_ioctl
};

struct serial_dev * win_serial_open(const char * com_port)
{
	char line[128];
	struct win_serial_drv * drv;
	struct serial_dev * dev;
	HANDLE hComm;


	drv = (struct win_serial_drv *)malloc(sizeof(struct win_serial_drv));
	if (drv == NULL) {
		fprintf(stderr, "%s: malloc() failed: %s.\n", 
				__func__, strerror(errno));
		fflush(stderr);
		return NULL;
	}

	dev = (struct serial_dev *)malloc(sizeof(struct serial_dev));
	if (dev == NULL) {
		free(drv);
		fprintf(stderr, "%s: malloc() failed: %s.\n", 
				__func__, strerror(errno));
		fflush(stderr);
		return NULL;
	}

	dev->drv = (void *)drv;
	dev->op = &win_serial_op;


	sprintf(line, "\\\\.\\%s", com_port);

	hComm = CreateFile(line,  
					   GENERIC_READ | GENERIC_WRITE, 
					   0, 
					   0, 
					   OPEN_EXISTING,
					   FILE_FLAG_OVERLAPPED,
					   0);

	if (hComm == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "%s: CreateFile(\"%s\") failed.\n", __func__, line);
		fflush(stderr);
		free(drv);
		free(dev);
		return NULL;
	}

	FillMemory(&drv->save_dcb, sizeof(DCB), 0);
	drv->save_dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(hComm, &drv->save_dcb)) {
	// Error getting current DCB settings
		fprintf(stderr, "%s: GetCommState() failed.\n", __func__);
		fflush(stderr);
		free(drv);
		free(dev);
		CloseHandle(hComm); 
		return NULL;
	}


	FillMemory(&drv->save_timeouts, sizeof(COMMTIMEOUTS), 0);
	if (!GetCommTimeouts(hComm, &drv->save_timeouts)) {
	   // Error getting time-outs.
		fprintf(stderr, "%s: GetCommTimeouts() failed.\n", __func__);
		fflush(stderr);
		free(drv);
		free(dev);
		CloseHandle(hComm); 
		return NULL;
	}

	drv->timeouts = drv->save_timeouts;

	/* Don't use total timeouts */
	drv->timeouts.ReadIntervalTimeout = MAXDWORD; 
	drv->timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
	drv->timeouts.ReadTotalTimeoutConstant = 0;
	drv->timeouts.WriteTotalTimeoutMultiplier = 0;
	drv->timeouts.WriteTotalTimeoutConstant = 0;


	if (!SetCommTimeouts(hComm, &drv->timeouts)) {
		// Error setting time-outs.
		fprintf(stderr, "%s: SetCommTimeouts() failed.\n", __func__);
		fflush(stderr);
		free(drv);
		free(dev);
		CloseHandle(hComm); 
		return NULL;
	}

	// Make sure the queue-sizes are reasonable sized. Win9X systems crash
	// if the input queue-size is zero. Both queues need to be at least
	// 16 bytes large.
	if (!SetupComm(hComm, 16, 16)) {
		// Error setting time-outs.
		fprintf(stderr, "%s: SetupComm() failed.\n", __func__);
		fflush(stderr);
		free(drv);
		free(dev);
		CloseHandle(hComm); 
		return NULL;
	}


	if (!PurgeComm(hComm, PURGE_TXCLEAR | PURGE_RXCLEAR)) {
		// Error setting time-outs.
		fprintf(stderr, "%s: PurgeComm() failed.\n", __func__);
		fflush(stderr);
		free(drv);
		free(dev);
		CloseHandle(hComm); 
		return NULL;
	}

	// DCB is ready for use.
	drv->dcb = drv->save_dcb;
	drv->hComm = hComm;
	/* clear buffer */
	drv->rx.pos = 0;
	drv->rx.cnt = 0;

//	SetCommMask(drv->hComm, EV_TXEMPTY);

	return dev;
}

#endif /* _WIN32 */

