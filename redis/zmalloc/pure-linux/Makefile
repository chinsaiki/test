FINAL_CFLAGS:=
FINAL_LIBS:= 
MALLOC=libc

# Backwards compatibility for selecting an allocator
ifeq ($(USE_TCMALLOC),yes)
	MALLOC=tcmalloc
endif

ifeq ($(USE_TCMALLOC_MINIMAL),yes)
	MALLOC=tcmalloc_minimal
endif

ifeq ($(USE_JEMALLOC),yes)
	MALLOC=jemalloc
endif

ifeq ($(USE_JEMALLOC),no)
	MALLOC=libc
endif

ifeq ($(MALLOC),tcmalloc)
	FINAL_CFLAGS+= -DUSE_TCMALLOC
	FINAL_LIBS+= -ltcmalloc
endif

ifeq ($(MALLOC),tcmalloc_minimal)
	FINAL_CFLAGS+= -DUSE_TCMALLOC
	FINAL_LIBS+= -ltcmalloc_minimal
endif

ifeq ($(MALLOC),jemalloc)
	FINAL_CFLAGS+= -DUSE_JEMALLOC 
	FINAL_LIBS += -ljemalloc
endif

default: all

.DEFAULT:
	@echo $(MALLOC) $(FINAL_CFLAGS) $(FINAL_LIBS) 


install:
	cd src && $(MAKE) $@

.PHONY: install

