#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 80000

// Error function used for reporting issues
void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int connectionSocket, newConnectionSocket, charsRead, portNumber;
  char buffer[MAXSIZE];
  struct sockaddr_in serverAddress, clientAddress;


//  size_t NumberOfElements = sizeof(buffer)/sizeof(buffer[0]);
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  char plaintext[MAXSIZE];

  char key[MAXSIZE];

  char decText[MAXSIZE];
  char tempDecr[MAXSIZE];
	char tempBuffer[MAXSIZE];

	int keySize;
	int msgSize;
	int decryption;

	int status;
	pid_t pid;

  // Check usage & args
  if (argc < 2)
	{
    fprintf(stderr,"USAGE: %s port\n", argv[0]);
    exit(1);
  }


   // Set up the address struct for the server socket
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  // The address should be network capable
  serverAddress.sin_family = AF_INET;
  // Store the port number
  portNumber = atoi(argv[1]);
  serverAddress.sin_port = htons(portNumber);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0)
	{
    error("ERROR opening socket");
  }


  // Associate the socket to the port
  if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5);

  // Accept a connection, blocking if one is not available until one connects
  while(1)
	{
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
    if (connectionSocket < 0)
		{
      error("ERROR on accept");
    }

		pid = fork();

    switch (pid)
		{
			//if error in fork
			case -1:
			{
				error("ERROR on fork");
			}
			// child process
			case 0:
			{
				memset(buffer, '\0', MAXSIZE);
    		// Read the client's message from the socket, this should be the msg size
		    charsRead = recv(connectionSocket, buffer, MAXSIZE, 0);
		    if (charsRead < 0)
				{
		      error("ERROR reading from socket");
		    }
		    // prints first message from client
		    //printf("SERVER: I received this from the client: \"%s\"\n", buffer);

				msgSize = atoi(buffer);

				// send success msg 1 to client
		    charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
		     if (charsRead < 0)
				 {
					 error("ERROR writing to socket");
				 }

				 // Get the message from the client and display it
		    memset(tempBuffer, '\0', MAXSIZE);
		    memset(plaintext, '\0', MAXSIZE);
		    // Read the client's message from the socket
		    charsRead = recv(connectionSocket, tempBuffer, MAXSIZE, 0);
		    if (charsRead < 0)
				{
		    	error("ERROR reading from socket");
    		}

				// prints first message from client
    		//printf("SERVER: I received this from the client: \"%s\"\n", buffer);

				strcat(plaintext, tempBuffer);

				//	printf("SERVER: I received this from the client: \"%s\"\n", plaintext);

				//sends success msg 2
	 			charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
     		if (charsRead < 0)
				{
					error("ERROR writing to socket");
    		}


	// print number of elements
//	printf("%d", NumberOfElements);
    // Send a Success message back to the client
//    charsRead = send(connectionSocket,
//                    "I am the server, and I got your message", 39, 0);
//    if (charsRead < 0){
//      error("ERROR writing to socket");
//    }
//

     // Get the key from the client
    memset(buffer, '\0', MAXSIZE);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, MAXSIZE, 0);
    if (charsRead < 0)
		{
      error("ERROR reading from socket");
    }
    // prints first message from client
    //printf("SERVER: I received this from the client: \"%s\"\n", buffer);

//	strcat(key, buffer);
		keySize = atoi(buffer);
//	printf("SERVER: I received this from the client: \"%d\"\n", keySize);
    // Send a Success message back to the client
    charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
    if (charsRead < 0)
		{
      error("ERROR writing to socket");
    }

     // Get the key from the client and display it
    memset(buffer, '\0', MAXSIZE);
    // Read the key from the socket
    charsRead = recv(connectionSocket, buffer, MAXSIZE, 0);
    if (charsRead < 0)
		{
      error("ERROR reading from socket");
    }
    // prints first message from client
//    printf("SERVER: I received this from the client: \"%s\"\n", buffer);

		strcat(key, buffer);

//    	// check for bad chars or unequal sizes
		if(keySize < msgSize)
		{
			fprintf(stderr, "ERROR: key size small, can't encrypt");
			exit(1);
		}

	  int i;
	  char n;
	  int plaintextInt;
    int keyInt;
	  int result;


	// used for reference
	// https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language

	 // decrypt plaintext
	 //  converts chars in plaintext to ints 0-26, all uppercase letters and space char
	  for (i=0; i < msgSize-1; i++)
		{
			// if the char in plaintext is a space put 26 in int plainttext-start at 0
			if(plaintext[i] == ' ')
			{
				plaintextInt = 26;
			}
			else
			{
				// otherwise take char A-ASCII 65 from char in file to get num 0-26
				plaintextInt = plaintext[i] -'A';
			}
			// if the char in key is a space put 26 in int plainttext-start at 0
			if(key[i] == ' ')
			{
				keyInt = 26;
			}
			else
			{
				keyInt = key[i] - 'A';
			}
			// encrypt plaintext- subtract key from plaintext and add 27, then mod 27 to encrypt
			result = (plaintextInt - keyInt + 27) % 27;

			// if result is equal to 26, it's a space
			if(result == 26)
			{
				decText[i] = ' ';
			}
			else
			{
				// decrypted text
				// add char A to get final original uppercase letter
				decText[i] = 'A' + (char)result;
			}
		}

    // send decryption
    charsRead = send(connectionSocket, decText, strlen(decText), 0);

    memset(decText, '\0', MAXSIZE);

//                    "I am the server, and I got your message", 39, 0);
    if (charsRead < 0)
		{
      error("ERROR writing to socket");
    }
  	}

  	default:
		{
			pid_t parentPid = waitpid(pid, &status, WNOHANG);
		}
	//	close(connectionSocket);
		}
  	close(connectionSocket);
	}
   //close(connectionSocket);
  // Close the listening socket
  close(listenSocket);
  return 0;
}
