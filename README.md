DirComparer
===========

**Version 1.0**


User Guide
==========


1. About the program
--------------------


DirComparer is a simple tool for Unix (and Unix-like) operating systems. It allows user to simply compare two directory trees (meaning recursively including their content). In addition, it can be used to synchronise content of directory trees and to make recursive copies of them.


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

 * `-s` ... synchronises content of both files
 
 * `-t` ... specifies how many threads should be used for checking the file content and copying

### Typical scenarios ###


    dircmp.out /dir1 /dir2

Compares just the directory structure and names of files included in them. Does not check sizes, types or actual content of files.

    dircmp.out -w /dir1 /dir2

Compares directory structure, file types, sizes and content of regular files. If anything doesn't match between the two directories, prints a message to standard output.

    dircmp.out -s /dir1 /dir2

Compares directory structure. If any directories or files don't have their counterpart, they are copied.

    dircmp.out -w -s /dir1 /dir2

Compares directory structure and file content. In addition to previous scenario, if a file from /dir1 has its counterpart in /dir2 with different content, it is replaced by content of the file in /dir1.

    dircmp -s /dir1 /newdir

If newdir doesn't exist, copies dir1 recursively with its content.