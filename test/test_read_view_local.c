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
	int64_t ret_size;
	upc_flag_t sync = 0;
	void *buffer;
	uint32_t i;
	char fname[] = "/mnt/plfs/upcio.test";
	upc_file_view *lfile_view;
	int read_count;

	lfile_view = NULL;
	printf("upcio test: test fread_view_local with %d Threads\n", THREADS);	
	fd=upc_all_fopen_view( fname, UPC_INDIVIDUAL_FP|UPC_RDONLY, 0666, &lfile_view, 1);
	if (lfile_view == NULL) {
		printf("TH%d: Error populating file view\n", MYTHREAD);
		upc_global_exit(-1);
	}

	if(fd == NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	buffer = malloc(upc_type_size(lfile_view->types[MYTHREAD]) * 
			lfile_view->block_lengths[MYTHREAD]);
	read_count = 1;
	ret_size = upc_all_fread_view_local(fd, (void *)buffer, read_count, sync, lfile_view);
	if( ret_size < 0 )
		printf("upcio test: fread_view_local error on TH%2d\n",MYTHREAD);
	else {
		if (lfile_view->types[MYTHREAD] == UPC_INT) {
			printf("TH%2d: Read: %d\n", MYTHREAD, ((int *)buffer)[0]);
		} else if (lfile_view->types[MYTHREAD] == UPC_FLOAT) {
			printf("TH%2d: Read float: %f\n", MYTHREAD, ((float *)buffer)[0]);
		}
	}

	upc_all_fsync(fd);
	if(upc_all_fclose_view(fd, lfile_view)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	free((void *)buffer);

	return 0;
}
	
