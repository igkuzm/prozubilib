#include "../include/yclients.h"
#include <unistd.h>

#define TIMEOUT 5

void yclients_main_loop(void *data)
{
	struct yclients *t = (struct yclients *)data;
	prozubi_t *p = t->p;
	while (1)
	{
		if (p->on_log)
			p->on_log(p->on_log_data, "yclients: run in loop...");

#ifdef _WIN32
		Sleep(TIMEOUT * 1000);
#else
		sleep(TIMEOUT);
#endif
	}

}
