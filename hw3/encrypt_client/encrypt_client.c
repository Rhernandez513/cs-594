#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *str = NULL;
    int key = 0;
    int opt;

    while ((opt = getopt(argc, argv, "s:k:")) != -1) {
        switch (opt) {
            case 's':
                str = optarg;
                break;
            case 'k':
                key = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -s string -k key\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (str == NULL || key == 0) {
        fprintf(stderr, "Both -s and -k options are required.\n");
        exit(EXIT_FAILURE);
    }

    int SYSCALL_NUM = 451;
    int result = syscall(SYSCALL_NUM, str, key);

    printf("Result of sys_s2_encrypt: %d\n", result);

    return 0;
}
