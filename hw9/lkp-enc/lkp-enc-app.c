#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define RKEY_IOCTL 0x80047101 
#define WKEY_IOCTL 0x40047101 

#define RCH_IOCTL  0x80047102
#define WCH_IOCTL  0x40047102 

#define MAX_LKP_ENC_BUFFER_SIZE 128

int main() {
    unsigned int usr_encryption_key = 0x0;
    char usr_str_data[MAX_LKP_ENC_BUFFER_SIZE] = "Hello World";

    int fd = open("/dev/lkp_enc_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return -1;
    }

    printf("Initial Encryption key: %u\n", usr_encryption_key);
    printf("Initial String data: %s\n", usr_str_data);
    
    // read the device encryption key, should be zero
    if(ioctl(fd, RKEY_IOCTL, &usr_encryption_key)) {
        perror("ioctl read key");
        return -1;
    }

    printf("Encryption key from driver: %u\n", usr_encryption_key);

    printf("Setting encryption key to 2\n");
    usr_encryption_key++;
    usr_encryption_key++;

    printf("writing encryption key to device");
    // write the encryption key
    if(ioctl(fd, WKEY_IOCTL, &usr_encryption_key)) {
        perror("ioctl write key");
        return -1;
    }

    printf("writing string to device");
    // write string to device
    if(ioctl(fd, WCH_IOCTL, &usr_str_data)) {
        perror("ioctl write string");
        return -1;
    }

    printf("reading string from deivce");
    // read string from device
    if(ioctl(fd, RCH_IOCTL, &usr_str_data)) {
        perror("ioctl read string");
        return -1;
    }

    printf("String data from driver: %s\n", usr_str_data);

    close(fd);
    return 0;
}
