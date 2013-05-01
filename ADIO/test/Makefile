BUPC = /usr/local/bin
CC = ${BUPC}/upcc -translator=/usr/local/bupc_translator/targ
#CC = upcc -translator=/usr/local/bupc_translator/targ
LINK = upcc
NP=1
CFLAGS = -DDEBUG -g -T=${NP} -I.. -I/usr/local/include
LDFLAGS = -DDEBUG -g -T=${NP} -L/usr/local/lib -lplfs

OBJS = utils.o adio_open.o adio_close.o adio_flush.o adio_getsize.o adio_read_contig.o adio_write_contig.o adio_iread_contig.o adio_isdone.o adio_iwrite_contig.o adio_done.o adio_iread_strided.o adio_iwrite_strided.o adio_read_strided.o adio_write_strided.o

TARGETS = test_open test_create test_close test_getsize test_read_contig test_write_contig test_read_strided test_write_strided test_async_read_contig test_async_read_strided test_async_write_contig test_async_write_strided test_rw_combine_contig

all: ${TARGETS} ${OBJS}

%.o: %.c 
	${CC} ${CFLAGS} -c $< -o $@
    

${TARGETS} : ${OBJS}

$(TARGETS) : % : %.o
	$(CC) $<  ${OBJS} ${LDFLAGS} -o $@
        

vpath %.c ..


.PHONY:
clean:
	@rm -f *.o *~  ${TARGETS}

	
	
	

