DirComparer
===========

**Version 1.0**


User Guide
==========


1. About the program
--------------------


DirComparer is a simple tool for Unix (and Unix-like) operating systems. It allows user to compare two directory trees (meaning recursively including their content) and find out their differences. In addition, it can be used to synchronize content of directory trees and to make recursive copies of them. Comparation and copying work multi-threded for greater efficiency and performance.


2. Getting and compiling the program
------------------------------------

### Requirements ###

DirComparer requires any kind of Unix-like operating system (anything compatible with POSIX and POSIX-threads). It was tested and should run on Linux (Ubuntu), Mac OS X and OpenBSD.

### Compiling ###

The program is distribued in the form of source codes which can be downloaded from GitHub repository (<https://github.com/ips1/DirComparer>). It has to be compiled for the target platform first in order to be used.

Compilation requires C compiler (should be present on all supported operating systems) and `make` utility. Compilation can be started by running following command in the direcotry with source files:

    make

A binary file `dircmp.out` is created in the same folder upon completion of the compilation along with other files. You can perform a cleanup (deletion of all the files created) by running command:

    make clean


3. Using the program
----------------------


### Basic usage overview ###

Program should be run from a terminal using following pattern:

    dircmp.out [-s] [-w] [-tn] FIRST SECOND
	
Where:

 * `FIRST` ... path (full or relative) to first directory

 * `SECOND` ... path (full or relative) to second directory

 * return value ... 0 if directories are similar, 1 otherwise 

You can use any combination of following optional switches:

 * `-w` ... compares content of regular files as well (With files)

 * `-s` ... synchronizes content of both files
 
 * `-t` ... specifies how many threads should be used for checking the file content and copying
 
### Synchronization details ###

If `-s` option is used, content of both directories is changed to match the other one. If a file exists in one tree without equivalent in the other, it is simply copied. The same goes for whole subdirectories. If a file has an equivalent with same name, one file is replaced by the other following those rules (if rule with higher preference can't decide, rule with lower preference is used):

 1. Newer file is kept
 2. Larger file is kept
 3. File from the first directory is kept

### Typical scenarios ###


    dircmp.out /dir1 /dir2

Compares just the directory structure and names of files included in them. Does not check sizes, types or actual content of files.

    dircmp.out -w /dir1 /dir2

Compares directory structure, file types, sizes and content of regular files. If anything doesn't match between the two directories, prints a message to standard output.

    dircmp.out -s /dir1 /dir2

Compares directory structure. If any directories or files don't have their counterpart, they are copied.

    dircmp.out -w -s /dir1 /dir2

Compares directory structure and file content. In addition to previous scenario, if a file from /dir1 has its counterpart in /dir2 with different content, the older file is replaced with the newer one.

    dircmp -s /dir1 /newdir

If newdir doesn't exist, copies dir1 recursively with its content.


4. Multiple thread usage
------------------------

During the program execution, files are compared and copied in separate threads that are independent on the main thread apart from being served the file names. This can be a fine performance boost especially for a directory trees with significant number of larger files.

Number of threads that are used for file operations can be specified by a command line option `-t`. Default value is 4, minimum value is 1 (for obvious reasons). The maximum value is not specified, although there is a limit which you should never exceed - the maximum number of files that can be opened at once on your system. If you allow more threads to be used, there might be situation in which all of those threads need to open a file and some of them would fail to do so. This would lead to program failure. If you use the `-s` option, the limit should be even lower - every thread potentially requires two opened files at once.

Ideal number of threads can't be recommended. It depends on the actual system, architecture, data input etc. Generally the default 4 threads seem to be working fine on most systems.


5. Quick source code overview
-----------------------------

### Implementation details ###

The program is implemented using producer-consumer scheme. There is one thread (the main one) crawling through the file system in folder specified by user, checking existence of the same folder structure in the other folder. Whenever a non-directory file is encountered, its path relative to the starting folder is added to the queue. It is then fetched by a consumer thread, which reconstructs both absolute paths (for file in the first folder and file in the second folder), checks, whether both exist, then checks if they are the same and copies the first one if needed. 

This entire process compares uses one directory as a model and checks whether the other one is the same. In order to perform both-way check, the same comparing function is run twice, second time using the directories swapped and avoiding checking the files which exist in both trees again.

### Source files ###

The source code is divided into a few stand-alone source files (which are compiled separately using the make utility). Some of those modules are designed to work on itself and can be used by any other program as an added library. List of those modules follows:

 * `stringstack.h`, `stringstack.c` - simple stack with variable size for storing c-strings (char *), uses automatic reallocation
 
 * `prod_con.h`, `prod_con.c` - structure (queue) for storing data in producer-consumer type of applications; uses locks and conditional variables and provides a very simple interface for both producer and consumer threads
 
 * `pthread_wrapper.h`, `pthread_wrapper.c` - simple wrapper over most used pthread functions which checks the return values and in case of an error prints an error message and exits the program
 
Interface for all those modules is very simple and can be found (along with short description) in the header files.

The other modules are specific for this program:

 * `thread.h`, `thread.c` - code executed by all the consumer threads - performs matching and copying of provided files

 * `search.h`, `search.c` - main logic of the application, functions for spawning threads, performing the producer part (crawling the filesystem and preparing the file names)
 
 * `main.h`, `main.c` - entry point for the application, parses user options and calls corresponding function from `search.c`