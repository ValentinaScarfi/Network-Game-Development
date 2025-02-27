/*	AG0907 Lab 3 TCP client example - by Henry Fortuna and Adam Sampson

	A simple client that connects to a server and waits for
	a response. The server sends "hello" when the client first
	connects. Text typed is then sent to the server which echos
	it back, and the response is printed out.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
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

//defining fixed length of string containing size of message
#define FIXEDCOUNT 5


// Prototypes
void die(const char *message);


int main()
{

	char* bufferDynamic;
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
	if (sock == INVALID_SOCKET)
	{
		die("socket failed");
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
	//char buffer[MESSAGESIZE];

	while (true)
	{
		printf("Type some text (\"quit\" to exit): ");
		fflush(stdout);

		// Read a line of text from the user.
		std::string line;
		std::getline(std::cin, line);
		// Now "line" contains what the user typed (without the trailing \n).
		int messageSize = line.size();

		//converting messageSize to string of fixed length
		std::ostringstream os;
		os << std::setfill('0') << std::setw(FIXEDCOUNT) << messageSize;
		std::string sizeString = os.str();

		std::string completeMessage = sizeString + line;

		//getting new size
		int newBufferSize = completeMessage.size();

		//buffer is a char*[] to allow for dynamic size
		bufferDynamic = new char[newBufferSize + 1];

		// Copy the line into the buffer, filling the rest with dashes.
		// (We must be careful not to write past the end of the array.)
		//memset(bufferDynamic, '#', newBufferSize);
		memcpy(bufferDynamic, completeMessage.c_str(), newBufferSize);

		// Send the message to the server.
		if (send(sock, bufferDynamic, newBufferSize, 0) != newBufferSize)
		{
			die("send failed");
		}

		// Read a response back from the server.
		int count = recv(sock, bufferDynamic, newBufferSize, MSG_WAITALL);
		if (count <= 0)
		{
			printf("Server closed connection\n");
			break;
		}

		printf("Received %d bytes from the server: '", count);
		fwrite(bufferDynamic, 1, count, stdout);
		printf("'\n");
	}
	delete[] bufferDynamic;
	printf("Quitting\n");

	// Close the socket and clean up the sockets library.
	closesocket(sock);
	WSACleanup();

	return 0;
}


// Print an error message and exit.
void die(const char *message)
{
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}