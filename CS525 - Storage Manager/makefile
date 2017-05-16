#compiler to be used is gcc
CC=gcc
#define the target file
all:test_assign1_1
test_assign1_1: test_assign1_1.o dberror.o storage_mgr.o
	$(CC) test_assign1_1.o dberror.o storage_mgr.o -o test_assign1_1
dberror.o: dberror.c dberror.h
	$(CC) -c dberror.c
storage_mgr.o: storage_mgr.c storage_mgr.h
	$(CC) -c storage_mgr.c
clean :
	rm dberror.o test_assign1_1.o storage_mgr.o
