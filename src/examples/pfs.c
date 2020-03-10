#include "syscall.h"
#include "pfs.h"

#include <random.h>
#include <stdio.h>
#include <string.h>


int main(void)
{
  /* int i;
  int pid[5];
  
  create("file.1", 50000);
  create("messages", 5000);
  
  pid[0] = exec("pfs_writer a z");
  pid[1] = exec("pfs_writer A Z");
  pid[2] = exec("pfs_reader");
  pid[3] = exec("pfs_reader");
  pid[4] = exec("pfs_reader");
  
  for (i = 0; i < 5; i++)
  {
    wait(pid[i]);
  }
  exit(0);
    */
    int i;
	int pid[N_PROC];
	char seed[R_BYTES];
	int ret;
	int len;
	char pfswriter[50] = "pfs_writer a z ";
	int fail = 0;

	ret = open("random");
	if (!ret) {
		printf("Failed to open random.\n");
		exit(-1);
	}

	read(ret, &seed[0], R_BYTES);
	close(ret);

	random_init((int)seed[0]);
	len = strlen(pfswriter);

	ret = create("file.1", BIG * TIMES);

	if (!ret)  {
		printf("Failed to create file.1\n");
		exit(-1);
	}

	for (i = 0; i < N_PROC; i++) {
		if(random_ulong() % 2 == 0)	 {
			pfswriter[len] = seed[i];
			pfswriter[len + 1] = 0;
			pid[i] = exec(pfswriter);
			if (pid[i] < 0) {
				printf("ERROR: Child creation returned %d\n", pid[i]);
				fail = 1;
			}
		}
		else  {
			pid[i] = exec("pfs_reader");
			if (pid[i] < 0) {
				printf("ERROR: Child creation returned %d\n", pid[i]);
				fail = 1;
			}
		}
	}

	for (i = 0; i < N_PROC; i++)
	{
		ret = wait(pid[i]);
		if (ret < 0) {
			printf("ERROR: Child wait returned %d\n", ret);
			fail = 1;
		}
	}


	printf("Testing concurrent writers\n");

	for (i =0; i < N_PROC; i++) {
		pfswriter[len] = seed[i];
		pfswriter[len + 1] = 0;
		pid[i] = exec(pfswriter);
		if (pid[i] < 0) {
			printf("ERROR: Child creation returned %d\n", pid[i]);
			fail = 1;
		}
	}

	for (i = 0; i < N_PROC; i++)
	{
		ret = wait(pid[i]);
		if (ret < 0) {
			printf("ERROR: Child wait returned %d\n", ret);
			fail = 1;
		}
	}

	printf("Testing concurrent readers\n");

	for (i =0; i < N_PROC; i++) {
		pid[i] = exec("pfs_reader");
		if (pid[i] < 0) {
			printf("ERROR: Child creation returned %d\n", pid[i]);
			fail = 1;
		}
	}

	for (i = 0; i < N_PROC; i++)
	{
		ret = wait(pid[i]);
		if (ret < 0) {
			printf("ERROR: Child wait returned %d\n", ret);
			fail = 1;
		}
	}

	if (fail)
		printf("Test failed!\n");

	exit(0);
}
