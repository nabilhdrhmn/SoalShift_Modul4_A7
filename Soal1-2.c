#define FUSE_USE_VERSION 28

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include <sys/stat.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static const char *dirpath = "/home/daniel/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	car fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath,stbuf);

	if(res == -1)
	{
		return -errno;
	}

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	if(strcmp(path,"/")==0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath,"%s%s",dirpath,path);

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if(dp==NULL) return -errno;

	while ( (de = readdir(dp)) != NULL)
	{
		struct stat st;
		memset(&st,0,sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type<<12;
		if(filler(buf,de->d_name,&st,0))
			break;
	}

	closedir(dp);
	return 0;
}
