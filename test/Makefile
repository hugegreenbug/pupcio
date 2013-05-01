# This Makefile produces the upc_io.o file

BUPC = /usr/local/bin
CC = ${BUPC}/upcc -translator=/usr/local/bupc_translator/targ
NP = 1
AR = ar crl
RANLIB = ranlib
INCLUDE_DIR = -I../ADIO -I../ -I/usr/local/include
CFLAGS = ${INCLUDE_DIR} -c -network=smp
LINK_DIR =
LFLAGS = ${INCLUDE_DIR} ${LINK_DIR} -network=smp -lm -lplfs
DEFINITION =
OPTIONS = -T=${NP} -DDEBUG -g
OBJS_PATH = ..

UPC_IO_OBJS = ${OBJS_PATH}/upc_fopen.o  ${OBJS_PATH}/upc_fopen_trans.o ${OBJS_PATH}/upc_fclose.o ${OBJS_PATH}/upc_fclose_view.o ${OBJS_PATH}/upc_commit.o ${OBJS_PATH}/upc_fsync.o ${OBJS_PATH}/upc_fread_local.o ${OBJS_PATH}/upc_fwrite_local.o ${OBJS_PATH}/upc_fread_shared.o ${OBJS_PATH}/upc_fwrite_shared.o ${OBJS_PATH}/upc_fcntl.o ${OBJS_PATH}/upc_fseek.o ${OBJS_PATH}/upc_fset_size.o ${OBJS_PATH}/upc_fget_size.o ${OBJS_PATH}/upc_ftest_async.o ${OBJS_PATH}/upc_fwait_async.o ${OBJS_PATH}/upc_fread_local_async.o ${OBJS_PATH}/upc_fwrite_local_async.o ${OBJS_PATH}/upc_fread_shared_async.o ${OBJS_PATH}/upc_fwrite_shared_async.o ${OBJS_PATH}/upc_fread_list_local.o ${OBJS_PATH}/upc_fread_list_local_async.o ${OBJS_PATH}/upc_fwrite_list_local.o ${OBJS_PATH}/upc_fwrite_list_local_async.o ${OBJS_PATH}/upc_fread_list_shared.o ${OBJS_PATH}/upc_fread_list_shared_async.o ${OBJS_PATH}/upc_fwrite_list_shared.o ${OBJS_PATH}/upc_fwrite_list_shared_async.o ${OBJS_PATH}/upc_fopen_view.o ${OBJS_PATH}/upc_fwrite_view_local.o ${OBJS_PATH}/upc_fread_view_local.o

ADIO_OBJS = ${OBJS_PATH}/adio_open.o ${OBJS_PATH}/adio_open_trans.o ${OBJS_PATH}/adio_close.o ${OBJS_PATH}/adio_close_view.o ${OBJS_PATH}/adio_commit.o ${OBJS_PATH}/adio_flush.o ${OBJS_PATH}/adio_getsize.o ${OBJS_PATH}/adio_resize.o ${OBJS_PATH}/adio_read_contig.o ${OBJS_PATH}/adio_read_strided.o ${OBJS_PATH}/adio_write_contig.o ${OBJS_PATH}/adio_write_strided.o ${OBJS_PATH}/adio_iread_contig.o ${OBJS_PATH}/adio_iwrite_contig.o ${OBJS_PATH}/adio_iread_strided.o ${OBJS_PATH}/adio_iwrite_strided.o ${OBJS_PATH}/adio_isdone.o ${OBJS_PATH}/adio_done.o ${OBJS_PATH}/adio_view_open.o ${OBJS_PATH}/adio_write_view_contig.o ${OBJS_PATH}/adio_read_view_contig.o

OBJS = ${UPC_IO_OBJS} ${ADIO_OBJS}

TARGETS = test_open_close test_read_local test_write_local test_read_shared_ind test_read_shared_com test_write_shared_ind test_write_shared_com test_seek test_size test_read_local_async test_write_local_async test_read_shared_ind_async test_read_shared_com_async test_write_shared_ind_async test_write_shared_com_async test_read_list_local test_write_list_local test_read_list_shared test_write_list_shared test_read_list_local_async test_write_list_local_async test_read_list_shared_async test_write_list_shared_async test_async_wait_async test_async_close_wait test_async_wait_close test_async_wait_sync perf_local test_read_shared_com_block test_read_shared_ind_block test_read_list_shared_block test_write_list_shared_block perf_num_open_files perf_shared test_write_view_local test_read_view_local test_read_trans_local test_write_trans_local

all: ${TARGETS}

.c.o:
	$(CC) ${OPTIONS} ${DEFINITION} $(CFLAGS) $<

test_open_close: ${OBJS} test_open_close.o
	$(CC) ${OPTIONS} test_open_close.o ${OBJS} ${LFLAGS} -o test_open_close

test_read_local: ${OBJS} test_read_local.o
	$(CC) ${OPTIONS} test_read_local.o ${OBJS} ${LFLAGS} -o test_read_local

test_read_trans_local: ${OBJS} test_read_trans_local.o
	$(CC) ${OPTIONS} test_read_trans_local.o ${OBJS} ${LFLAGS} -o test_read_trans_local

test_read_view_local: ${OBJS} test_read_view_local.o
	$(CC) ${OPTIONS} test_read_view_local.o ${OBJS} ${LFLAGS} -o test_read_view_local

test_write_local: ${OBJS} test_write_local.o
	$(CC) ${OPTIONS} test_write_local.o ${OBJS} ${LFLAGS} -o test_write_local

test_write_trans_local: ${OBJS} test_write_trans_local.o
	$(CC) ${OPTIONS} test_write_trans_local.o ${OBJS} ${LFLAGS} -o test_write_trans_local

test_write_view_local: ${OBJS} test_write_view_local.o
	$(CC) ${OPTIONS} test_write_view_local.o ${OBJS} ${LFLAGS} -o test_write_view_local

test_read_shared_ind: ${OBJS} test_read_shared_ind.o
	$(CC) ${OPTIONS} test_read_shared_ind.o ${OBJS} ${LFLAGS} -o test_read_shared_ind

test_read_shared_com: ${OBJS} test_read_shared_com.o
	$(CC) ${OPTIONS} test_read_shared_com.o ${OBJS} ${LFLAGS} -o test_read_shared_com

test_write_shared_ind: ${OBJS} test_write_shared_ind.o
	$(CC) ${OPTIONS} test_write_shared_ind.o ${OBJS} ${LFLAGS} -o test_write_shared_ind

test_write_shared_com: ${OBJS} test_write_shared_com.o
	$(CC) ${OPTIONS} test_write_shared_com.o ${OBJS} ${LFLAGS} -o test_write_shared_com

test_seek: ${OBJS} test_seek.o
	$(CC) ${OPTIONS} test_seek.o ${OBJS} ${LFLAGS} -o test_seek

test_size: ${OBJS} test_size.o
	$(CC) ${OPTIONS} test_size.o ${OBJS} ${LFLAGS} -o test_size

test_read_local_async: ${OBJS} test_read_local_async.o
	$(CC) ${OPTIONS} test_read_local_async.o ${OBJS} ${LFLAGS} -o test_read_local_async

test_write_local_async: ${OBJS} test_write_local_async.o
	$(CC) ${OPTIONS} test_write_local_async.o ${OBJS} ${LFLAGS} -o test_write_local_async

test_read_shared_ind_async: ${OBJS} test_read_shared_ind_async.o
	$(CC) ${OPTIONS} test_read_shared_ind_async.o ${OBJS} ${LFLAGS} -o test_read_shared_ind_async

test_read_shared_com_async: ${OBJS} test_read_shared_com_async.o
	$(CC) ${OPTIONS} test_read_shared_com_async.o ${OBJS} ${LFLAGS} -o test_read_shared_com_async

test_write_shared_ind_async: ${OBJS} test_write_shared_ind_async.o
	$(CC) ${OPTIONS} test_write_shared_ind_async.o ${OBJS} ${LFLAGS} -o test_write_shared_ind_async

test_write_shared_com_async: ${OBJS} test_write_shared_com_async.o
	$(CC) ${OPTIONS} test_write_shared_com_async.o ${OBJS} ${LFLAGS} -o test_write_shared_com_async

test_read_list_local: ${OBJS} test_read_list_local.o
	$(CC) ${OPTIONS} test_read_list_local.o ${OBJS} ${LFLAGS} -o test_read_list_local

test_write_list_local: ${OBJS} test_write_list_local.o
	$(CC) ${OPTIONS} test_write_list_local.o ${OBJS} ${LFLAGS} -o test_write_list_local

test_read_list_shared: ${OBJS} test_read_list_shared.o
	$(CC) ${OPTIONS} test_read_list_shared.o ${OBJS} ${LFLAGS} -o test_read_list_shared

test_write_list_shared: ${OBJS} test_write_list_shared.o
	$(CC) ${OPTIONS} test_write_list_shared.o ${OBJS} ${LFLAGS} -o test_write_list_shared

test_read_list_local_async: ${OBJS} test_read_list_local_async.o
	$(CC) ${OPTIONS} test_read_list_local_async.o ${OBJS} ${LFLAGS} -o test_read_list_local_async

test_write_list_local_async: ${OBJS} test_write_list_local_async.o
	$(CC) ${OPTIONS} test_write_list_local_async.o ${OBJS} ${LFLAGS} -o test_write_list_local_async

test_read_list_shared_async: ${OBJS} test_read_list_shared_async.o
	$(CC) ${OPTIONS} test_read_list_shared_async.o ${OBJS} ${LFLAGS} -o test_read_list_shared_async

test_write_list_shared_async: ${OBJS} test_write_list_shared_async.o
	$(CC) ${OPTIONS} test_write_list_shared_async.o ${OBJS} ${LFLAGS} -o test_write_list_shared_async

test_async_wait_async: ${OBJS} test_async_wait_async.o
	$(CC) ${OPTIONS} test_async_wait_async.o ${OBJS} ${LFLAGS} -o test_async_wait_async

test_async_close_wait: ${OBJS} test_async_close_wait.o
	$(CC) ${OPTIONS} test_async_close_wait.o ${OBJS} ${LFLAGS} -o test_async_close_wait

test_async_wait_close: ${OBJS} test_async_wait_close.o
	$(CC) ${OPTIONS} test_async_wait_close.o ${OBJS} ${LFLAGS} -o test_async_wait_close

test_async_wait_sync: ${OBJS} test_async_wait_sync.o
	$(CC) ${OPTIONS} test_async_wait_sync.o ${OBJS} ${LFLAGS} -o test_async_wait_sync

perf_local: ${OBJS} perf_local.o
	$(CC) ${OPTIONS} perf_local.o ${OBJS} ${LFLAGS} -o perf_local

test_read_shared_com_block: ${OBJS} test_read_shared_com_block.o
	$(CC) ${OPTIONS} test_read_shared_com_block.o ${OBJS} ${LFLAGS} -o test_read_shared_com_block

test_read_shared_ind_block: ${OBJS} test_read_shared_ind_block.o
	$(CC) ${OPTIONS} test_read_shared_ind_block.o ${OBJS} ${LFLAGS} -o test_read_shared_ind_block

test_read_list_shared_block: ${OBJS} test_read_list_shared_block.o
	$(CC) ${OPTIONS} test_read_list_shared_block.o ${OBJS} ${LFLAGS} -o test_read_list_shared_block

test_write_list_shared_block: ${OBJS} test_write_list_shared_block.o
	$(CC) ${OPTIONS} test_write_list_shared_block.o ${OBJS} ${LFLAGS} -o test_write_list_shared_block

perf_num_open_files: ${OBJS} perf_num_open_files.o
	$(CC) ${OPTIONS} perf_num_open_files.o ${OBJS} ${LFLAGS} -o perf_num_open_files

perf_shared: ${OBJS} perf_shared.o
	$(CC) ${OPTIONS} perf_shared.o ${OBJS} ${LFLAGS} -o perf_shared

.PHONY:
clean: 
	rm -f core *.o ${TARGETS} *~
