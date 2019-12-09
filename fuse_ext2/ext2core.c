#include "ext2core.h"

#define DIR_CAPACITY 100

static ext2 *fs;

ext2_inode* search_file(ext2_inode* root, const char *path);
ext2_inode* find_file(const char *path);

// initialize fs struct
int init_ext2_core(int img_fd) 
{
	fs = (ext2*) calloc(1, sizeof(ext2));
	fs->first_sb = (ext2_sb*) calloc(1, sizeof(ext2_sb));

	lseek(img_fd, SB_OFFSET, SEEK_SET);
	size_t n = read(img_fd, fs->first_sb, sizeof(ext2_sb));
	if (n <= 0) {
		fprintf(stderr, "init failed\n");
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
		fprintf(stderr, "init failed\n");
		return -1;
	}

	ext2_inode *root = (ext2_inode*) calloc(1, sizeof(ext2_inode));
	// root inode has index = 1
	int offset = bgroup.bg_inode_table*(fs->blocksize);
	lseek(img_fd, offset + 1*fs->inode_size, SEEK_SET);
	n = read(img_fd, root, sizeof(ext2_inode));
	if (n <= 0) {
		fprintf(stderr, "init failed\n");
		return -1;
	}
	

	fs->root = root;
	fs->img_fd = img_fd;
	fs->inode_table_offset = offset;

	return 0;
}

ext2_inode* read_inode(int inonum)
{
    int index = (inonum - 1) % fs->inodes_per_group;
    ext2_inode *ret = (ext2_inode*) calloc(1, sizeof(ext2_inode));
    lseek(fs->img_fd, fs->inode_table_offset+index*fs->inode_size, SEEK_SET);
    read(fs->img_fd, ret, sizeof(ext2_inode));

    return ret;
}

ext2_inode* find_file(const char *path)
{
    if (strcmp(path, "/") == 0)
        return fs->root;

	char new_path[256];
    // skip leading '/'
    snprintf(new_path, strlen(path), "%s", path+1);

    return search_file(fs->root, new_path);
}

ext2_inode* search_file(ext2_inode* root, const char *path)
{
	fprintf(stderr, "search file: %s   <----\n", path);
    char name[256] = {};
    char new_path[256] = {};
	int last_step = 0;
    int i = 0;
    
    while ((path[i] != '/') && (path[i] != 0)) {
        name[i] = path[i];
        i++;
    }
    
	name[i] = 0;
	if (path[i] == 0) {
		last_step = 1;
	}
	
    if (S_ISDIR(root->i_mode)) {
        int n = 0;
        int name_len = 0;
        int blk_num = 0;
        ext2_dir_entry entry;

        for (int i = 0; i < 13; i++) {
            blk_num = root->i_block[i];
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
				read(fs->img_fd, read_name, entry.name_len);
				offset += entry.rec_len;
				
				if (strcmp(read_name, name) == 0) {
					int inonum = entry.inode;
					ext2_inode *found = read_inode(inonum);
					if (found == NULL) {
						fprintf(stderr, "didn't find inode\n");
						return NULL;
					}
					
					snprintf(new_path, strlen(path), "%s", path + strlen(name) + 1);
					// it means that the search is over
					if (last_step)
						return found;
					
					return search_file(found, new_path);
				}
			}
		}
	}

	// TBD: indirect blocks search
	return NULL;
}

char** list_dir(const char *path)
{
    ext2_inode *dir = find_file(path);
    if (dir == NULL)
        return NULL;

    if (!S_ISDIR(dir->i_mode))
        return NULL;

    char** dirs = (char**) calloc(DIR_CAPACITY, sizeof(char*));
    if (dirs == NULL)
        return NULL;
		
    for (int i = 0; i < DIR_CAPACITY; i++) {
        dirs[i] = (char*) calloc(256, sizeof(char));
        if (dirs[i] == NULL)
            return NULL;
    }

    int name_len = 0;
    int blk_num = 0;
	unsigned int dir_count = 0;
    ext2_dir_entry entry;

    for (int i = 0; i < 13; i++) {
        blk_num = dir->i_block[i];
        if (blk_num == 0)
            break;

        int offset = 0;
        while (offset < fs->blocksize) {
            char name[256] = {};
            lseek(fs->img_fd, blk_num*fs->blocksize + offset, SEEK_SET);
            read(fs->img_fd, &entry.inode, 4);
            if (entry.inode == 0) {
                break;
            }

            read(fs->img_fd, &entry.rec_len, 2);
            read(fs->img_fd, &entry.name_len, 1);
            read(fs->img_fd, &entry.file_type, 1);
            read(fs->img_fd, name, entry.name_len);
            offset += entry.rec_len;

            strcpy(dirs[dir_count], name);
            dir_count += 1;

            if (dir_count % DIR_CAPACITY == 0) {
                dirs = (char**) realloc(dirs, dir_count + DIR_CAPACITY);
                if (dirs == NULL)
                    return NULL;

                for (int j = dir_count ; j < dir_count+DIR_CAPACITY; j++) {
                    dirs[j] = calloc(256, sizeof(char));
                    if (dirs[j] == NULL)
                        return NULL;
                }

            }

        }
    }
	
	dirs[dir_count] = NULL;
    return dirs;
}


