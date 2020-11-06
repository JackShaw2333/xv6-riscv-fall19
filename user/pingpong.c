#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int parent_fd[2];
    int child_fd[2];
    
    char p_recv[0];
    char c_recv[0];
    char p2c = 'a';
    char c2p = 'z';

    // parent process
    pipe(parent_fd);
    pipe(child_fd);
    if (write(parent_fd[1], &p2c, 1) != 1) {
        fprintf(2, "parent write error\n");
    }

    int pid = fork();

    if (pid > 0) {
        // parent process
        wait(); // wait for child process
        if (read(child_fd[0], p_recv, 1) != 1) {
            fprintf(2, "parent read error\n");
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }
    } else if (pid == 0) {
        // child process
        if (read(parent_fd[0], c_recv, 1) != 1) {
            fprintf(2, "child read error\n");
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if (write(child_fd[1], &c2p, 1) != 1) {
            fprintf(2, "child write error\n");
        }
    } else {
        // fork error
        fprintf(2, "fork error\n");
    }
    exit();
}
