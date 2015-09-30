#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dcclog.h>
#include <thinkos.h>

#include "simlnk.h"
#include "simrpc.h"
#include "simrpc_svc.h"

int simrpc_send(uint32_t hdr, void * data, unsigned int cnt)
{
	return thinkos_comm_send(hdr, data, cnt);
}

int simrpc_send_int(uint32_t hdr, int val)
{
	volatile uint32_t data[1];

	DCC_LOG1(LOG_INFO, "val=%d", val);

	data[0] = (uint32_t)val;

	return thinkos_comm_send(hdr, (void *)data, 4);
}

int simrpc_send_opc(uint32_t hdr)
{
	return thinkos_comm_send(hdr, NULL, 0);
}

void __attribute__((noreturn)) simlnk_recv_task(void)
{
	uint32_t data[(SIMLNK_MTU + 3) / 4];
	unsigned int cnt;
	uint32_t hdr = 0;

	DCC_LOG(LOG_TRACE, "SIMLNK task ready.");

	for (;;) {
		cnt = thinkos_comm_recv(&hdr, data, SIMLNK_MTU);

		DCC_LOG1(LOG_TRACE, "comm receive: %d bytes", cnt);

		switch SIMRPC_INSN(hdr) {
		case SIMRPC_FILE_OPEN:
			simrpc_file_open_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_CREATE:
			simrpc_file_create_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_CLOSE:
			simrpc_file_close_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_READ:
			simrpc_file_read_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_WRITE:
			simrpc_file_write_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_SEEK:
			simrpc_file_seek_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_UNLINK:
			simrpc_file_unlink_svc(hdr, data, cnt);
			break;
		case SIMRPC_FILE_CRC16:
			simrpc_file_crc16_svc(hdr, data, cnt);
			break;
		case SIMRPC_APPINFO:
			simrpc_appinfo_svc(hdr, data, cnt);
			break;
		case SIMRPC_DBCOMPILE:
			simrpc_dbcompile_svc(hdr, data, cnt);
			break;
		case SIMRPC_DBINFO:
			simrpc_dbinfo_svc(hdr, data, cnt);
			break;
		case SIMRPC_CFGCOMPILE:
			simrpc_cfgcompile_svc(hdr, data, cnt);
			break;
		case SIMRPC_CFGINFO:
			simrpc_cfginfo_svc(hdr, data, cnt);
			break;
		case SIMRPC_SHELLEXEC:
			simrpc_shellexec_svc(hdr, data, cnt);
			break;
		case SIMRPC_JSEXEC:
			simrpc_jsexec_svc(hdr, data, cnt);
			break;
		case SIMRPC_ATTR_GET:
			simrpc_attr_get_svc(hdr, data, cnt);
			break;
		case SIMRPC_STDOUT_FLUSH:
			simrpc_stdout_flush_svc(hdr, data, cnt);
			break;
		case SIMRPC_STATS:
			simrpc_stats_get_svc(hdr, data, cnt);
			break;

		default:
			DCC_LOG1(LOG_WARNING, "Invalid insn: %d", SIMRPC_INSN(hdr));
			simrpc_send_int(SIMRPC_REPLY_ERR(hdr), SIMRPC_ENOSYS);
		}
	}
}

