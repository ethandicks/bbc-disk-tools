#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

char infile[256], outfile[256];
int verbose, noglob, inter;

void syntax(command)
char *command;
{
   printf("Syntax: %s -f <bbcimage> <bbcfile> [<bbcfile>...] <outfile>\n", \
      command);
   printf("Flags are:\n");
   printf("\t-h|-?      - this message\n");
   printf("\t-i         - interactive - prompt on filename\n");
   printf("\t-n         - noglob\n");
   printf("\t-v         - verboseg\n");
   exit(0);
}

char *lengthen_string(instring)
char *instring;
{
   char i;
   if (strlen(instring) == 7) return instring;
   for (i=strlen(instring); i< 7; i++)
   {
      strcat(instring," ");
   }
   return instring;
}

int parse_arguments(argc,argv)
int argc;
char *argv[];
{
   int carg=1,c=0,carg2;

   infile[0]='\0';

   while (carg<argc)
   {
      c=0;
      while (argv[carg][0]='-')
      {
         carg2=carg;
         while (c<strlen(argv[carg])-1)
         {
            switch(argv[carg][++c])
            {
               case 'h' :
               case '?' :
                  syntax(argv[0]);

               case '-' :
                  break;

               case 'n' :
                  noglob=1;
                  break;

               case 'v' :
                  verbose=1;
                  break;

               case 'i' :
                  inter=1;
                  break;

               case 'f' :
                  strncpy(infile,argv[++carg2],255);
                  break;

               default :
                  printf("%s: Invalid flag: -%c\n",argv[0],argv[carg][c]);
                  syntax(argv[0]);
            }
            if (argv[carg][++c]=='-') break;
            carg++;
         }
      }
      if ( argv[carg][0] != '-' ) break;
   }

   if (infile[0]=='\0')
   {
      printf("%s: No input file specified\n",argv[0]);
      syntax(argv[0]);
   }
 
   return carg;
}
 
int main( argc, argv)
int argc;
char **argv;
{
   unsigned char *dblock;
   int success,i,j,dirsize,next;
   FILE *inhandle,*outhandle;
   directory_type *directory;
   char permissions[12];
   char working[256],name[256],load[256],exec[256],ssize[256];
   unsigned char dirname;
   int size,files;
   int sector=2;

   strncpy( infile, argv[1], 255);
   next=2;

   dblock=(char *) calloc( 1, 260 );
   directory=(directory_type *) calloc( 144, sizeof( directory_type ) );

   outhandle=fopen( infile, "w" );
   if ( outhandle == 0 )
   {
      printf( "Failed to open file %s\n", infile );
      return 1;
   }

   /* work out the files */

   for (i=2; i < argc; i++)
   {
      sprintf(working,"%s.inf",argv[i]);
      inhandle=fopen( working, "r" );
      if ( inhandle == 0 )
      {
         sprintf(working,"%s.INF",argv[i]);
         inhandle=fopen( working, "r" );
         if ( inhandle == 0 )
         {
            strcpy(working,argv[i]);
            inhandle=fopen( working, "r" );
            if ( inhandle == 0 )
            {
               printf( "Failed to open file %s\n", working );
               return 1;
            }
         }
      }

      fscanf(inhandle,"%s %lx %lx %lx",directory[i-2].name,&directory[i-2].owner,&directory[i-2].group,&directory[i-2].size);
/*      fscanf(inhandle,"%s %s %s %s",name,load,exec,ssize);
      strcpy(directory[i-2].name,name);
      directory[i-2].owner=strtol(load,NULL,16);
      directory[i-2].group=strtol(exec,NULL,16);
      directory[i-2].size=strtol(ssize,NULL,16);*/

      if (directory[i-2].size<17)
      {
         FILE *temphandle;
         char work2[256];

         strcpy(work2,working);
         strtok(work2,".");
         temphandle=fopen( work2,"r");
         fseek(temphandle,0,SEEK_END);
         directory[i-2].size=ftell(temphandle);
         fclose(temphandle);
      }
     
      fclose(inhandle);
      /* Now we need to assign a sector to the file (and reserve the space!)*/
      directory[i-2].start_track=sector/10;
      directory[i-2].start_sector=sector%10;

      /* and reserve the space */
      sector+=(directory[i-2].size / 256)+1;
   }

   /* first make a blank disc! */
   memset(dblock,0,256);
   for (i=0;i<40;i++);
   {
      for (j=0;j<10;j++)
      {
         bbc_write_sector(dblock,i,j,outhandle);
      }
   }
   /* Right now we have the catalogue, let's make the damn thing! */
   /* First the filenames */
   files=argc-2;
   memset(dblock,0,256);
   memcpy(dblock,"BBCMK",5);
   for (i=0;i<files;i++)
   {
      dirname=directory[i].name[0];
      sprintf(working,"%7s%c",lengthen_string(directory[i].name+2),dirname);
      memcpy(dblock+((i+1)*8),working,8);
   }

   /* And put it! */
   success=bbc_write_sector( dblock, 0, 0, outhandle);
   if ( success == -1 )
   {
      printf("Failed to write to outfile\n");
      fclose(outhandle);
      return 1;
   }

   /* Sector 1 - the information! */
   memset(dblock,0,256);
   /* fill in the information block */
   dblock[5]=files*8; 
   dblock[6]=1;
   dblock[7]=0x90;
   for (i=0;i<files;i++)
   {
      int base=(i+1)*8; 
      dblock[base]=directory[i].owner % 256;
      dblock[base+1]=directory[i].owner / 256 % 65536;
      dblock[base+2]=directory[i].group % 256;
      dblock[base+3]=directory[i].group / 256 % 65536;
      dblock[base+4]=directory[i].size % 256;
      dblock[base+5]=directory[i].size / 256 % 65536;
      /* Now the fun byte! - unsupported 'cos it only get's used for roms! */
      dblock[base+6]=((directory[i].start_track*10) >> 8);
      dblock[base+7]=directory[i].start_track*10+directory[i].start_sector;
      printf("%d %d\n",dblock[base+6],dblock[base+7]);
   }
   /* And put it! */
   success=bbc_write_sector( dblock, 0, 1, outhandle);
   if ( success == -1 )
   {
      printf("Failed to write to outfile\n");
      fclose(outhandle);
      return 1;
   }

   /* Now write the files */
   for (i=0;i<files;i++)
   {
      int file_sectors=directory[i].size/256+1;
      int filesize=directory[i].size;
      int endsize=256;
      int next_sector=directory[i].start_sector;
      int next_track=directory[i].start_track;

      strcpy(working,argv[i+2]);
      strtok(working,".");
      printf("Processing %s\n",working);
      inhandle=fopen( working, "r" );
      if ( inhandle == 0 )
      {
         printf( "Failed to open file %s\n", infile );
         return 1;
      }
      /* Now read in the file 256 bytes at a time! */

      for (j=0; j<file_sectors; j++)
      {
         if ( filesize < 256 ) endsize=256;
         fread(dblock,endsize,1,inhandle);
         bbc_write_sector(dblock,next_track,next_sector,outhandle);
         next_sector++;
         next_sector%=10;
         next_track=(next_sector==0)?next_track+1:next_track;
         filesize-=256;
      }
      fclose (inhandle);
   }
   fclose(outhandle); 
}
