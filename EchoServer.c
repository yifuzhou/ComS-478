#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
		// Receiving a text string that is ended with "newline, 0x0A"
		// Don't know how long the string is, so receive it char by char
		// For each char, convert it to upcase and send it back back 
		char cur_char = ' ';
		while (cur_char != 0x0A)
		{
			ssize_t bytes_received = recv(client_socket, &cur_char, 1, 0);
			if (bytes_received == 0)
			{
				printf("bytes_received == 0\n");
			}
			printf("%c", cur_char);
			
			if (cur_char >= 'a' && cur_char <= 'z')
			{
				cur_char = cur_char - 'a' + 'A'; 
			}
			send(client_socket, &cur_char, 1, 0);
		}
	}

   return 0;
}
