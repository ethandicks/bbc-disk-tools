#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "common.h"

char infile[256], outfile[256];
char copyfiles[256][256];
int verbose=0, noglob=0, prompt=0, nofiles;

void syntax(void)
{
   fprintf(stderr,"Syntax: bbccp -i <bbcimage> <bbcfile> [<bbcfile>...] <outfile>\n");
   fprintf(stderr,"Flags are:\n");
   fprintf(stderr,"\t-h|-?      - this message\n");
   fprintf(stderr,"\t-p         - prompt on filename\n");
   fprintf(stderr,"\t-n         - noglob\n");
   fprintf(stderr,"\t-v         - verboseg\n");
}

int parse_cli(argc,argv)
int argc;
char **argv;
{
   int i;

   while ( 1 )
   {
      i=getopt( argc, argv, "i:pnvh?" );
      if (i == -1 ) /* no more parameters */
         break;

      switch (i)
      {
         case 'i' :
            strncpy(infile,optarg,256);
            break;

         case 'p' :
            prompt=1;
            break;

         case 'n' :
            noglob=1;
            break;

         case 'v' :
            verbose=1;
            break;

         case 'h' :
         case '?' :
            syntax();
            exit (0);

         default :
            fprintf (stderr,"Illegal option specified: %c\n",i);
            syntax();
            exit (1);
      }
   }
   if ( strlen( infile ) == 0 )
   {
      fprintf(stderr, "Input file not specified\n" );
      syntax();
      exit (1);
   }

   if ( optind >= argc )
   {
      fprintf(stderr, "No files provided!\n" );
      syntax();
      exit (1);
   }
 
   if ( optind == argc - 1)
   {
      strncpy(outfile,argv[optind],256);
      strncpy(copyfiles[0],argv[optind],256);
      nofiles=1;
      return;
   }

   i=optind;
   while (i < argc-1)
   {
      strncpy(copyfiles[i-optind],argv[i],256);
      i++;
   }
   nofiles=i-optind;
   strncpy( outfile, argv[argc-1], 256 );
}
 
int main( argc, argv)
int argc;
char **argv;
{
   unsigned char *dblock;
   int success,i,j,dirsize,next,copied;
   FILE *inhandle,*outhandle;
   directory_type *directory;
   char permissions[12],current_out[256];
   struct stat buf;

   parse_cli(argc,argv);

   dblock=(char *) calloc( 1, 260 );
   directory=(directory_type *) calloc( 144, sizeof( directory_type ) );

   inhandle=fopen( infile, "r" );
   if ( inhandle == 0 )
   {
      fprintf(stderr, "Failed to open infile %s\n", infile );
      return 1;
   }

   dirsize=bbc_list_directory( directory, inhandle );
   if ( dirsize == -1 )
   {
      fprintf(stderr, "Failed to read directory\n" );
      fclose( inhandle );
      return 1;
   }

   for (j=0; j<nofiles; j++)
   {
      for (i=0; i<dirsize; i++)
      {
         success=(noglob==0)?fnmatch( copyfiles[j], directory[i].name, 0):(strcmp(copyfiles[j],directory[i].name)!=0);
         if (success == 0)
         {
            /* Check whether the outfile is a directory */
            success=0;
            success=stat(outfile, &buf);
            if (S_ISDIR(buf.st_mode) && (success == 0))
            {
               sprintf(current_out, "%s/%s",outfile,directory[i].name);
            }
            else
            {
               strncpy(current_out,outfile,256);
            }
            
            if (verbose) printf("Copying file %s as %s\n",directory[i].name,current_out);
            outhandle=fopen( current_out, "w" );
            if ( outhandle == 0 )
            {
               fprintf(stderr, "Failed to open outfile %s\n", current_out );
               return 1;
            }
            bbc_copy_file( inhandle, outhandle, directory[i] );
            fclose( outhandle );
            copied=1;
         }
      }
      
      if ( ! copied )
      {
         fprintf(stderr, "Failed to find file %s\n",copyfiles[j] );
         fclose( inhandle );
         exit(2);
      }
   }
   fclose( inhandle );
   exit(0);
}
