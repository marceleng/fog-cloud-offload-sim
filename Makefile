CC = gcc
CFLAGS = -Wall -Werror -pedantic
INCLUDE="."

BUILDIR = $(shell pwd)/build

SUBDIRS = caches helpers queueing

2LRU_OBJ = $(addprefix $(BUILDIR)/,2lru.o)

HSOURCES = $(shell find $(SUBDIRS) -type f -iname '*.c')

HOBJECTS = $(addprefix $(BUILDIR)/,$(notdir $(HSOURCES:%.c=%.o)))

.PHONY: clean create_dir

2lru: create_dir $(HOBJECTS) $(2LRU_OBJ)
	echo $(HOBJECTS)
	echo $(2LRU_OBJ)
	$(CC) -I$(INCLUDE) $(2LRU_OBJ) $(HOBJECTS) -o 2lru_sim

.SECONDEXPANSION:

$(BUILDIR)/%.o: $$(shell gfind -name $$*.c)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $^

create_dir:
	mkdir -pm 777 $(BUILDIR)

clean:
	rm -fr $(BUILDIR)
