// Received a lot of help from Tutorialspoint.com and Repl.it

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LENGTH 2048

// to flag if background processes are allowed
int hasBackground = 1;

void getInput(char*[], int*, char[], char[], int);
void execUserCmd(char*[], int*, struct sigaction, int*, char[], char[]);
void handleSIGTSTP(int);
void printStatus(int);

/*
* will receive the input, and then check for any blanks/comments, and processes
* the 3 commands: exit, cd, and status.
*/
int main()
{
	int pid = getpid();
	int cont = 1;
	int i;
	int exitStatus = 0;
	int background = 0;
	// half of 512
	char inputFile[256] = "";
	char outputFile[256] = "";
	// maximum of 512 arguments
	char* input[512];
	for (i=0; i<512; i++)
	{
		input[i] = NULL;
	}

	// Signal Handling from Module 5 Exploration 3

	// CTRL-C command sends SIGINT signal so ignore CTRL-C command
	struct sigaction sa_sigint = {0};
	sa_sigint.sa_handler = SIG_IGN;
	sigfillset(&sa_sigint.sa_mask);
	sa_sigint.sa_flags = 0;
	sigaction(SIGINT, &sa_sigint, NULL);

	// CTRL-Z command redirects SIGTSTP signal to handleSIGTSTP()
	struct sigaction sa_sigtstp = {0};
	sa_sigtstp.sa_handler = handleSIGTSTP;
	sigfillset(&sa_sigtstp.sa_mask);
	sa_sigtstp.sa_flags = 0;
	sigaction(SIGTSTP, &sa_sigtstp, NULL);

	do
	{
		// Get input
		getInput(input, &background, inputFile, outputFile, pid);

		// Comments and Blank lines
		if (input[0][0] == '\0' || input[0][0] == '#')
		{
			continue;
		}

		// Changes directory using 'cd'
		else if (strcmp(input[0], "cd") == 0)
		{
			// change to the directory is not found
			if (input[1])
			{
				if (chdir(input[1]) == -1)
				{
					printf("Directory not found.\n");
					fflush(stdout);
				}
			}

			else
			{
				// If directory is not specified, go to HOME environment variable
				chdir(getenv("HOME"));
			}
		}

		// exit out of the shell using 'exit'
		else if (strcmp(input[0], "exit") == 0)
		{
			cont = 0;
		}

		// print out the exit status
		else if (strcmp(input[0], "status") == 0)
		{
			printStatus(exitStatus);
		}

		// print out the terminating signal
		else
		{
			execUserCmd(input, &exitStatus, sa_sigint, &background, inputFile, outputFile);
		}

		// variables reset to 0 or NULL
		for (i=0; input[i]; i++)
		{
			input[i] = NULL;
		}
		background = 0;
		inputFile[0] = '\0';
		outputFile[0] = '\0';

	}

	while (cont);

	return 0;
}

void handleSIGTSTP(int signo)
{
	// if the hasBackground is at 1
	if (hasBackground == 1)
	{
		// display the message (49 characters)
		char* message = "Entering foreground-only mode (& is now ignored)\n";
		write(1, message, 49);
		fflush(stdout);
		// and set hasBackground to 0
		hasBackground = 0;
	}

	// If the hasBackground is at 0
	else
	{
		// display the message (29 characters)
		char* message = "Exiting foreground-only mode\n";
		write (1, message, 29);
		fflush(stdout);
		// set the hasBackground to 1
		hasBackground = 1;
	}
}

// the input should become an array
void getInput(char* arr[], int* background, char inputName[], char outputName[], int pid)
{
	char input[MAX_LENGTH];
	int i, j;
	printf(": ");
	fflush(stdout);
	fgets(input, MAX_LENGTH, stdin);

	int found = 0;
	// remove line
	for (i=0; !found && i<MAX_LENGTH; i++)
	{
		if (input[i] == '\n')
		{
			input[i] = '\0';
			found = 1;
		}
	}

	// if the input is blank then return blank
	if (!strcmp(input, ""))
	{
		arr[0] = strdup("");
		return;
	}

	const char space[2] = " ";
	char *token = strtok(input, space);

	for (i=0; token; i++)
	{
		// if the command is to be executed in the background, last word must be &
		if (!strcmp(token, "&"))
		{
			*background = 1;
		}
		// if standard input or output is to be redirected
		else if (!strcmp(token, "<"))
		{
			token = strtok(NULL, space);
			strcpy(inputName, token);
		}
		else if (!strcmp(token, ">"))
		{
			token = strtok(NULL, space);
			strcpy(outputName, token);
		}
		else
		{
			arr[i] = strdup(token);
			// must expand any instance of "$$" in a command into the process ID
			for (j=0; arr[i][j]; j++)
			{
				if (arr[i][j] == '$' && arr[i][j+1] == '$')
				{
					arr[i][j] = '\0';
					snprintf(arr[i], 256, "%s%d", arr[i], pid);
				}
			}
		}
		token = strtok(NULL, space);
	}
}

// execute the command parsed into arr[]
void execUserCmd(char* arr[], int* childExitStatus, struct sigaction small_sigint, int* background, char inputName[], char outputName[])
{
	int input, output, result;
	pid_t childPid = -5;

	// Similar to Exploration 5.4 on Processes and I/O
	childPid = fork();
	switch (childPid)
	{
		case -1:;
			perror("Hull Breach!\n");
			exit(1);
			break;

		case 0:;
			small_sigint.sa_handler = SIG_DFL;
			sigaction(SIGINT, &small_sigint, NULL);

			// handles input
			if (strcmp(inputName, ""))
			{
				input = open(inputName, O_RDONLY);
				if (input == -1)
				{
					perror("Unable to open input file\n");
					exit(1);
				}
				result = dup2(input, 0);
				if (result == -1)
				{
					perror("Unable to assign input file\n");
					exit(2);
				}
				fcntl(input, F_SETFD, FD_CLOEXEC);
			}

			// handles output
			if (strcmp(outputName, ""))
			{
				output = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (output == -1)
				{
					perror("Unable to open output file\n");
					exit(1);
				}
				result = dup2(output, 1);

				if (result == -1)
				{
					perror("Unable to assign output file\n");
					exit(2);
				}
				fcntl(output, F_SETFD, FD_CLOEXEC);
			}

			if (execvp(arr[0], (char* const*)arr))
			{
				printf("%s: no such file or directory\n", arr[0]);
				fflush(stdout);
				exit(2);
			}

			break;

		default:;
			// iff hasBackground then execute
			if (*background && hasBackground)
			{
				pid_t actualPid = waitpid(childPid, childExitStatus, WNOHANG);
				printf("background pid is %d\n", childPid);
				fflush(stdout);
			}
			else
			{
				pid_t actualPid = waitpid(childPid, childExitStatus, 0);
			}

			while ((childPid = waitpid(-1, childExitStatus, WNOHANG)) > 0)
			{
				printf("child %d terminated\n", childPid);
				printStatus(*childExitStatus);
				fflush(stdout);
			}
	}
}

void printStatus(int childExitMethod)
{
	// this calls the exit status
	if (WIFEXITED(childExitMethod))
	{
		// print the exit value if exited by status
		printf("exit value %d\n", WEXITSTATUS(childExitMethod));
	}
	else
	{
		// print terminated by signal if done so
		printf("terminated by signal %d\n", WTERMSIG(childExitMethod));
	}
}
