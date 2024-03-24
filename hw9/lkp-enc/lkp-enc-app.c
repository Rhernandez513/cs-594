#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define RAND_IOCTL	0x80047101
#define SEED_IOCTL	0x40047101

#define MAX_LKP_ENC_BUFFER_SIZE 128

int main() {
    int fd = open("/dev/my_rng_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return -1;
    }

    unsigned int seed = 0x0;
    unsigned int random_number = 0;

    for(int i=0; i<2; i++) {

        // seed the generator
        if(ioctl(fd, SEED_IOCTL, &seed)) {
            perror("ioctl seed");
            return -1;
        }

        // get 5 random numbers
        for (int j=0; j<5; j++) {
            if(ioctl(fd, RAND_IOCTL, &random_number)) {
                perror("ioctl rand");
                return -1;
            }

            printf("Round %d number %d: %u\n", i, j, random_number);
        }
    }

    close(fd);
    return 0;
}
