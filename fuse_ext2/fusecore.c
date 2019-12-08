#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "ext2core.h"
#include "ext2core.c"

static int img_fd;

static int ext2_getattr(const char *path, struct stat *st)
{
	int res = 0;
	st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time(NULL);
	st->st_mtime = time(NULL);
	// allow only for reading
	st->st_mode = S_IRUSR | S_IRGRP | S_IROTH;
	
	ext2_inode *inode = find_file(path);
	if (inode == NULL) {
	    printf("failed to get attr\n")
	    return -ENOENT;
	}

	// disable original access flags
	st->st_mode |= ((inode->i_mode >> 8) << 8);
	st->st_nlink = inode->i_links_count;
	st->st_size = inode->i_size;

	return res;
}

static int ext2_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	if (strcmp(path, "/") != 0)
		return -ENOENT;
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	//filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

//static int ext2_opendir(const char *, struct fuse_file_info *)


static int ext2_open(const char *path, struct fuse_file_info *fi)
{
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int ext2_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	//if (strcmp(path, hello_path) != 0)
	//	return -ENOENT;

	//len = strlen(hello_str);
	//f (offset < len) {
	//	if (offset + size > len)
	//		size = len - offset;
	//	memcpy(buf, hello_str + offset, size);
	//} else
	//	size = 0;

	return 0;
}

static struct fuse_operations ext2_op = {
	.getattr	= ext2_getattr,
	.readdir	= ext2_readdir,
	.open		= ext2_open,
	.read		= ext2_read,
};

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("too few arguments!\n");
		return -1;
	}
	
	if (argc > 3) {
		printf("too many arguments!\n");
		return -1;
	}
	
	img_fd = open(argv[2], O_RDONLY);
	if (img_fd < 0) {
		printf("failed to open image file\n");
		return -1;
	}
	
	int err = init_ext2_core(img_fd);
	if (err < 0)
	    return -1;
	
	return fuse_main(argc - 1, argv, &ext2_op, NULL);
}