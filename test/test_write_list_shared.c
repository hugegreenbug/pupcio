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
	shared [] char *buffer;
	uint32_t size, i;
	struct upc_io_shared_memvec memvec[2];
	struct upc_io_filevec filevec[2];
	upc_flag_t sync = 0;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fwrite_list_shared with %d Threads\n", THREADS);

	size = 10;
	buffer = (shared [] char *)upc_local_alloc(1,sizeof(char)*size);
	memvec[0].baseaddr = (shared void *)&buffer[0];
	memvec[0].blocksize = 0;
	memvec[0].len = 4;
	memvec[1].baseaddr = (shared void *)&buffer[6];
	memvec[1].blocksize = 0;
	memvec[1].len = 3;
	filevec[0].offset = 4*MYTHREAD;
	filevec[0].len = 3;
        filevec[1].offset = 8+4*MYTHREAD;
        filevec[1].len = 4;

	for(i=0; i<size; i++)
		buffer[i] = 'z';

	fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	size = upc_all_fwrite_list_shared(fd, 2, (struct upc_io_shared_memvec const *)&memvec[0], 
					  2, (struct upc_io_filevec const *)&filevec[0], sync);
	if( size == -1 )
                printf("TH%2d: write_list_shared error \n",MYTHREAD);

        upc_barrier;
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with fwrite_list_shared testing\n");

	upc_free(buffer);
	return 0;
}
	
