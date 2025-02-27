/*	AG0907 Lab 2 TCP client example - by Henry Fortuna and Adam Sampson

	A simple client that connects to a server and waits for
	a response. The server sends "hello" when the client first
	connects. Text typed is then sent to the server which echos
	it back, and the response is printed out.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")


// The IP address of the server to connect to
#define SERVERIP "127.0.0.1"

// The TCP port number on the server to connect to
#define SERVERPORT 5555

// The message the server will send when the client connects
#define WELCOME "hello"

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40			// in bytes


// Prototypes
void die(const char *message);


int main()
{
	printf("Client Program\n");

	// Initialise the WinSock library -- we want version 2.2.
	WSADATA w;
	int error = WSAStartup(0x0202, &w);
	if (error != 0)
	{
		die("WSAStartup failed");
	}
	if (w.wVersion != 0x0202)
	{
		die("Wrong WinSock version");
	}

	// Create a TCP socket that we'll connect to the server
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	// FIXED: check for errors from socket

	if (sock == INVALID_SOCKET)
	{
		die("Invalid socket");
	}
	// Fill out a sockaddr_in structure with the address that
	// we want to connect to.
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	// htons converts the port number to network byte order (big-endian).
	addr.sin_port = htons(SERVERPORT);
	addr.sin_addr.s_addr = inet_addr(SERVERIP);
	

	// inet_ntoa formats an IP address as a string.
	printf("IP address to connect to: %s\n", inet_ntoa(addr.sin_addr));
	// ntohs does the opposite of htons.
	printf("Port number to connect to: %d\n\n", ntohs(addr.sin_port));

	// Connect the socket to the server.
	if (connect(sock, (const sockaddr *) &addr, sizeof addr) == SOCKET_ERROR)
	{
		die("connect failed");
	}

	printf("Connected to server\n");

	// We'll use this buffer to hold what we receive from the server.
	char buffer[MESSAGESIZE];

	// We expect the server to send us a welcome message (WELCOME) when we connect.

	// Receive a message.
	// FIXED: check for errors, or for unexpected message size
	if (recv(sock, buffer, MESSAGESIZE, 0) == SOCKET_ERROR)
	{
		die("Receive failed");
	}

	// Check it's what we expected.
	if (memcmp(buffer, WELCOME, strlen(WELCOME)) != 0)
	{
		die("Expected \"" WELCOME "\" upon connection, but got something else");
	}

	while (true)
	{
		printf("Type some text (\"quit\" to exit): ");
		fflush(stdout);

		// Read a line of text from the user.
		std::string line;
		std::getline(std::cin, line);
		// Now "line" contains what the user typed (without the trailing \n).
		const void *thisMessage = line.c_str();
		size_t numline = line.size();
		// Copy the line into the buffer, filling the rest with dashes.

		//using a for loop to split up the string in subtrings of size MESSAGESIZE
		for (int i = 0; i <= numline; i += MESSAGESIZE)
			{
				std::string substring = line.substr(i, MESSAGESIZE);
				memset(buffer, '-', MESSAGESIZE);
				memcpy(buffer, substring.c_str(), substring.size());
				// FIXED: if line.size() is bigger than the buffer it'll overflow (and likely corrupt memory)			
				// Send the message to the server.

				// FIXED: check for error from send
				if (send(sock, buffer, MESSAGESIZE, 0) == SOCKET_ERROR)
				{
					die("Can't send to server");
				}
				// Read a response back from the server.
				int count = recv(sock, buffer, MESSAGESIZE, 0);
				// FIXED: check for error from recv
				if (count == SOCKET_ERROR)
				{
					die("Can't receive response from server");
				}

				if (count <= 0)
				{
					printf("Server closed connection\n");
					break;
				}

				printf("Received %d bytes from the server: '", count);
				fwrite(buffer, 1, count, stdout);
				printf("'\n");
			}
	}

	printf("Quitting\n");

	// Close the socket and clean up the sockets library.
	closesocket(sock);
	WSACleanup();

	return 0;
}


// Print an error message and exit.
void die(const char *message) {
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}

////////////////// LAB EXCERCISES //////////////////

//Differences between server and client: client doesn't have a nested while loop. Server needs it to firstly listen at connections 
//and secondly to receive messages
//client doesn't have a listen or bind method

//Removing htons and ntohs:
// both of them > connection error
// htons > connection error
// ntohs > no connection error, but port printed is different: should be 5555 but it prints 45845

//Running client without server triggers error in the if statement when connect is called:
//if (connect(sock, (const sockaddr *)&addr, sizeof addr) == SOCKET_ERROR)
//
	//die("connect failed");
//}

//closing the server with client still running:
//there is no error message set for receive response back from server,
//the client waits for input and, when given, it just closes down, calling break in if statement below:
//int count = recv(sock, buffer, MESSAGESIZE, 0);
//if (count <= 0)
//{
	//printf("Server closed connection\n");
	//break;
//}

//Closing client while server is =till running doesn't trigger any error, 
//as the connection just closes and server waits for new connection,
//which is possible as, opening the client again, starts a new connection on a different port
//P.S. After fixing error handling, an error is triggered when client is not properly closed.

//Removing bind from server:
//listen fails and application crashes
//removing listen function from server:
//server binds correctly, but error occurs when waiting for connection

//Fixed all  code bits

//Long messages: they are sent to server up to 40th character as the buffer is completely full.
//Error handling implementation avoids this, but it stops the application.
//Fixed: message is just split in chunks of size of MESSAGESIZE

//Implemented small for loop to invert case letter of buffer message,
//therefore server sends message back to client with reverse case letters
//so that lowercase char in array are converted to uppercase and vice versa