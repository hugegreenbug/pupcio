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

int main()
{
	 upcio_file_t *fd;
	char *buffer;
	uint32_t size, i;
	struct upc_io_local_memvec memvec[2];
	struct upc_io_filevec filevec[2];
	upc_flag_t sync = 0;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fwrite_list_list_local with %d Threads\n", THREADS);

	size = 10;
	buffer = (char *)malloc(sizeof(char)*size);
	memvec[0].baseaddr = &buffer[0];
	memvec[0].len = 4;
	memvec[1].baseaddr = &buffer[6];
	memvec[1].len = 3;
	filevec[0].offset = 4*MYTHREAD;
	filevec[0].len = 3;
        filevec[1].offset = 8+4*MYTHREAD;
        filevec[1].len = 4;

	for(i=0; i<size; i++)
		buffer[i] = 'z';

	upc_barrier;
	if (!MYTHREAD) {
		fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY, 0666);
	}

	upc_barrier;
	if (MYTHREAD) {
		fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY, 0666);
	}

	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	size = upc_all_fwrite_list_local(fd, 2, (struct upc_io_local_memvec const *)&memvec, 
					 2, (struct upc_io_filevec const *)&filevec, sync);
	if( size == -1 )
                printf("TH%2d: write_list_local Error\n",MYTHREAD);

        upc_barrier;

	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with fwrite_list_local testing\n");

	free((void *)buffer);
	return 0;
}
	
