CFLAGS = -g -Wall
LDFLAGS = -lm
CC = gcc

all: clean external_sort binary_file_manager generate_files

external_sort: priority-queue.h priority-queue.c external-sort.h external-sort.c
	$(CC) $(CFLAGS) -o external-sort priority-queue.c external-sort.c $(LDFLAGS)

binary_file_manager: binary-file-manager.h binary-file-manager.c
	$(CC) $(CFLAGS) -o binary-file-manager binary-file-manager.c $(LDFLAGS)

generate_files:
	./binary-file-manager -w file_500MB.bin 500
	./binary-file-manager -w file_1500MB.bin 1500
	./binary-file-manager -w file_4000MB.bin 4000

tests:
# SAME K, SAME MEMORY SIZES, DIFFERENT FILES SIZES
	./external-sort file_500MB.bin output_500MB-M100-K10.bin 100 10
	./external-sort file_1500MB.bin output_1500MB-M100-K10.bin 100 10
	./external-sort file_4000MB.bin output_4000MB-M100-K10.bin 100 10
# SAME K, SAME FILE SIZE, DIFFERENT MEMORY SIZES
#	./external-sort file_4000MB.bin output_4000MB-M100-K10.bin 100 10
	./external-sort file_4000MB.bin output_4000MB-M150-K10.bin 150 10
	./external-sort file_4000MB.bin output_4000MB-M300-K10.bin 300 10
# SAME MEMORY, SAME FILE SIZE, DIFFERENT K
	./external-sort file_4000MB.bin output_4000MB-M400-K10.bin 400 10
	./external-sort file_4000MB.bin output_4000MB-M400-K10.bin 400 25
	./external-sort file_4000MB.bin output_4000MB-M400-K10.bin 400 40

clean:
	rm -f *.out *.o external-sort kfile*.bin
