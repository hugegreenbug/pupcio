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
	upc_off_t ret_size;
	upc_flag_t sync = 0;
	char *buffer;
	uint32_t i, size, nmemb;
	char fname[] = "/mnt/plfs/upcio.test";
	int tid = 0;
	char c = '0';

	nmemb = 1;
	size = 5*(MYTHREAD+1);
	buffer = (char *)malloc(sizeof(char)*size*nmemb);

	if(!MYTHREAD)
		printf("upcio test: test fwrite_local with %d Threads\n", THREADS);

    	if (argc < 3) {
                printf("usage: ./test_write_trans_local transaction_id char_to_write\n");
                exit(1);
        }

        tid = atoi(argv[1]);
        c = *argv[2];
	fd=upc_all_fopen_trans( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY|UPC_CREATE, 0666, tid );
	upc_barrier;
	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	for(i=0; i<size; i++)
	{
		buffer[i] = c;
	}

	upc_barrier;
	upc_all_fseek(fd, MYTHREAD*10, UPC_SEEK_SET);
	ret_size = upc_all_fwrite_local(fd, (void *)buffer, size, nmemb, sync);
	upc_barrier;

	if( ret_size == -1 )
		printf("upcio test: fwrite_local error on TH%2d\n",MYTHREAD);
	
	upc_barrier;

	upc_all_fsync(fd);
	upc_all_commit(fd);
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with fwrite_local testing\n");

	free((void *)buffer);
	return 0;
}
	
