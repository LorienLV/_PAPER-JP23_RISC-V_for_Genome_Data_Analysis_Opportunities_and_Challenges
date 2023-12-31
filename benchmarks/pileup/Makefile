# Set compiler
CC:=gcc
CXX:=g++

ifeq ($(arch),sse41)
	ARCH_FLAGS=-msse4.1
else ifeq ($(arch),avx2)
	ifeq ($(CXX), icpc)
		ARCH_FLAGS=-march=core-avx2 #-xCORE-AVX2
	else	
		ARCH_FLAGS=-mavx2
	endif
else ifeq ($(arch),avx512)
	ifeq ($(CXX), icpc)
		ARCH_FLAGS=-xCORE-AVX512
	else	
		ARCH_FLAGS=-mavx512bw
	endif
else ifeq ($(arch),native)
		ARCH_FLAGS=-march=native
else ifneq ($(arch),)
	## To provide a different architecture flag like -march=core-avx2.
		ARCH_FLAGS=$(arch)
endif

# Set flags.
CFLAGS:=-g -Wall -pthread -fPIC -std=c99 -O3 -fopenmp $(ARCH_FLAGS)
CPPFLAGS:=-D_FORTIFY_SOURCE=2 -DPRINT_OUTPUT=1
DEPFLAGS:=-MD -MP # Autogenerate dependencies.
LDFLAGS:=-lz -lm -lbz2 -llzma -lpthread -lcurl -lcrypto -ldl
# INCLUDES=

# Perf
PERF_ANALYSIS=0

# Intel VTune Profiler
VTUNE_ANALYSIS=0

ifeq ($(VTUNE_ANALYSIS),1)
	INCLUDES+=$(VTUNE_INCLUDES)
	LDFLAGS+=$(VTUNE_LDFLAGS)
endif

# Fujitsu Advanced Profiler
FAPP_ANALYSIS=0

# Dynamorio
DYNAMORIO_ANALYSIS=0

# PWR
PWR=0

ifeq ($(PWR),1)
	INCLUDES+=$(PWR_INCLUDES)
	LDFLAGS+=$(PWR_LDFLAGS)
endif

# RAPL_STOPWATCH
RAPL_STOPWATCH=0

ifeq ($(RAPL_STOPWATCH),1)
	INCLUDES+=$(RAPL_STOPWATCH_INCLUDES)
	LDFLAGS+=$(RAPL_STOPWATCH_LDFLAGS)
endif

CPPFLAGS+=-DPERF_ANALYSIS=$(PERF_ANALYSIS) -DVTUNE_ANALYSIS=$(VTUNE_ANALYSIS) -DFAPP_ANALYSIS=$(FAPP_ANALYSIS) -DDYNAMORIO_ANALYSIS=$(DYNAMORIO_ANALYSIS) -DPWR=$(PWR) -DRAPL_STOPWATCH=$(RAPL_STOPWATCH)

# Directories.
BUILD_DIR:=build
SRC_DIR:=src
OBJ_DIR:=$(BUILD_DIR)/obj

# Htslib
HTS_LIB_BUILD_DIR:=$(BUILD_DIR)/libhts
HTS_LIB:=$(HTS_LIB_BUILD_DIR)/lib/libhts.a
INCLUDES+=-I$(HTS_LIB_BUILD_DIR)/include
# If the library is appended instead of prepended there is
# an undefined reference error.
LDFLAGS:=$(HTS_LIB) $(LDFLAGS)

#
# Htslib
#
$(HTS_LIB):
	mkdir -p $(@D)
	cp htslib.tar.bz2 $(HTS_LIB_BUILD_DIR)
	tar -xf $(HTS_LIB_BUILD_DIR)/htslib.tar.bz2 -C $(HTS_LIB_BUILD_DIR)
	rm -f $(HTS_LIB_BUILD_DIR)/htslib.tar.bz2
	cd $(HTS_LIB_BUILD_DIR)/htslib && \
	CC='$(CC)' CXX='$(CXX)' \
	CFLAGS='-O2 $(ARCH_FLAGS)' CXXFLAGS='-O2 $(ARCH_FLAGS)' \
	./configure --prefix=`pwd`/../ --enable-bz2=no --enable-lzma=no --with-libdeflate=no --enable-libcurl=no  --enable-gcs=no --enable-s3=no && \
	$(MAKE) VERBOSE=1 && \
	$(MAKE) install

# Files.
SRCS:=$(shell find $(SRC_DIR) -name "*.c")
OBJS:=$(subst $(SRC_DIR),$(OBJ_DIR),$(SRCS:%.c=%.o))
DEPS:=$(subst $(SRC_DIR),$(OBJ_DIR),$(SRCS:%.c=%.d))

#
# Executables.
#
PILEUP:=$(BUILD_DIR)/pileup
PILEUP_OBJS:=medaka_bamiter.o medaka_common.o medaka_counts.o
PILEUP_OBJS:=$(addprefix $(OBJ_DIR)/, $(PILEUP_OBJS))

#
# Generate executables
#
$(PILEUP): $(HTS_LIB) $(PILEUP_OBJS)
	$(CC) $(CFLAGS) $(PILEUP_OBJS) $(LDFLAGS) -o $@

#
# Generate objects.
#
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(INCLUDES) $(DEPFLAGS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include dependencies.
-include $(DEPS)

# The default goal is the next goal.
.DEFAULT_GOAL:=
.PHONY: all
all: PILEUP

PILEUP: $(PILEUP)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*
