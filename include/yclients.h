#ifndef PROZUBI_YCLIENTS_H
#define PROZUBI_YCLIENTS_H
#include "../prozubilib.h"

struct yclients {
	prozubi_t *p;
	const CYCUser *user;
};

void yclients_main_loop(void *data);


#endif /* ifndef PROZUBI_YCLIENTS_H */
