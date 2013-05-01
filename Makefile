# This Makefile produces the upc_io.o file

BUPC = /usr/local/bin
CC = ${BUPC}/upcc -translator=/usr/local/bupc_translator/targ

INCLUDE_DIR = -I./ADIO -I/usr/local/include
CFLAGS = ${INCLUDE_DIR} -c -network=smp

LINK_DIR =
LFLAGS = ${INCLUDE_DIR} ${LINK_DIR} -network=smp

DEFINITION =
NP=1
OPTIONS = -T=${NP} -DDEBUG -g

OBJS = upc_fopen.o upc_fopen_trans.o upc_fopen_view.o upc_fclose.o upc_fsync.o upc_fread_local.o upc_fwrite_local.o upc_fread_shared.o upc_fwrite_shared.o upc_fcntl.o upc_fseek.o upc_fset_size.o upc_fget_size.o upc_ftest_async.o upc_fwait_async.o upc_fread_local_async.o upc_fwrite_local_async.o upc_fread_shared_async.o upc_fwrite_shared_async.o upc_fread_list_local.o upc_fread_list_shared.o upc_fwrite_list_local.o upc_fwrite_list_shared.o upc_fread_list_local_async.o upc_fread_list_shared_async.o upc_fwrite_list_local_async.o upc_fwrite_list_shared_async.o upc_fwrite_view_local.o upc_fread_view_local.o upc_fclose_view.o upc_commit.o
ADIO_OBJS = adio_open.o adio_close.o adio_flush.o adio_getsize.o adio_resize.o adio_read_contig.o adio_read_strided.o adio_write_contig.o adio_write_strided.o adio_iread_contig.o adio_iwrite_contig.o adio_iread_strided.o adio_iwrite_strided.o adio_done.o adio_isdone.o adio_view_open.o adio_write_view_contig.o adio_read_view_contig.o adio_close_view.o adio_commit.o adio_open_trans.o

all: ${OBJS} ${ADIO_OBJS} upc_tests adio_tests

adio_open.o: ADIO/adio.h ADIO/adio_open.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_open.c

adio_view_open.o: ADIO/adio.h ADIO/adio_view_open.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_view_open.c

adio_close.o: ADIO/adio.h ADIO/adio_close.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_close.c

adio_close_view.o: ADIO/adio.h ADIO/adio_close_view.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_close_view.c

adio_commit.o: ADIO/adio.h ADIO/adio_commit.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_commit.c

adio_flush.o: ADIO/adio.h ADIO/adio_flush.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_flush.c

adio_getsize.o: ADIO/adio.h ADIO/adio_getsize.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_getsize.c

adio_resize.o: ADIO/adio.h ADIO/adio_resize.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_resize.c

adio_read_contig.o: ADIO/adio.h ADIO/adio_read_contig.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_read_contig.c

adio_open_trans.o: ADIO/adio.h ADIO/adio_open_trans.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_open_trans.c

adio_read_view_contig.o: ADIO/adio.h ADIO/adio_read_view_contig.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_read_view_contig.c

adio_read_strided.o: ADIO/adio.h ADIO/adio_read_strided.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_read_strided.c

adio_write_contig.o: ADIO/adio.h ADIO/adio_write_contig.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_write_contig.c

adio_write_view_contig.o: ADIO/adio.h ADIO/adio_write_view_contig.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_write_view_contig.c

adio_write_strided.o: ADIO/adio.h ADIO/adio_write_strided.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_write_strided.c

adio_iread_contig.o: ADIO/adio.h ADIO/adio_iread_contig.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_iread_contig.c

adio_iwrite_contig.o: ADIO/adio.h ADIO/adio_iwrite_contig.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_iwrite_contig.c

adio_iread_strided.o: ADIO/adio.h ADIO/adio_iread_strided.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_iread_strided.c

adio_iwrite_strided.o: ADIO/adio.h ADIO/adio_iwrite_strided.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_iwrite_strided.c

adio_done.o: ADIO/adio.h ADIO/adio_done.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_done.c

adio_isdone.o: ADIO/adio.h ADIO/adio_isdone.c
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) ADIO/adio_isdone.c

upc_fopen.o: upc_fopen.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fopen.c

upc_fopen_trans.o: upc_fopen_trans.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fopen_trans.c

upc_fopen_view.o: upc_fopen_view.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fopen_view.c

upc_fclose.o: upc_fclose.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fclose.c

upc_fclose_view.o: upc_fclose_view.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fclose_view.c

upc_fsync.o: upc_fsync.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fsync.c

upc_fget_size.o: upc_fget_size.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fget_size.c

upc_fset_size.o: upc_fset_size.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fset_size.c

upc_fseek.o: upc_fseek.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fseek.c

upc_commit.o: upc_commit.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_commit.c

upc_fcntl.o: upc_fcntl.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fcntl.c
	
upc_fread_local.o: upc_fread_local.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_local.c

upc_fread_view_local.o: upc_fread_view_local.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_view_local.c

upc_fwrite_local.o: upc_fwrite_local.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_local.c

upc_fwrite_view_local.o: upc_fwrite_view_local.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_view_local.c

upc_fread_local_async.o: upc_fread_local_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_local_async.c

upc_fwrite_local_async.o: upc_fwrite_local_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_local_async.c

upc_fread_shared.o: upc_fread_shared.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_shared.c

upc_fwrite_shared.o: upc_fwrite_shared.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_shared.c

upc_fread_shared_async.o: upc_fread_shared_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_shared_async.c

upc_fwrite_shared_async.o: upc_fwrite_shared_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_shared_async.c

upc_fread_list_local.o: upc_fread_list_local.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_list_local.c

upc_fwrite_list_local.o: upc_fwrite_list_local.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_list_local.c

upc_fread_list_local_async.o: upc_fread_list_local_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_list_local_async.c

upc_fwrite_list_local_async.o: upc_fwrite_list_local_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_list_local_async.c
	
upc_fread_list_shared.o: upc_fread_list_shared.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_list_shared.c

upc_fwrite_list_shared.o: upc_fwrite_list_shared.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_list_shared.c

upc_fread_list_shared_async.o: upc_fread_list_shared_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fread_list_shared_async.c

upc_fwrite_list_shared_async.o: upc_fwrite_list_shared_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwrite_list_shared_async.c
	
upc_ftest_async.o: upc_ftest_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_ftest_async.c
	
upc_fwait_async.o: upc_fwait_async.c upc_io.h ADIO/adio.h
	${CC} ${OPTIONS} ${DEFINITION} $(CFLAGS) upc_fwait_async.c

.PHONY: upc_tests adio_tests

upc_tests:
	make -C test

adio_tests:
	make -C ADIO/test

clean: 
	rm -f core *.o *~ *.a
	make -C test clean
	make -C ADIO/test clean
