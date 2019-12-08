#include "ext2core.h"

static ext2 *fs;

// initialize fs struct
int init_ext2_core(int img_fd) 
{
    fs = (ext2*) calloc(1, sizeof(ext2));
	fs->first_sb = (ext2_sb*) calloc(1, sizeof(ext2_sb));

	lseek(img_fd, SB_OFFSET, SEEK_SET);
	size_t n = read(img_fd, fs->first_sb, sizeof(ext2_sb));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}
	
	fs->blocksize = 1024 << (fs->first_sb->log_block_size);
	fs->blocks_per_group = fs->first_sb->blocks_per_group;
	fs->inodes_per_group = fs->first_sb->inodes_per_group;
	// this is to be updated
	fs->inode_size = 128;

	// read first block group header
    ext2_group_desc bgroup;
	lseek(img_fd, fs->first_sb->first_data_block + fs->blocksize, SEEK_SET);
	n = read(img_fd, &bgroup, sizeof(ext2_group_desc));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}

	ext2_inode *root = (ext2_inode*) calloc(1, sizeof(ext2_inode));
	// root inode has index = 1
	int offset = bgroup.bg_inode_table*(fs->blocksize)+1*fs->inode_size;
	lseek(img_fd, offset, SEEK_SET);
	n = read(img_fd, root, sizeof(ext2_inode));
	if (n <= 0) {
		printf("init failed\n");
		return -1;
	}

	fs->root = root;
	fs->img_fd = img_fd;

	return 0;
}

ext2_inode* read_inode(int inonum)
{
    int bgroup_num = (inonum – 1) / fs->inodes_per_group;
    int index = (inonum – 1) % fs->inodes_per_group;
    ext2_inode *ret = (ext2_inode*) calloc(1, sizeof(ext2_inode));
    lseek(fs->img_fd, index*fs->inode_size, SEEK_SET);
    read(fs->img_fd, ret, sizeof(ext2_inode));

    return ret;
}

ext2_inode* find_file(const char *path)
{
    if (strcmp(path, "/") == 0)
        return fs->root;

    ext2_inode *found = (ext2_inode*) calloc(1, sizeof(ext2_inode));
    char root_name = path[0];
    // skip leading '/'
    snprintf(path, strlen(path), "%s", path+1);

    found = search_file(fs->root, path);
    return found;
}

ext2_inode* search_file(ext2_inode* root, path)
{
    char[256] name;
    int i = 0;
    while (path[i] != '/') {
        name[i] = path[i];
        i++;
    }
    name[i] = '\0';

    if (S_ISDIR(root->i_mode)) {
        int n = 0;
        int name_len = 0;
        int blk_num = 0;
        ext2_dir_entry entry;
        char[256] read_name;

        for (int i = 0; i < 13; i++) {
            blk_num = root->i_blocks[i];
            if (blk_num == 0)
                return NULL;

            lseek(fs->img_fd, blk_num*fs->blocksize, SEEK_SET);
            read(fs->img_fd, &entry.inode, 4);
            read(fs->img_fd, &entry.rec_len, 2);
            read(fs->img_fd, &entry.name_len, 2);
            name_len = entry.rec_len - 8;
            read(fs->img_fd, &read_name, name_len);
            if (strcmp(read_name, name) == 0) {
                snprintf(path, strlen(path), "%s", path + strlen(name) + 1);
                int inonum = entry.inode;
                ext2_inode *found = read_inode(inonum);
                if (found == NULL) {
                    return NULL;
                }

                return search_file(found, path);
            }
        }

        // TBD: indirect blocks search
    }

    if (S_ISREG(root->i_mode)) {
        return root;
    }
}

