/*********************************************
  UPC-IO Reference Implementation
  Synthetic perf cases
  This cases is used to test the performance
  of read/write from/to local memory buffers 
  using a private file pointer. 2 different
  senarios are tested: with fsync and w/o 
  fsync
 
  The originally scheme is presented in the
  MPI-IO test cases

  HPCL, The George Wasnington University
  Author: Yiyi Yao
  E-mail: yyy@gwu.edu
*********************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "upc.h"
#include "upc_io.h" 

/* A simple performance test. The file name is taken as a 
   command-line argument. */

//#define SIZE (1048576*4)       /* read/write size per node in bytes */
#define SIZE (65536)       /* read/write size per node in bytes */

 upcio_file_t *uopen(char *fname, int ronly) {
	 upcio_file_t *fd = NULL;
	int flags = 0;

	upc_barrier;
	if (ronly)
		flags = UPC_INDIVIDUAL_FP|UPC_RDONLY;
	else
		flags = UPC_INDIVIDUAL_FP|UPC_WRONLY|UPC_CREATE;

	fd = upc_all_fopen( fname, flags, 0666);
	upc_barrier;
	if(fd == NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	return fd;
}

int main(int argc, char **argv)
{
	int i, j, ntimes, err, flag;
	unsigned char *buf;
	long double stim, read_time, write_time;
	long double total_read_time, total_write_time;
	long double read_bw, write_bw;
	 upcio_file_t *fh;
	upc_flag_t sync = 0;
	char *filename;
	int strl;
	shared char *gfilename;
	shared int *len;
	
	upc_off_t seek_pos;
	struct timeval tv;

	ntimes=1;
/* process 0 takes the file name as a command-line argument and 
   broadcasts it to other processes */
	len = (shared int *) upc_all_alloc(1, sizeof(int));
	upc_barrier;
	if (!MYTHREAD) {
		i = 1;
		while ((i < argc) && strcmp("-fname", *argv)) {
			i++;
			argv++;
		}
		if (i >= argc) {
			printf("\n*#  Usage: perf -fname filename\n\n");
			upc_global_exit(-1);
		}
		argv++;
		strl = strlen(*argv);
		upc_memput(len, &strl , sizeof(int));
	}

	upc_barrier;
	upc_memget(&strl, len, sizeof(int));
	upc_barrier;
	gfilename = (shared char *) upc_all_alloc(1,sizeof(char)*(strl));
	upc_barrier;
	if (!MYTHREAD)
	{
		upc_memput(gfilename, *argv, strl);
		printf("Access size per process = %d bytes, ntimes = %d\n", SIZE, ntimes);
	}

	upc_barrier;
	filename = (char *) malloc(sizeof(char)*(strl+1));
	upc_barrier;
	upc_memget(filename, gfilename, strl);
	filename[strl] = '\0';

	buf = (unsigned char *) malloc(SIZE);
	memset(buf, MYTHREAD + '0', SIZE);
	upc_barrier;
	total_read_time = 0.0;
	total_write_time = 0.0;

	upc_barrier;
	fh = uopen( filename, 0); 
	for (j=0; j<ntimes; j++) {
		upc_barrier;
		gettimeofday(&tv, NULL);
		stim = tv.tv_sec + (long double) (tv.tv_usec / 1000000.0f);
		seek_pos =  MYTHREAD*SIZE + SIZE*THREADS*j;
		upc_all_fseek(fh, seek_pos, UPC_SEEK_SET);
		err = upc_all_fwrite_local(fh, buf, SIZE, sizeof(unsigned char), sync);
		if( err == -1 )
		{
			printf("TH%2d: Error in write\n", MYTHREAD);
			break;
		}

		upc_barrier;
		gettimeofday(&tv, NULL);
		write_time = (long double) (tv.tv_sec + (long double) tv.tv_usec / 1000000.0f) - stim;
		total_write_time += write_time;  	
	}
	
	upc_all_fsync(fh);
	upc_all_fclose(fh);	
	
	upc_barrier;
	fh = uopen( filename, 1); 
	for (j=0; j<ntimes; j++) {
		upc_barrier;
		gettimeofday(&tv, NULL);
		stim = tv.tv_sec + (long double) (tv.tv_usec / 1000000.0f);
		seek_pos = MYTHREAD*SIZE + SIZE*THREADS*j;
		upc_all_fseek(fh, seek_pos, UPC_SEEK_SET);
		err = upc_all_fread_local(fh, buf, SIZE, sizeof(unsigned char), sync);
		if( err == -1 )
		{
			printf("TH%2d: Error in read\n", MYTHREAD);
			break;
		}

		upc_barrier;
		gettimeofday(&tv, NULL);
		read_time = (long double) (tv.tv_sec  + (long double) tv.tv_usec / 1000000.0f) - stim;		
		total_read_time += read_time;
	}
  	
	upc_all_fclose(fh);

	upc_barrier;
	if (!MYTHREAD) {
		if (!total_read_time)
			total_read_time = 1;

		if (!total_write_time)
			total_write_time = 1;

		read_bw = (SIZE*THREADS*ntimes)/(total_read_time*1024.0f*1024.0f);
		write_bw = (SIZE*THREADS*ntimes)/(total_write_time*1024.0f*1024.0f);
		printf("TH: %d - Write bandwidth with a prior file sync = %lf Mbytes/sec\n", MYTHREAD, write_bw);
		printf("TH: %d - Read bandwidth with a prior file sync = %lf Mbytes/sec\n", MYTHREAD, read_bw);
	}

	upc_barrier;
	free(buf);
	free(filename);
	upc_barrier;
	
	if (!MYTHREAD) {
		upc_free(gfilename); 
		upc_free(len);
	}
	 
	return 0;
}
