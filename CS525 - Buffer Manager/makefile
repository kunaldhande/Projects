#compiler to be used is gcc
CC=gcc
#define the target file
all:test_assign2_1
test_assign2_1: test_assign2_1.o dberror.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
	$(CC) test_assign2_1.o dberror.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o -o test_assign2_1
dberror.o: dberror.c dberror.h
	$(CC) -c dberror.c
storage_mgr.o: storage_mgr.c storage_mgr.h
	$(CC) -c storage_mgr.c
buffer_mgr.o: buffer_mgr.c buffer_mgr.h dberror.h dt.h storage_mgr.h
	$(CC) -c buffer_mgr.c
buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr.h dberror.h dt.h storage_mgr.h buffer_mgr_stat.h
	$(CC) -c buffer_mgr_stat.c
clean :
	rm dberror.o test_assign2_1.o storage_mgr.o buffer_mgr.o buffer_mgr_stat.o
