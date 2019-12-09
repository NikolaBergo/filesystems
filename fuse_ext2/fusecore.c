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
	fprintf(stderr, "GET_ATTR\n");
	st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time(NULL);
	st->st_mtime = time(NULL);
	
	// allow only for reading
	st->st_mode = S_IRUSR | S_IRGRP | S_IROTH;
	
	ext2_inode *inode = find_file(path);
	if (inode == NULL) {
	    fprintf(stderr, "failed to get attr\n");
	    return -ENOENT;
	}

	// disable original access flags
	st->st_mode |= (inode->i_mode >> 12 << 12);
	st->st_nlink = inode->i_links_count;
	st->st_size = inode->i_size;

	return 0;
}

static int ext2_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			            off_t offset, struct fuse_file_info *fi)
{
	fprintf(stderr, "READ_DIR %s\n", path);
	char **dirs = list_dir(path);

	if (dirs == NULL)
	    return -EFAULT;
		
	int i = 0;
	while (dirs[i]) {
        filler(buf, dirs[i], NULL, 0);
        i++;
    }

	return 0;
}

static int ext2_opendir(const char *path, struct fuse_file_info *fi)
{
	fprintf(stderr, "OPENDIR\n");
    return 0;
}


static int ext2_open(const char *path, struct fuse_file_info *fi)
{
	fprintf(stderr, "OPEN\n");
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int ext2_read(const char *path, char *buf, size_t size, off_t offset,
		             struct fuse_file_info *fi)
{
	size_t n;

    ext2_inode *inode = find_file(path);
    if (inode == NULL)
        return -ENOENT;

    n = read(inode, buf, size, offset);

	return n;
}

static int ext2_release(const char *path, struct fuse_file_info *fi)
{
    return 0;
}

static int ext2_releasedir(const char *path, struct fuse_file_info *fi)
{
    return 0;
}

static struct fuse_operations ext2_op = {
	.getattr	= ext2_getattr,
	.readdir	= ext2_readdir,
	.open		= ext2_open,
	.read		= ext2_read,
	.opendir    = ext2_opendir,
	.release    = ext2_release,
	.releasedir = ext2_releasedir
};

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("too few arguments!\n");
		return -1;
	}
	
	img_fd = open(argv[argc-1], O_RDONLY);
	fsync(img_fd);
	if (img_fd < 0) {
		printf("failed to open image file\n");
		return -1;
	}
	
	int err = init_ext2_core(img_fd);
	if (err < 0)
	    return -1;
	
	return fuse_main(argc - 1, argv, &ext2_op, NULL);
}