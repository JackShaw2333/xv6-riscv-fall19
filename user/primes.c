#include "kernel/types.h"
#include "user/user.h"

#define INTLEN (sizeof(int))
#define EOF 0
#define STDIN 0
#define STDOUT 1
#define STDERR 2

/**
 * pipe(pipe)
 * if pid > 0:
 *     close(pipe[0])
 *     write(pipe[1])
 *     close(pipe[1])
 * else if pid == 0:
 *     close(STDIN);
 *     dup(pipe[0])
 *     close(pipe[0])
 *     read(pipe[1])
 *     close(STDIN)
 */
static void generate_process(void);

int main(int argc, char *argv[]) {
    int left_pipe_fd[2];
    pipe(left_pipe_fd); 
    // fprintf(STDERR, "pid %d pipe_read_end %d pipe_write_end %d\n", getpid(), left_pipe_fd[0], left_pipe_fd[1]);
    int pid = fork();
    if (pid > 0) {
        close(left_pipe_fd[0]);
        int p;
        for (int i = 2; i <= 35; ++i) {
            p = i;
            if (write(left_pipe_fd[1], &p, INTLEN) != INTLEN) {
                fprintf(STDERR, "pid %d write pipe %d error\n", getpid(), left_pipe_fd[1]);
                exit();
            }
        }
        close(left_pipe_fd[1]);
    } else if (pid == 0) {
        // child process
        close(STDIN);
        dup(left_pipe_fd[0]);
        close(left_pipe_fd[0]);
        close(left_pipe_fd[1]);
        generate_process();
        close(STDIN);
    } else if (pid < 0) {
        // fork error
        fprintf(STDERR, "pid %d fork error\n", getpid());
    }
    wait();
    exit();
}

static void generate_process(void) {
    /**
     * read from STDIN, write to STDOUT
     */
    int p;
    int state;
    state = read(STDIN, &p, INTLEN);

    if (state != INTLEN) {
        close(STDIN);
        if (state != EOF)
            fprintf(STDERR, "pid %d read error\n", getpid());
        exit();
    }
    printf("prime %d\n", p);

    int right_pipe_fd[2];
    pipe(right_pipe_fd);
    
    int pid = fork();
    if (pid > 0) {
        close(right_pipe_fd[0]);
        int n;
        while ((state = read(STDIN, &n, INTLEN)) == INTLEN) {
            if (n % p) {
                if (write(right_pipe_fd[1], &n, INTLEN) != INTLEN) {
                    fprintf(2, "pid %d write pipe %d error\n", getpid(), right_pipe_fd[1]);
                    exit();
                }
            }
        }

        if (state != EOF) {
            fprintf(STDERR, "pid %d read pipe %d error\n", getpid(), STDIN);
            exit();
        }
        close(STDIN);
        close(right_pipe_fd[1]);
    } else if (pid == 0) {
        // child process
        close(STDIN);
        dup(right_pipe_fd[0]);
        close(right_pipe_fd[0]);
        close(right_pipe_fd[1]);
        generate_process();
        close(STDIN);
    } else if (pid < 0) {
        fprintf(2, "pid %d fork error\n", getpid());
        exit();
    }
}