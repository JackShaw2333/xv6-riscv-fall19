#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

/**
 * for f in path:
 *     if f is dir:
 *         path = f
 *         recur()
 *     else if f.name matchs:
 *         printf(f.name)
 */

static char* fmtname(char *path) {
    char *p;
    for (p = path + strlen(path); p >= path && *p != '/'; --p);

    return ++p;
}

static int* get_next(char *pattern) {
    static int next[DIRSIZ];

    next[0] = -1;
    int j = 0;
    int k = -1;
    int pattern_len = strlen(pattern);
    while (j < pattern_len - 1) {
        if (k == -1 || pattern[j] == pattern[k]) {
            if (pattern[++j] == pattern[++k]) {
                next[j] = next[k];
            } else {
                next[j] = k;
            }
        } else {
            k = next[k];
        }
    }

    return next;
}

static int kmp_match(char *pattern, char *text) {
    int i = 0;
    int j = 0;

    int *next = get_next(pattern);
    
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    while (i < text_len && j < pattern_len) {
        if (j == -1 || text[i] == pattern[j]) {
            ++i;
            ++j;
        } else {
            j = next[j];
        }
    }

    if (j == pattern_len) {
        return i - j;
    } else {
        return -1;
    }
}

static void find(char *target_dir, char *pattern) {
    int fd;
    struct dirent de;
    struct stat st;
    char buf[64];
    char *p, *f_name;

    // get target_dir's file descriptor
    if ((fd = open(target_dir, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", target_dir);
        return;
    }

    // get fd's struct stat
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", target_dir);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_FILE:
        f_name = fmtname(target_dir);
        if (kmp_match(pattern, f_name) != -1) {
            printf("%s\n", target_dir);
        }
        break;
    case T_DIR:
        if (strlen(target_dir) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "find: target directory's name is too long\n");
            break;
        }
        // chdir(target_dir);
        strcpy(buf, target_dir);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (!strcmp(de.name, ".") || !strcmp(de.name, "..") || de.inum == 0)
                continue;
            
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = '\0';
            find(buf, pattern);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    char *target_dir;
    char *pattern;

    switch (argc) {
    case 1:
        target_dir = ".";
        pattern = "";
        break;
    case 2:
        target_dir = argv[1];
        pattern = "";
        break;
    case 3:
        target_dir = argv[1];
        pattern = argv[2];
        break;
    default:
        fprintf(2, "find: needs 3 arguments, offered to many\n");
        exit();
    }

    find(target_dir, pattern);

    exit();
}