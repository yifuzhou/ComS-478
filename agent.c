#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int read_one_byte(int client_socket, char *buffer);
int receiveFully(int client_socket, char *buffer, int length);
void printBinaryArray(char *buffer, int length);
int toInteger32(char *bytes);
void convertUpperCase(char *buffer, int length);

int main()
{
    // get a socket descriptor
   	int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   	printf("server_socket = %d\n", server_socket);
   
    // bind to a port
 	struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_len = sizeof(sin);  // comment this line out if running on pyrite (linux) 
    sin.sin_family = AF_INET; // or AF_INET6 (address family)
    sin.sin_port = htons(1234);
    sin.sin_addr.s_addr= INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&sin, sizeof(sin)) < 0) 
    {
        // Handle the error.
		printf("bind error\n");
    }
    
    listen(server_socket, 5); /* maximum 5 connections will be queued */
	int counter = 0;
	while (1)
	{
		struct sockaddr client_addr;
		unsigned int client_len;
		
		printf("accepting ....\n");
  		int client_socket = accept(server_socket, &client_addr, &client_len);
  		printf("request %d comes ...\n", counter++);
		/* launch a new thread to take care of this client connection */
		/* client_addr contains the address of the connecting client */
		/* client_len is the buffer length that is valid in client_addr */
		/* both client_addr and client_len are optional */
		
		// processing this request

		// get the command length
		char packet_length_bytes[4];
		receiveFully(client_socket, packet_length_bytes, 4);
			
		printBinaryArray(packet_length_bytes, 4);
		
		int packet_length = toInteger32(packet_length_bytes);
		printf("packet_length_bytes = %d", packet_length);
		
		// allocate buffer to receive the data
		char *buffer = (char *)malloc(packet_length);
		receiveFully(client_socket, buffer, packet_length);
		
		// convert to upper case 
	    convertUpperCase(buffer, packet_length);
	    
	    // send back
	    send(client_socket, packet_length_bytes, 4, 0); // 4 bytes first
	    send(client_socket, buffer, packet_length, 0);
	    
	    // release buffer
	    free(buffer);
	}
}

void convertUpperCase(char *buffer, int length)
{
	int i = 0;
	while (i < length)
	{
		if (buffer[i] >= 'a' && buffer[i] <= 'z')
		{
			buffer[i] = buffer[i] - 'a' + 'A'; 
		}
		i++;
	}
}

int receive_one_byte(int client_socket, char *cur_char)
{
    ssize_t bytes_received = 0;
	while (bytes_received != 1)
	{
		bytes_received = recv(client_socket, cur_char, 1, 0);
	} 
	
	return 1;
}

int receiveFully(int client_socket, char *buffer, int length)
{
	char *cur_char = buffer;
	ssize_t bytes_received = 0;
	while (bytes_received != length)
	{
	    receive_one_byte(client_socket, cur_char);
	    cur_char++;
	    bytes_received++;
	}
	
	return 1;
}

void printBinaryArray(char *buffer, int length)
{
    int i=0;
    while (i<length)
    {
		printf("%d ", buffer[i]);
		i++;
	}
	
	printf("\n");
}

int toInteger32(char *bytes)
{
	int tmp = (bytes[0] << 24) + 
	          (bytes[1] << 16) + 
	          (bytes[2] << 8) + 
	          bytes[3];
   
	return tmp;
}

/* On an OS with a different endian from java, use this one
int toInteger32(char *bytes)
{
	int tmp = bytes[0] + 
	          (bytes[1] << 8) + 
	          (bytes[2] << 16) + 
	          (bytes[3] << 24);
	             
	return tmp;
}
*/
