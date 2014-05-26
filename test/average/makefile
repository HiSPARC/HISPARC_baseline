#
# Makefile
#
# Jorian van Oostenbrugge
# Used to make test-average
#
# Sources:
# http://gribblelab.org/CBootcamp/12_Compiling_linking_Makefile_header_files.html at 26-05-14
# https://github.com/pobept63/CS50/blob/master/Makefile at 26-05-14
#

# Set compiler
CC = gcc

# Set flags
CFLAGS = -Wall

# Name of binary
BINARY = test-average

# All header files space separated
HDRS = ../minunit.h ../../src/sequence.h

# Source files space separated
SRCS = test-average.c ../../src/sequence.c

# All object files
OBJS = $(SRCS:.c=.o)

# default target
$(BINARY): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# dependencies 
$(OBJS): $(HDRS) Makefile

$(BINARY): $(OBJ)

# Do not pollute filesystem
clean:
	rm -f core $(EXE) $(OBJS)
