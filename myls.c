/*ls -l implementation */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#define BUFFER_SIZE 256 //better fit for this than just 80 chars

/*
DIRENT STRUCTURE
struct dirent {
               ino_t          d_ino;       // inode number //
               off_t          d_off;       // not an offset; see NOTES //
               unsigned short d_reclen;    // length of this record //
               unsigned char  d_type;      // type of file; not supported
                                              by all filesystem types //
               char           d_name[256]; // filename //
           };

STAT STRUCTURE
struct stat {
    dev_t     st_dev;     // ID of device containing file //
    ino_t     st_ino;     // inode number //
    mode_t    st_mode;    // protection //
    nlink_t   st_nlink;   // number of hard links //
    uid_t     st_uid;     // user ID of owner //
    gid_t     st_gid;     // group ID of owner //
    dev_t     st_rdev;    // device ID (if special file) //
    off_t     st_size;    // total size, in bytes //
    blksize_t st_blksize; // blocksize for file system I/O //
    blkcnt_t  st_blocks;  // number of 512B blocks allocated //
    time_t    st_atime;   // time of last access //
    time_t    st_mtime;   // time of last modification //
    time_t    st_ctime;   // time of last status change //
};
*/


void ls()
{
	struct dirent *d;
	DIR *dir;
	char *temp = malloc(BUFFER_SIZE);;
	//Get the pwd
	if (getcwd(temp,BUFFER_SIZE)==NULL)
	{
		printf("Failed fetching current working directory\n");
		return;
	}
	printf("%s\n",temp);
	dir = opendir(temp);
	
	while((d = readdir(dir))!=NULL)
	{
		//ls -l ->no hidden files, filter the names that start with dot
		if(d->d_name[0]!='.')
		printf("%s ",d->d_name);
	}
	printf("\n");
	closedir(dir);
}

int main(void)
{
	ls();
	return 0;
}