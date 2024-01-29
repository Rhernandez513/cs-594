#include <stdio.h>

void addOneToEachCharacter(char *str) {
    // Iterate through each character in the string
    for (int i = 0; str[i] != '\0'; ++i) {
        // Add 1 to the ASCII value of the character
        str[i] = str[i] + 1;
    }
}

int main() {
    // Example usage
    char myString[] = "hello";

    // Print the original string
    printf("Original string: %s\n", myString);

    // Call the function to add 1 to each character
    addOneToEachCharacter(myString);

    // Print the modified string
    printf("Modified string: %s\n", myString);

    return 0;
}
