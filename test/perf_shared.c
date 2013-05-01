/*********************************************
  UPC-IO Reference Implementation
  Synthetic perf cases
  This cases is used to test the performance
  of read/write from/to local/shared  memory
  buffers using both private and common file
  pointers
 
  For the cases using shared memory buffer,
  Both infinite and finite blocksize senarios
  are tested.

  HPCL, The George Wasnington University
  Author: Yiyi Yao
  E-mail: yyy@gwu.edu
*********************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "upc.h"
#include "upc_io.h" 

/* A simple performance test. The file name is taken as a 
   command-line argument. */

//#define SIZE (1048576*4)       /* read/write size per node in bytes */
#define SIZE (524288)       /* read/write size per node in bytes */
#define BLOCK 10

double UPC_Wtime()
{
	struct timeval sampletime;
	double time;
	upc_barrier;
	gettimeofday(&sampletime, NULL);
	time=sampletime.tv_sec + (sampletime.tv_usec / 1000000.0);
	return(time);
}

 upcio_file_t *uopen(char *fname, int ronly) {
	 upcio_file_t *fd = NULL;
	int flags = 0;

	if (ronly)
		flags = UPC_INDIVIDUAL_FP|UPC_RDONLY;
	else
		flags = UPC_INDIVIDUAL_FP|UPC_WRONLY|UPC_CREATE;

	fd = upc_all_fopen( fname, flags, 0666);
	upc_barrier;
	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	return fd;
}

int main(int argc, char **argv)
{
	int i, j, ntimes, err, flag, strl;
	double stim, read_tim, write_tim;
	double min_read_tim, min_write_tim, read_bw, write_bw;
	 upcio_file_t *fh;
	upc_flag_t sync = 0;
	char *filename;
	
	shared int *buf;
	shared char *gfilename;
	shared int *len;

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
			fprintf(stderr, "\n*#  Usage: perf -fname filename\n\n");
			upc_global_exit(-1);
		}
		argv++;
		strl = strlen(*argv);
		upc_memput(len, &strl, sizeof(int));
	}

	upc_barrier;
	upc_memget(&strl, len, sizeof(int));
	upc_barrier;
	gfilename = (shared char *) upc_all_alloc(1,sizeof(char)*(strl));
	if (!MYTHREAD)
	{
		upc_memput(gfilename, *argv, strl);
		fprintf(stderr, "Access size per process = %d bytes, ntimes = %d\n", SIZE, ntimes);
	}

	upc_barrier;
	filename = (char *) malloc(sizeof(char)*(strl+1));
	upc_memget(filename, gfilename, strl);
	filename[strl] = '\0';

	/* allocate the shared buf on each thread
	   this is for shared w/r with INDIVIDUAL FP */
	buf = (shared int *) upc_global_alloc(1,SIZE);

	upc_barrier;
	min_read_tim=0.0;
	min_write_tim=0.0;

	upc_barrier;

	fh = uopen( filename, 0); 
	for (j=0; j<ntimes; j++) {
		upc_barrier;
		stim = UPC_Wtime();
		upc_all_fseek(fh, MYTHREAD*SIZE + SIZE*THREADS*j, UPC_SEEK_SET);
		err = upc_all_fwrite_shared(fh, buf, BLOCK, SIZE, sizeof(unsigned char), sync);
		if( err == -1 )
		{
			fprintf(stderr, "TH%2d: Error in write\n", MYTHREAD);
			break;
		}

		write_tim = UPC_Wtime() - stim;       
		min_write_tim += write_tim;	
	}

	upc_all_fclose(fh);
	upc_all_fsync(fh);
	min_write_tim /= ntimes;

	upc_barrier;
	fh = uopen( filename, 1); 
	for (j=0; j<ntimes; j++) {
		upc_barrier;
		stim = UPC_Wtime();
		upc_all_fseek(fh, MYTHREAD*SIZE + SIZE*THREADS*j, UPC_SEEK_SET);
		err = upc_all_fread_shared(fh, buf, BLOCK, SIZE, sizeof(unsigned char), sync);
		if( err == -1 )
		{
			fprintf(stderr, "TH%2d: Error in read\n", MYTHREAD);
			break;
		}

		read_tim = UPC_Wtime() - stim;
		min_read_tim += read_tim;
	}

	upc_all_fclose(fh);
	min_read_tim /= ntimes;
	
	upc_barrier;
    
	if (!MYTHREAD) {
		read_bw = (SIZE*THREADS*ntimes)/(min_read_tim*1024.0*1024.0);
		write_bw = (SIZE*THREADS*ntimes)/(min_write_tim*1024.0*1024.0);
		printf("TH: %d - Write bandwidth with a prior file sync = %f Mbytes/sec\n", MYTHREAD, write_bw);
		printf("TH: %d - Read bandwidth with a prior file sync = %f Mbytes/sec\n", MYTHREAD, read_bw);
	}

	upc_barrier;
	/* only thread 0 clean up the single shared buf */
	if(!MYTHREAD) {
		upc_free(buf);
		upc_free(gfilename);
		upc_free(len);
	}

	free(filename);
	return 0;
}
