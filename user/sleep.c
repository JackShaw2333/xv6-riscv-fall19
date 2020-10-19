#include "kernel/syscall.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(2, "Error: no arguments!\n");
    }

    int ticks = atoi(argv[1]);
    sleep(ticks);
    exit();
}
