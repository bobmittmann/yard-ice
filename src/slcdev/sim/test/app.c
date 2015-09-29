#include <stdlib.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

void __attribute__((noreturn)) app_main(void);


void __attribute__((noreturn)) app_default(void)
{
	DCC_LOG(LOG_TRACE, "calling app_main()");
	app_main();
}

