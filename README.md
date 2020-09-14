These tools let you manipulate disk image files for the BBC Micro.

   bbcls - list contents of a disc image, UNIX style
   bbccp - extract a file from a disc image
   bbcmk - turn a load of 'real' files into a disc image

Usage:
---------
   bbcls -i <image> [-alh]
   
   -a - show all files (not implemented!)
   -l - do a unix like long listing of files
   -h - help

   eg bbcls -i dave.ssd -l

   bbccp -i <image> [-nvph] <file> [outfile/directory]
   
   -n - noglob - do not glob wildcards, treat them as literal file names
   -v - verbose
   -h - help

   Note 1: When globbing, on Unix the shell will try to expand glob, put this
   in single quotes to get around it
   Note 2: Like Unix copy if you specify many files to one file the file will
   be overwritten by the last extracted file.

   eg bbccp -i dave.ssd $.!BOOT
       - extracts $.!BOOT to $.!BOOT
      bbccp -i dave.ssd '$.!BOOT' out
       - extracts $.!BOOT to the file out
      bbccp -i dave.ssd '$.*' bbcstuff       (where bbcstuff is a dir)
       - extracts the whole of the $.* files to the bbcstuff directory

   bbcmk <outfile> <filelist>

   Note 1: outfile is a new disk image to be constructed and will contain
   all of the files in <filelist>
   Note 2: files to be placed in the disk image must be .inf files (file
   data plus load, save, and run addresses and file permission bits).

To Make
-------
Type make all


Original version (C) Copyright 30/12/1999 David Lodge and "All programs released into the public domain under the GPL."
