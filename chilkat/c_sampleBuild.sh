#!/bin/bash -ev

gcc -c c_Sample.c -o"c_Sample.o"

g++ c_Sample.o -o"c_Sample.exe" -L. -lchilkat-9.5.0 -L/c/MinGW/lib -lcrypt32 -lws2_32 -ldnsapi


