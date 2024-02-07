#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5984
#define BUFF_SIZE 4096

int main(int argc, const char *argv[])
{
	int sock = 0;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[BUFF_SIZE] = {0};
	int ret = 0;

	/* [C1: point 1]
	 * Explain following in here.
	 *
	 * The following three lines are used to create a socket.
	 * AF_INET means IPv4 protocol.
	 * SOCK_STREAM means TCP protocol.
	 * 0 means the default protocol.
	 * The if condition is used to check if the socket is created 
	 * successfully
	 */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	/* [C2: point 1]
	 * Explain following in here.
	 *
	 * the first line is used to set the structure of the server 
	 * address of the server to all to all zeros
	 * the second line is used to set the family of the server 
	 * address we use IPv4 protocol, so we use AF_INET
	 * the third line is used to set the port of the server address
	 * we use port 5984
	 * we use htons (host to network short) to convert the port 
	 * number from host byte order to network byte order. This conversion 
	 * is necessary because different computer architectures may use 
	 * different byte orders, and network protocols typically use a 
	 * specific order (big-endian)
	 * /
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	/* [C3: point 1]
	 * Explain following in here.
	 *
	 * This line converts the human-readable IP address "127.0.0.1" to a 
	 * binary format and assigns it to serv_addr.sin_addr. The function 
	 * inet_pton (presentation to network) is used for this purpose. It 
	 * returns 1 on success, 0 if the input string is not a valid IP address,
	 * and -1 if an error occurs.
	 * 
	 */
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	/* [C4: point 1]
	 * Explain following in here.
	 */
	// The following line is used to connect to the server
	// inside the if condition, we check if the connection is
	// successful by checking the return value of the connect function
	// if the return value is less than 0, it means the connection is
	// not successful
	if (connect(sock, (struct sockaddr *)&serv_addr,
		    sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	printf("Press any key to continue...\n");
	getchar();

	/* [C5: point 1]
	 * Explain following in here.
	 */
	// The folllowing line is used to send a message to the server
	// the first parameter is the socket
	// the second parameter is the message
	// the third parameter is the size of the message
	// the fourth parameter is the flag which in this case
	// is set to 0 meaning no flags
	send(sock, hello, strlen(hello), 0);
	printf("Hello message sent\n");

	/* [C6: point 1]
	 * Explain following in here.
	 */
	// The following line is used to read the message from the server
	// the first parameter is the socket
	// the second parameter is the buffer to store the message
	// the third parameter is the size of the buffer
	ret = read(sock, buffer, 1024);
	printf("Read %d bytes of message from a server: %s\n", ret, buffer);

	return 0;
}
