# BIKE reference and optimized implementations assume that OpenSSL and NTL libraries are available in the platform.

# To compile this code for NIST KAT routine use: make bike-nist-kat
# To compile this code for demo tests use: make bike-demo-test

# TO EDIT PARAMETERS AND SELECT THE BIKE VARIANT: please edit defs.h file in the indicated sections.

# The file measurements.h controls how the cycles are counted. Note that #define REPEAT is initially set to 10,
# which means that every keygen, encaps and decaps is repeated 10 times and the number of cycles is averaged.

# Verbose levels: 0, 1, 2 or 3

VERBOSE=0

CC:=arm-linux-gnueabihf-g++
CFLAGS:=-O3 -mcpu=cortex-a9

SRC:=*.c ntl.cpp FromNIST/rng.c

NTL_PREFIX:=
GF2X_PREFIX:=
GMP_PREFIX:=
PETALINUX_PROJECT_DIR:=

OPENSSL_DIR := $(PETALINUX_PROJECT_DIR)/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/usr/include/openssl 

INCLUDE:= -I. -I$(NTL_PREFIX)/include -L$(NTL_PREFIX)/lib -I$(GMP_PREFIX)/include -L$(GMP_PREFIX)/lib -I$(GF2X_PREFIX)/include -L$(GF2X_PREFIX)/lib -I$(OPENSSL_DIR) -L$(PETALINUX_PROJECT_DIR)/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/lib -L$(PETALINUX_PROJECT_DIR)/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/usr/lib --sysroot=$(PETALINUX_PROJECT_DIR)/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi -Wl,-rpath-link=$(PETALINUX_PROJECT_DIR)/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/lib -Wl,-rpath-link=$(PETALINUX_PROJECT_DIR)/images/linux/sdk/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/usr/lib -lcrypto -lssl -lm -ldl -lntl -lgmp -lgf2x -lpthread

all: bike-nist-kat

bike-demo-test: $(SRC) *.h tests/test.c
	$(CC) $(CFLAGS) tests/test.c $(SRC) $(INCLUDE) -DVERBOSE=$(VERBOSE) -DNIST_RAND=1 -o $@

bike-nist-kat: $(SRC) *.h FromNIST/*.h FromNIST/PQCgenKAT_kem.c
	$(CC) $(CFLAGS) FromNIST/PQCgenKAT_kem.c $(SRC) $(INCLUDE) -DVERBOSE=$(VERBOSE) -DNIST_RAND=1 -o $@

clean:
	rm -f PQCkemKAT_*
	rm -f bike*
