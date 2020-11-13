#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    char c;
    char *args[16];
    char *q;
    int i;
    int t;

    for (i = 0; i < 16; ++i) {
        args[i] = 0;
    }

    for (i = 0; i < argc; ++i) {
        args[i] = argv[i];
    }

    t = argc;
    args[i] = malloc(16 * sizeof(char));
    q = args[i];
    while (read(0, &c, sizeof(char)) == sizeof(char)) {
        if (c == 0) {
            for (i = t; i < 16; ++i) {
                if (args[i] != 0) {
                    free(args[i]);
                }
            }
            break;
        }

        if (c == '\n') {
            *q = '\0';

            int pid = fork();
            if (pid == 0) {
                exec(args[1], args+1);
                exit();
            } else if (pid > 0) {
                wait();
                for (i = t; i < 16; ++i) {
                    if (args[i] != 0) {
                        free(args[i]);
                        args[i] = 0;
                    }
                }
                i = t;
                args[i] = malloc(16 * sizeof(char));
                q = args[i];
            } else {
                fprintf(2, "xargs: fork error\n");
            }
        } else if (c == ' ' || c == '\t') {
            *q = '\0';
            ++i;
            args[i] = malloc(16 * sizeof(char));
            q = args[i];
        } else {
            *q++ = c;
        }
    }

    exit();
}