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
	uint32_t size;
	upc_off_t fsize;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: size with %d Threads\n", THREADS);

	fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY|UPC_CREATE, 0666);
	upc_barrier;	

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	fsize = upc_all_fget_size(fd);
	if( fsize == -1 )
		printf("TH%2d: get_size Error\n",MYTHREAD);
	else
		printf("TH%2d: the size of file is %ld\n", MYTHREAD, fsize);

	upc_barrier;
	size = upc_all_fset_size(fd, 5);
	if( size == -1 )
		printf("TH%2d: set_size Error\n",MYTHREAD);

	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with size testing\n");

	return 0;
}
	
