#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 5984
#define BUFF_SIZE 4096

int main(int argc, const char *argv[])
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFF_SIZE] = {0};
	char *hello = "Hello from server";
	int ret = 0;

	/* [S1: point 1]
	 * Explain following in here.
	 *
	 * This code creates a socket using the socket system call. It checks 
	 * if the socket creation was successful, and if not, it prints an 
	 * error message and exits the program.
	 */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	/* [S2: point 1]
	 * Explain following in here.
	 *
	 * setsockopt is used to set options for the socket
	 * 
	 * sever_fd is the fild descriptor of the socket
	 * 
	 * SOL_SOCKET is The level at which the option 
	 * is defined, in this case, it's the socket level.
	 * 
	 * SO_REUSEADDR and SO_REUSEPORT are the options being set
	 * the first allows reuse of local addresses, the second allows
	 * multiple sockets on the same port
	 * 
	 * opt is a pointer to the value of the option
	 * 
	 * If we fail we print the error and exit 
	 */
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		       &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* [S3: point 1]
	 * Explain following in here.
	 * 
	 * First we set the address family to AF_INET, which is IPv4
	 * 
	 * It sets the IP address to INADDR_ANY. This means that the server will 
	 * bind to all available network interfaces on the machine and be 
	 * reachable through any of the machine's IP addresses.
	 * 
	 * finally we set the port to 5984 using htons to convert it to network
	 * byte order
	 */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	/* [S4: point 1]
	 * Explain following in here.
	 * 
	 * The next section binds the socket to a specific IP Address and port
	 * 
	 * (struct sockaddr *)&address: A pointer to the 
	 * struct sockaddr containing the address information. 
	 * The (struct sockaddr *)&address is used because bind expects 
	 * a pointer to a generic socket address structure (struct sockaddr), 
	 * and address is of type struct sockaddr_in. This typecasting is safe
	 * in this context because struct sockaddr_in starts with the same 
	 * fields as struct sockaddr.
	 */
	if (bind(server_fd, (struct sockaddr *)&address,
		 sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	/* [S5: point 1]
	 * Explain following in here.
	 * 
	 * The listen function puts the socket into a state where it is ready
	 * to accept incoming connections.  Here we set the backlog to 3, which
	 * is the maximum number of pending connections that can be queued up
	 * 
	 * If listen returns a negative number, it means that the call failed
	 * and we print an error message and exit
	 */
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* [S6: point 1]
	 * Explain following in here.
	 *
	 * The accept function is used to accept a connection on a listening socket
	 * It creates a new socket (new_socket) for the accepted connection.
	 * 
	 * The code then prints a message and waits for keyboard input
	 * which seems to be for interactive monitoring of the app 
	 */
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
				 (socklen_t*)&addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	printf("Press any key to continue...\n");
	getchar();

	/* [S7: point 1]
	 * Explain following in here.
	 *
	 * The read function is used to read data from a file descriptor
	 * in this case, from the new_socket. It reads up to 1024 
	 * bytes of data from the client and stores it in the buffer.
	 * 
	 * The number of bytes read is stored in the variable ret
	 * If ret is negative, it indicates an error in the read operation.
	 */
	ret = read(new_socket, buffer, 1024);
	printf("Read %d bytes of message from a client: %s\n", ret, buffer);

	/* [S8: point 1]
	 * Explain following in here.
	 * 
	 * The following code sends a message to the client using the send
	 * function. It sends the message "Hello from server" to the client
	 * and then prints a status message to the console
	 */
	send(new_socket, hello, strlen(hello), 0);
	printf("Hello message sent\n");

	return 0;
}
