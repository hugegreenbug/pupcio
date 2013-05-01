/*********************************************
  UPC-IO Reference Implementation         
  Internal test cases

  HPCL, The George Wasnington University
  Author: Yiyi Yao
  E-mail: yyy@gwu.edu
*********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <upc.h>
#include "upc_io.h"

int main(int argc, char **argv)
{
	upcio_file_t *fd;
	upc_off_t ret_size, size;
	uint32_t nmemb;
	upc_flag_t sync = 0;
	char *buffer;
	uint32_t i;
	int tid = 0;
	char fname[] = "/mnt/plfs/upcio.test";

	if(!MYTHREAD)
		printf("upcio test: test fread_local with %d Threads\n", THREADS);

	if (argc < 2) {
		printf("usage: ./test_read_trans_local transaction_id\n");
		exit(1);
	}

	tid = atoi(argv[1]);
	nmemb = 1;
	size = 10*(MYTHREAD+1);
	buffer = (char *)malloc(sizeof(char)*(size+1)*nmemb);
	buffer[size] = '\0';

	fd=upc_all_fopen_trans( fname, UPC_INDIVIDUAL_FP|UPC_RDONLY|UPC_CREATE, 0666, tid);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	ret_size = upc_all_fread_local(fd, (void *)buffer, size, nmemb, sync);
	if( ret_size == -1 )
		printf("upcio test: fread_local error on TH%2d\n",MYTHREAD);
	else
	{
		for(i=0; i<THREADS; i++)
		{
			if(MYTHREAD==i)
				printf("upcio test: read \"%s\" on TH%2d\n",buffer,MYTHREAD);

			upc_barrier;
		}
	}
	
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with fread_local testing\n");

	free((void *)buffer);

	return 0;
}
	
