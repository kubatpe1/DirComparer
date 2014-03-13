DirComparer
===========
Version 1.0

User Guide
==========

1. About the program
--------------------

DirComparer is a simple tool for Unix (and Unix-like) operating systems. It allows user to simply compare two directory trees (meaning recursively including their content). In addition, it can be used to synchronise content of directory trees and to make recursive copies of them.


2. Getting and compiling the program
------------------------------------

TODO


3. Running the program
----------------------

### Basic usage overview ###

    dircmp.out [-s] [-w] FIRST SECOND

 * `-w` ... compares content of regular files as well (With files)

 * `-s` ... synchronises content of both files

 * `FIRST` ... path (full or relative) to first directory

 * `SECOND` ... path (full or relative) to second directory

 * return value ... 0 if directories are similar, 1 otherwise 

### Typical scenarios ###

    dircmp.out /dir1 /dir2

- compares just the directory structure and names of files included in them
- does not check sizes, types or actual content of files

    dircmp.out -w /dir1 /dir2

- compares directory structure, file types, sizes and content of regular files
- if anything doesn't match between the two directories, prints a message to standard output

    dircmp.out -s /dir1 /dir2

- compares directory structure
- if any directories or files don't have their counterpart, they are copied

    dircmp.out -w -s /dir1 /dir2

- compares directory structure and file content
- in addition to previous scenario, if a file from /dir1 has its counterpart in /dir2 with different content, it is replaced by content of the file in /dir1

    dircmp -s /dir1 /newdir

- if newdir doesn't exist, copies dir1 recursively with its content