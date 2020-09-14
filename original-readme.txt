Inside this archive are 3 quick utilities for manipulating BBC disc images,
these are:

   bbcls - list a disc image unix style
   bbccp - extract a file from a disc image
   bbcmk - turn a load of 'real' files into a discimage

bbcls and bbccp are written around a generic disc interface system I've
been coding (I have modules for C64 discs and Apple 2 discs; and probably
Amstrad CPC soon! - if you want these send me an email (address below)!)

bbmk was a quick and nasty frig because I had a lot of archive of files which
I needed to put in an archive quickly!

Syntax's:
---------
   bbcls -i <image> [-alh]
   
   -a - show all files (not implemented!)
   -l - do a unix like long listing of files
   -h - help

   eg bbcls -i dave.ssd -l

   bbccp -i <image> [-nvph] <file> [outfile/directory]
   
   -n - noglob - ie match $.* to the file $.*
   -v - verbose
   -p - prompt (not implemented)
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

   Note 1: The filelist can have .inf already attached!
   
   eg bbcmk games.ssd *.inf

To Make
-------
Type make all

All programs released into the public domain under the GPL.

As for support, I coded these for my own use (and to get me back into C coding
- too long doing Sys Admin work :-), but if you do use them and want extra
stuff just send me an email (to dave@muspellheim.demon.co.uk)

(C) Copyright 30/12/1999 David Lodge
