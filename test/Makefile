TARGET = x86_64-unknown-linux-gnu-
ARCH =
CARCH =
CFLAGS = -I.. -DDEBUG

.PHONY: %.test

all: min_heap_extract.svg min_heap_decrease_key.svg

%.out: %.o
	$(CC) $^ -o $@

%.test: %.out
	./$<

%.svg: %.dot
	dot -Tsvg $< > $@

test_min_heap.out: ../min_heap.o

test_ucs.out: ../ucs.o ../min_heap.o

min_heap_%.dot: test_min_heap.test

include ../common.mk
