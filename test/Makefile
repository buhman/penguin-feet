TARGET = x86_64-unknown-linux-gnu-
ARCH =
CARCH =
CFLAGS = -I..

.PHONY: %.test

all: min_heap_extract.svg min_heap_decrease_key.svg

%.out: %.o
	$(CC) $^ -o $@

%.test: %.out
	./$<

%.svg: %.dot
	dot -Tsvg $< > $@

test_min_heap.out: ../min_heap.o

min_heap_extract.dot: test_min_heap.test
min_heap_decrease_key.dot: test_min_heap.test

include ../common.mk
