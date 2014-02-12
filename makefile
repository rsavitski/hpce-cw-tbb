# Makefile for posix and gcc

CXX=clang++
CPPFLAGS = -I include -Wall -std=c++11
LDFLAGS = 
LDLIBS = -lm -ltbb

# Turn on optimisations
CPPFLAGS += -O2

# Indicate where you have put the TBB installer
TBB_DIR = /home/kryu/studies/HPCE/cw3/tbb42_20131118oss

TBB_INC_DIR = $(TBB_DIR)/include

# Choose the correct library for your build
TBB_LIB_DIR = $(TBB_DIR)/build/linux_intel64_gcc_cc4.7_libc2.13_kernel3.2.0_release

CPPFLAGS += -I $(TBB_INC_DIR)
LDFLAGS += -L $(TBB_LIB_DIR)

# The very basic parts
FOURIER_CORE_OBJS = src/fourier_transform.o src/fourier_transform_register_factories.o

# implementations
FOURIER_IMPLEMENTATION_OBJS =  src/fast_fourier_transform.o	src/direct_fourier_transform.o

FOURIER_IMPLEMENTATION_OBJS += src/rs5010/direct_fourier_transform_parfor.o src/rs5010/fast_fourier_transform_taskgroup.o

FOURIER_OBJS = $(FOURIER_CORE_OBJS) $(FOURIER_IMPLEMENTATION_OBJS)

bin/test_fourier_transform : src/test_fourier_transform.cpp $(FOURIER_OBJS)
	-mkdir bin
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

bin/time_fourier_transform : src/time_fourier_transform.cpp $(FOURIER_OBJS)
	-mkdir bin
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

all : bin/test_fourier_transform bin/time_fourier_transform
