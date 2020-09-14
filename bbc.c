#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int bbc_read_sector(dblock,track,sector,handle)
char *dblock;
int track,sector;
FILE *handle;
{
   int offset,success;

   /* tracks start at 0 - 256 bytes/sector */
   offset=track*(10 * 256);

   /* work out sector offset - sectors start at 0! */
   offset+=(sector*256); /* offset for the sector start */

   if ( fseek( handle, offset, SEEK_SET) != 0 ) return -1;
   success=fread( (void *) dblock, 256, 1, handle);
   if ( success != 1 ) return -1;
   return 0;
}

int bbc_write_sector(dblock,track,sector,handle)
char *dblock;
int track,sector;
FILE *handle;
{
   int offset,success;

   /* tracks start at 0 - 256 bytes/sector */
   offset=track*2560;

   /* work out sector offset - sectors start at 0! */
   offset+=(sector*256); /* offset for the sector start */

   if ( fseek( handle, offset, SEEK_SET) != 0 ) return -1;
   success=fwrite( (void *) dblock, 256, 1, handle);
   if ( success != 1 ) return -1;
   return 0;
}

int bbc_list_directory( direct, handle )
directory_type *direct;
FILE *handle;
{
   unsigned char *direct_block, *offset=0;
   int current_entry=0, i, j, end=0, success=0, namesize=0, entries=0;
   int next_track=0, next_sector=0;

   direct_block=(char *)malloc(260);
   offset=direct_block;

   /* Read the filenames first */
   success=bbc_read_sector( direct_block, next_track, next_sector, handle );
   if ( success == -1 )
   {
      free( direct_block );
      return -1;
   }
   offset=direct_block;

   /* Ignore the Discname */
   offset+=0x08;

   i=0;
   while( ( end == 0 ) && ( i < 30 ) )
   {
      /* first read the directory entry */
      direct[current_entry].name[0]=offset[7];
      direct[current_entry].name[1]='.';

      namesize=2;
      for ( j=0; j < 7 ; j++)
      {
         direct[current_entry].name[j+2]=(offset[j]);
         if (offset[j]!=' ') namesize++;
      }

      if (strlen(direct[current_entry].name) == 0)
      { /* name doesn't exist; assume the directory ain't right! */
         current_entry--;
         end=1;
      }
      direct[current_entry].name[namesize]='\0';
      current_entry++;
      i++;
      offset+=8;
   }

   /* Now read the directory information */
   success=bbc_read_sector( direct_block, 0, 1, handle );
   if ( success == -1 )
   {
      free( direct_block );
      return -1;
   }
   offset=direct_block;

   /* Ignore the Discname */
   offset+=8;

   i=0;
   current_entry=0;
   end=0;
   while( ( end == 0 ) && ( i < 30 ) )
   {
      direct[current_entry].owner=offset[0]+(offset[1]*256)+((offset[6]&0x0c)>>2)*65536;
      direct[current_entry].group=offset[2]+(offset[3]*256)+((offset[6]&0xc0)>>6)*65536;;
      direct[current_entry].size=offset[4]+(offset[5]*256)+((offset[6]&0x30)>>4)*65536;;

      direct[current_entry].start_track=offset[7]/10;
      direct[current_entry].start_sector=offset[7]%10;

      if (strlen(direct[current_entry].name)==0)
      { /* end of directory */
         current_entry--;
         break;
      }

      direct[current_entry].owner_permissions=7;
      direct[current_entry].group_permissions=7;
      direct[current_entry].world_permissions=7;
      direct[current_entry].flags=1;
      current_entry++;
      i++;
      offset+=8;
   }

   free( direct_block );
   return current_entry+1;
}

int bbc_copy_file( inhandle, outhandle, direct )
FILE *inhandle, *outhandle;
directory_type direct;
{
   unsigned char *direct_block, *offset;
   int i, j, success=0, endsize=256;
   int next_track=direct.start_track, next_sector=direct.start_sector;
   int sectors=direct.size/256+1, filesize=direct.size;

   direct_block=(char *)malloc(260);

   for ( i=0; i<sectors; i++ )
   {
      success=bbc_read_sector( direct_block, next_track, next_sector, inhandle ); 
      if ( success == -1 )
      {
         free( direct_block );
         return -1;
      }
      next_sector++;
      next_sector%=10;
      next_track=(next_sector==0)?next_track+1:next_track;

      if ( filesize < 256 )
      {
         endsize=filesize;
      }
      else
      {
         filesize-=256;
      }
      success=fwrite( direct_block, endsize, 1, outhandle );
      if ( success != 1 )
      {
         free( direct_block );
         return -1;
      }
   }
   return 0;
}

int bbc_assign_sector( directory, nofiles, direct )
directory_type *direct;
directory_type *directory;
int nofiles;
{
   char *map;
   int i,j,sector_count,start_sector;
   /* first draw up a map of what sectors are free - this is totally 
      inefficient - so sue me! */

   map=calloc(1,500);
   memset(map,0,500);
   /* Reserve the catalogue */
   map[0]=map[1]=1;

   for (i=0; i<nofiles; i++)
   {
      sector_count=(directory[i].size/256)+1;
      start_sector=(directory[i].start_track*10)+directory[i].start_sector;

      for (j=start_sector; j<start_sector+sector_count; j++)
      {
         map[j]=1;
      }
   }

   /* Now find free space long enough! */
   sector_count=direct->size/256+1;
   i=0;
   do
   {
      while (map[i]==1 && i < 400) i++;
      if ( i < 400 )
      {
         j=i;
         while (map[j]==0 && j < 400 ) j++;
         if ( (j-i) >= sector_count )
         {
            /* Huzzah enough free space! */
            start_sector=i;
            break;
         }
      }
   } while ( i < 400 );

   direct->start_sector=start_sector % 10;
   direct->start_track=start_sector / 10;
   printf("%d %d\n",start_sector % 10, start_sector / 10);

   return 0;
}

int bbc_put_file( inhandle, outhandle, direct, directory, nofiles )
FILE *inhandle, *outhandle;
directory_type direct;
directory_type *directory;
int nofiles;
{
   int i,j;
   int sector_start=2;

   
   
}
