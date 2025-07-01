# Project 5: Parallel zip

In this project, I implement the parallel zip project with possibility to both compress and uncompress the files using multiple threads where possible. This has all the features implemented that are mentioned in the assignment, so it is aiming for full points (5).

# Usage

There are two C files, `pzip.c`, which is the multithreaded RLE compressor implementation, and `punzip.c`, the multithreaded uncompressor implementation.  
  
To compile and run `pzip.c` with the included test input files, use the following commands
```
gcc -Wall -Werror -pthread -O -o pzip pzip.c
./pzip t1.txt > t1.z
```

To compile and run `punzip.c` to uncompress the file ending in .z, do the following:  

```
gcc -Wall -Werror -pthread -O -o punzip punzip.c
./punzip t1.z > o.txt
```
This should result in a file that is equal to the t1.txt   
To compress or uncompress multiple files, simply add the file names with a space like such:
```
./pzip t1.txt t2.txt > t1.z
./punzip t1.z t2.z > o.txt
```
There is also a file called `t3.txt` included, that has a long run of the same characters, from which it's possible to check that the RLE compression actually reduces file size in ideal scenario.

# Documentation and code explanation

I will briefly go over the code and the way it works, but both of the .c files are fairly well commented so I won't go into 100% detail.

## pzip
This program functions by using some raw Linux system calls such as get_nprocs to get available amount of processors and create a thread for each of them, and for reading the files it uses read and mmap for efficiency, as described in the assignment. Multiple files are handled by just making their data into 1 large buffer, where the files are just appended after each other.
  
The basic concept of this program is as follows: take an input file, and divide it by the amount of processors available into equally sized chunks, which are then compressed individually with the `rleCompress` function. This implementation didn't use locks or condition variables for communication between threads, and instead after all of these chunks have been compressed, they are single-threadedly checked for any errors between boundaries (for example letter "a" continues across to another chunk) and these are then fixed, and then all of the chunks are combined. This results in the compressed file.  

Example image of running the program, and showcasing correct result with hexdump:
![Alt text](./pzip.png?raw=true "Optional Title")

## punzip
This program is mainly single-threaded, but if multiple files are given to compress, a thread is created for each of the files. Similarly like in the pzip program, if multiple files are given for unzipping, they are just outputted in a row into a single output file.

The implementation of this is fairly simple, create a pthread for every file given, and in the function unzipFile, we simply assume that the file is in correct format (so has an rle pair of first the count and then the character) and start reading it. First we read the 4 byte integer `count`, and then the 1 byte character `ch`. Then we loop the number of count, pushing the corresponding character to output count times. This is done in all of the threads, and then they are just written to a file in row.  
  
Example of running punzip with a single and multiple files (cat command might be a bit unclear because of no newline):
![Alt text](./punzip.png?raw=true "Optional Title")

# AI declaration
AI was used for some help with understanding the system calls and threading, but no code was generated. Linux man pages and other websites were used for some help, but they are linked in the code where used.