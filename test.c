#include <stdio.h>
#include "include/prozubilib_conf.h"
#include "prozubilib.h"

void on_log(void *data, const char *msg){
	if (msg)
		printf("%s\n", msg);
}

void on_err(void *data, const char *msg){
	if (msg)
		printf("\x1B[31m%s\x1B[0m\n", msg);
}

int progress(
			void *progressp, pphase phase, int current, int total)
{
	if (phase == PPHASE_COUNTING)
		printf("\x1B[32mCOUNTING: %d of %d (%0.2lf%%)\x1B[0m\n", 
				current, total,
				(double)current/(double)total*100);
	if (phase == PPHASE_UPLOADING)
		printf("\x1B[32mUPLOADING: %d of %d (%0.2lf%%)\x1B[0m\n", 
				current, total,
				(double)current/(double)total*100);
	if (phase == PPHASE_DOWNLOADING)
		printf("\x1B[32mDOWNLOADING: %d of %d (%0.2lf%%)\x1B[0m\n", 
				current, total,
				(double)current/(double)total*100);
	if (phase == PPHASE_DELETING)
		printf("\x1B[32mDELETING: %d of %d (%0.2lf%%)\x1B[0m\n", 
				current, total,
				(double)current/(double)total*100);
	return 0;
}

int main(int argc, char *argv[])
{ 
	prozubi_t *p = prozubi_init(
			"database.db", 
			NULL, 
			NULL, on_err, 
			NULL, on_log);

	printf("init yandexdisk...\t");
	char token[128] = {0};
	FILE *fp = fopen("token", "r");
	if (fp){
		fread(token, 1, 58, fp);
		fclose(fp);
	}
	printf("TOKEN: '%s'\t", token);
	kdydm_t *module = yandex_disk_module_load(
			&p->kdata2, token,
		 	NULL, progress);
	printf("OK\n");
	
	printf("press any key to exit\n");
	getchar();

	return 0;
}
