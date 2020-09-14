#ifndef COMMON
#define COMMON

/* directory filetype */
typedef struct
{
   char name[255];
   int owner_permissions;
   int group_permissions;
   int world_permissions;
   int owner;
   int group;
   int flags;
   int start_track;
   int start_sector;
   int size;
} directory_type;

int bbc_read_sector( char *dblock, int track, int sector, FILE *handle );
int bbc_list_directory( directory_type *direct, FILE *handle );
int bbc_copy_file( FILE *inhandle, FILE *outhandle, directory_type direct );
int bbc_assign_sector( directory_type *directory, int nofiles, directory_type *direct);

#endif
