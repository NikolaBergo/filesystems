#include "ext2core.h"

static ext2 *fs;
FILE *debug;


ext2_inode* search_file(ext2_inode* root, const char *path);
ext2_inode* find_file(const char *path);

// initialize fs struct
int init_ext2_core(int img_fd) 
{
	debug = fopen("debug", "w+");
	fprintf(debug, "INIT\n");
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
	lseek(img_fd, (fs->first_sb->first_data_block + 1) * fs->blocksize, SEEK_SET);
	n = read(img_fd, &bgroup, sizeof(ext2_group_desc));
	if (n <= 0) {
		fprintf(debug, "init failed\n");
		return -1;
	}
	printf("ino table %d\n", bgroup.bg_inode_table);

	ext2_inode *root = (ext2_inode*) calloc(1, sizeof(ext2_inode));
	// root inode has index = 1
	int offset = bgroup.bg_inode_table*(fs->blocksize)+1*fs->inode_size;
	printf("inode table: %d\n", bgroup.bg_inode_table );
	lseek(img_fd, offset, SEEK_SET);
	n = read(img_fd, root, sizeof(ext2_inode));
	if (n <= 0) {
		fprintf(debug, "init failed\n");
		return -1;
	}

	fs->root = root;
	fs->img_fd = img_fd;
	fs->inode_table_offset = offset;
	
	ext2_inode* found = find_file("/1/file.01\n");
	if (found)
		printf("number %d\n", S_ISREG(found->i_mode));

	return 0;
}

ext2_inode* read_inode(int inonum)
{
    int bgroup_num = (inonum - 1) / fs->inodes_per_group;
    int index = (inonum - 1) % fs->inodes_per_group;
    ext2_inode *ret = (ext2_inode*) calloc(1, sizeof(ext2_inode));
    lseek(fs->img_fd, fs->inode_table_offset+index*fs->inode_size, SEEK_SET);
    read(fs->img_fd, ret, sizeof(ext2_inode));

    return ret;
}

ext2_inode* find_file(const char *path)
{
	fprintf(stderr, "findfile %s\n", path);
    if (strcmp(path, "/") == 0)
        return fs->root;

	char new_path[256];
    ext2_inode *found = (ext2_inode*) calloc(1, sizeof(ext2_inode));
    // skip leading '/'
    snprintf(new_path, strlen(path), "%s", path+1);

    found = search_file(fs->root, new_path);
    return found;
}

ext2_inode* search_file(ext2_inode* root, const char *path)
{
	fprintf(stderr, "search file: %s\n", path);
    char name[256];
    char new_path[256];
    int i = 0;
    
    while (path[i] != '/') {
        name[i] = path[i];
        i++;
    }
    name[i] = '\0';

    if (S_ISDIR(root->i_mode)) {
		fprintf(stderr, "DIR\n");
        int n = 0;
        int name_len = 0;
        int blk_num = 0;
        ext2_dir_entry entry;

        for (int i = 0; i < 13; i++) {
            blk_num = root->i_block[i];
			fprintf(stderr, "III: %d blk_num: %d\n", i, blk_num);
            if (blk_num == 0)
                return NULL;
			
			int offset = 0;
			while (offset < fs->blocksize) {
				char read_name[256] = {};
				lseek(fs->img_fd, blk_num*fs->blocksize + offset, SEEK_SET);
				read(fs->img_fd, &entry.inode, 4);
				if (entry.inode == 0) {
					break;
				}
				read(fs->img_fd, &entry.rec_len, 2);
				read(fs->img_fd, &entry.name_len, 1);
				read(fs->img_fd, &entry.file_type, 1);
				fprintf(stderr, "direntry ino %d rec_len %d name_len %d\n", entry.inode, 
							entry.rec_len, entry.name_len);
				
				read(fs->img_fd, read_name, entry.name_len);
				fprintf(stderr, "name_read %s name_search %s\n", read_name, name);
				offset += entry.rec_len;
				fprintf(stderr, "off %d\n", offset);
				
				if (strcmp(read_name, name) == 0) {
					snprintf(new_path, strlen(path), "%s", path + strlen(name) + 1);
					int inonum = entry.inode;
					fprintf(stderr, "inonum for 1 dir: %d\n", entry.inode);
					ext2_inode *found = read_inode(inonum);
					if (found == NULL) {
						fprintf(stderr, "didn't find inode\n");
						return NULL;
					}

					return search_file(found, new_path);
				}
			}
		}
	}

	// TBD: indirect blocks search

    if (S_ISREG(root->i_mode)) {
		fprintf(stderr, "I'M REG\n");
        return root;
    }
}

