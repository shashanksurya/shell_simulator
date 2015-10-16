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
#include <time.h>
#include <pwd.h>
#include <grp.h>
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

void print_lastmtime(struct stat *sf)
{//following linux manual way

	char output[BUFFER_SIZE];
	strftime(output,BUFFER_SIZE,"%b %d %H:%M",localtime(&(sf->st_mtime)));
	printf("%s ",output);
}

void print_usrgrp(struct stat *sf)
{
	struct passwd *u = getpwuid(sf->st_uid);
	struct group  *g = getgrgid(sf->st_gid);
	printf("%s ",u->pw_name);
	printf("%s ",g->gr_name);
}

void print_type(struct stat *sf)
{
	if(sf->st_mode & S_IFDIR)
		printf("d");
	else
		printf("-");
}

void print_permissions(struct stat *sf)
{
	//User
	if(sf->st_mode & S_IRUSR)
		printf("r");
	else
		printf("-");
	if(sf->st_mode & S_IWUSR)
		printf("w");
	else
		printf("-");
	if(sf->st_mode & S_IXUSR)
		printf("x");
	else
		printf("-");
	//Group
	if(sf->st_mode & S_IRGRP)
		printf("r");
	else
		printf("-");
	if(sf->st_mode & S_IWGRP)
		printf("w");
	else
		printf("-");
	if(sf->st_mode & S_IXGRP)
		printf("x");
	else
		printf("-");
	//Other
	if(sf->st_mode & S_IROTH)
		printf("r");
	else
		printf("-");
	if(sf->st_mode & S_IWOTH)
		printf("w");
	else
		printf("-");
	if(sf->st_mode & S_IXOTH)
		printf("x");
	else
		printf("-");	
	printf(" %d ",(int)sf->st_nlink);
}

void print_total_block_size(char *temp)
{
	DIR *dir;
	struct dirent *d;
	struct stat *sf = malloc(sizeof(struct stat));
	unsigned long int bt = 0;
	char *fname;
	dir = opendir(temp);

	while((d = readdir(dir))!=NULL)
	{
		//ls -l ->no hidden files, filter the names that start with dot
		if(d->d_name[0]!='.')
		{
			fname = d->d_name;
			stat(fname,sf);
			bt+=sf->st_blocks;
		}
	}
	printf("total %ld\n",(bt/2));
	free(sf);
	closedir(dir);
}

void ls()
{
	struct dirent *d;
	DIR *dir;
	struct stat *sf = malloc(sizeof(struct stat));
	char *temp = malloc(BUFFER_SIZE);
	char *fname;
	//Get the pwd
	if (getcwd(temp,BUFFER_SIZE)==NULL)
	{
		printf("Failed fetching current working directory\n");
		return;
	}
	//printf("%s\n",temp);
	dir = opendir(temp);
	print_total_block_size(temp);
	while((d = readdir(dir))!=NULL)
	{
		//ls -l ->no hidden files, filter the names that start with dot
		if(d->d_name[0]!='.')
		{
			fname = d->d_name;
			stat(fname,sf);
			print_type(sf);
			print_permissions(sf);
			print_usrgrp(sf);
			printf("%ld ",sf->st_size);
			print_lastmtime(sf);
			printf("%s ",fname);
			printf("\n");
		}
	}
	printf("\n");
	closedir(dir);
	free(sf);
	free(temp);
}

int main(void)
{
	ls();
	return 0;
}