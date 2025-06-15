#
# TANAT - Text ANAlyze Toolkit
# Author: Kaige Zheng
# Email: kambrikg@outlook.com
# Homepage: https://github.com/KaigeZheng/tanat
# License: MIT License
# Description: Makefile for building the text analysis toolkit.
#
CC = gcc
CFLAGS = -Wall -O2
TARGET = tanat
SRCS = tanat.c

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

rebuild: clean all
