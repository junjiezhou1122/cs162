#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit lim;

    // Get and print stack size limit
    if (getrlimit(RLIMIT_STACK, &lim) == 0) {
        printf("stack size: %ld\n", lim.rlim_cur);
    } else {
        perror("getrlimit (RLIMIT_STACK)");
    }

    // Get and print process limit
    if (getrlimit(RLIMIT_NPROC, &lim) == 0) {
        printf("process limit: %ld\n", lim.rlim_cur);
    } else {
        perror("getrlimit (RLIMIT_NPROC)");
    }

    // Get and print max file descriptors
    if (getrlimit(RLIMIT_NOFILE, &lim) == 0) {
        printf("max file descriptors: %ld\n", lim.rlim_cur);
    } else {
        perror("getrlimit (RLIMIT_NOFILE)");
    }

    return 0;
}