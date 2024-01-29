int sys_s2_encrypt(char *str, int key) {
    if (key < 1 || key > 5) {
        return -EINVAL;
    }

    // Iterate through each character in the string
    for (int i = 0; str[i] != '\0'; ++i) {
        // Add encryption key to the ASCII value of the character
        str[i] = str[i] + key;
    }

    // print the encrypted string
    printk(KERN_INFO "Message: %s", " In sys_s2_encript: - %s\0", str);

    return 0;
}
