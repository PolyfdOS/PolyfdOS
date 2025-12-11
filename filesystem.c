#include "filesystem.h"

/* Global file table */
static struct file file_table[MAX_FILES];

/** Helper: string comparison */
static int fs_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/** Helper: string copy */
static void fs_strcpy(char *dest, const char *src, int max_len)
{
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

/** Helper: string length */
static int fs_strlen(const char *str)
{
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/** Helper: extract directory and filename from path */
static void fs_split_path(const char *filepath, char *directory, char *filename)
{
    int i, last_slash = -1;
    int len = fs_strlen(filepath);
    
    /* Find last slash */
    for (i = len - 1; i >= 0; i--) {
        if (filepath[i] == '/') {
            last_slash = i;
            break;
        }
    }
    
    if (last_slash == -1) {
        /* No directory, just filename */
        fs_strcpy(directory, "/", MAX_FILENAME);
        fs_strcpy(filename, filepath, MAX_FILENAME);
    } else if (last_slash == 0) {
        /* Root directory */
        fs_strcpy(directory, "/", MAX_FILENAME);
        fs_strcpy(filename, filepath + 1, MAX_FILENAME);
    } else {
        /* Copy directory part */
        int j;
        for (j = 0; j < last_slash && j < MAX_FILENAME - 1; j++) {
            directory[j] = filepath[j];
        }
        directory[j] = '\0';
        
        /* Copy filename part */
        fs_strcpy(filename, filepath + last_slash + 1, MAX_FILENAME);
    }
}

/** fs_init */
void fs_init(void)
{
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        file_table[i].in_use = 0;
        file_table[i].filename[0] = '\0';
        file_table[i].directory[0] = '\0';
        file_table[i].size = 0;
        file_table[i].is_directory = 0;
    }
}

/** fs_create */
int fs_create(const char *filepath, const char *content, int size)
{
    char directory[MAX_FILENAME];
    char filename[MAX_FILENAME];
    int i;
    
    /* Split path */
    fs_split_path(filepath, directory, filename);
    
    /* Check if file already exists - if so, update it */
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].filename, filename) == 0 &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            
            /* Update existing file */
            int copy_size = size < MAX_FILE_CONTENT ? size : MAX_FILE_CONTENT;
            int j;
            for (j = 0; j < copy_size; j++) {
                file_table[i].content[j] = content[j];
            }
            file_table[i].size = copy_size;
            return 0;
        }
    }
    
    /* Find empty slot */
    for (i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].in_use) {
            file_table[i].in_use = 1;
            fs_strcpy(file_table[i].filename, filename, MAX_FILENAME);
            fs_strcpy(file_table[i].directory, directory, MAX_FILENAME);
            
            /* Copy content */
            int copy_size = size < MAX_FILE_CONTENT ? size : MAX_FILE_CONTENT;
            int j;
            for (j = 0; j < copy_size; j++) {
                file_table[i].content[j] = content[j];
            }
            file_table[i].size = copy_size;
            
            return 0;
        }
    }
    
    /* No space */
    return -1;
}

/** fs_read */
int fs_read(const char *filepath, char *buffer, int max_size)
{
    char directory[MAX_FILENAME];
    char filename[MAX_FILENAME];
    int i;
    
    /* Split path */
    fs_split_path(filepath, directory, filename);
    
    /* Find file */
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].filename, filename) == 0 &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            
            /* Copy content */
            int copy_size = file_table[i].size < max_size ? 
                           file_table[i].size : max_size;
            int j;
            for (j = 0; j < copy_size; j++) {
                buffer[j] = file_table[i].content[j];
            }
            
            return copy_size;
        }
    }
    
    /* File not found */
    return -1;
}

/** fs_delete */
int fs_delete(const char *filepath)
{
    char directory[MAX_FILENAME];
    char filename[MAX_FILENAME];
    int i;
    
    /* Split path */
    fs_split_path(filepath, directory, filename);
    
    /* Find and delete file */
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].filename, filename) == 0 &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            
            file_table[i].in_use = 0;
            return 0;
        }
    }
    
    /* File not found */
    return -1;
}

/** fs_exists */
int fs_exists(const char *filepath)
{
    char directory[MAX_FILENAME];
    char filename[MAX_FILENAME];
    int i;
    
    /* Split path */
    fs_split_path(filepath, directory, filename);
    
    /* Find file */
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].filename, filename) == 0 &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            
            return 1;
        }
    }
    
    return 0;
}

/** fs_list_directory */
void fs_list_directory(const char *directory, void (*callback)(const char *filename))
{
    int i;
    
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            
            if (callback) {
                callback(file_table[i].filename);
            }
        }
    }
}

/** fs_mkdir */
int fs_mkdir(const char *dirpath)
{
    char directory[MAX_FILENAME];
    char dirname[MAX_FILENAME];
    int i;
    
    /* Split path */
    fs_split_path(dirpath, directory, dirname);
    
    /* Check if already exists */
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].filename, dirname) == 0 &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            return 0;  /* Already exists */
        }
    }
    
    /* Find empty slot */
    for (i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].in_use) {
            file_table[i].in_use = 1;
            file_table[i].is_directory = 1;
            fs_strcpy(file_table[i].filename, dirname, MAX_FILENAME);
            fs_strcpy(file_table[i].directory, directory, MAX_FILENAME);
            file_table[i].size = 0;
            return 0;
        }
    }
    
    return -1;  /* No space */
}

/** fs_is_directory */
int fs_is_directory(const char *filepath)
{
    char directory[MAX_FILENAME];
    char filename[MAX_FILENAME];
    int i;
    
    /* Split path */
    fs_split_path(filepath, directory, filename);
    
    /* Find entry */
    for (i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            fs_strcmp(file_table[i].filename, filename) == 0 &&
            fs_strcmp(file_table[i].directory, directory) == 0) {
            
            return file_table[i].is_directory;
        }
    }
    
    return 0;
}