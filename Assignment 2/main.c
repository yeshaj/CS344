#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PREFIX "movies_"
#define ENDING ".csv"


/*
* Use struct to create data type to group items into single type
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/studentsc#main.c
* struc for movie information
*/
struct movies
{
  char *title;
  char *year;
  char *languages;
  char *ratingValue;
  struct movies *next;
};

// this will find out how many movies there are on the file
int movieCount=0;

int initMovies(struct movies *list)
{
  int numMovies = 0;
  while (list != NULL)
  {
    numMovies++;
    list = list->next;
  }
// will be 1 less than than the number of lines in the file
// because the first line has the column headers
  return (numMovies - 1);
}


// listing the two different choices

void listChoicesMain() {
	char choiceOne[] = "1.";
	char choiceTwo[] = "2.";

	printf("%s %s \n", choiceOne, "Select file to process");
	printf("%s %s \n", choiceTwo, "Exit the program \n");
	printf("%s", "Enter a choice 1 or 2: ");
}

/*
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/studentsc#main.c
* Parse the current line which is space delimited and create a
* movies struct with the data in this line
*/
struct movies *createMovie(char *currLine)
  {
    struct movies *currMovie = malloc(sizeof(struct movies));
    // For use with strtok_r
    char *saveptr;

    // Use this token for the title of the movie
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // Use this token for the year of the movie
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

    // Use this token for the languages of the movie
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // Use this token for the rating value of the movie
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->ratingValue = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->ratingValue, token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
  }

  /*
  * Adapted from <CS 344> <main.c> https://repl.it/@cs344/studentsc#main.c
  * Return a linked list of movies by parsing data from
  * each line of the specified file.
  */
  struct movies *processFile(char *filePath)
  {
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movies *head = NULL;
    // The tail of the linked list
    struct movies *tail = NULL;

      // Read the file line by line
      while ((nread = getline(&currLine, &len, movieFile)) != -1)
      {
        // Get a new movie node corresponding to the current line
        struct movies *newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
          // This is the first node in the linked link
          // Set the head and the tail to this node
          head = newNode;
          tail = newNode;
          }
          else
          {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
          }
      }
      free(currLine);
      fclose(movieFile);
      return head;
  }


// process the file and make sure there's only one entry per year
void processFile(char *filename)
{
  struct movie *list = parseData(filename);

	int movieSize = 0;
	int maxStringSize = 50;
	int listYears[movieCount];
	char titleList[movieCount][maxStringSize];
	movieSize = movieCount;
	int newYearsList[movieCount];
	int p;


	while(list != NULL)
	{
    for(p=0; p<movieCount; p++)
    {
      listYears[p] = list->year;
			newYearsList[p] = list->year;
			strcpy(titleList[p], list->title);
			list = list->next;
		}
	}

	// loop var
	int i;
	int j;
	int k;

	// make sure there are only one entry per year
	for (i = 0; i<movieSize; i++)
  {
    for(j=i+1; j<movieSize; j++)
    {
			if(listYears[i] == listYears[j])
      {
				 for(k=j; k < movieSize; k++)
         {
           listYears[k] = listYears[k + 1];
          }
          movieSize--;
          j--;
			}
		}
	}

  char directoryName[50];
  char directoryName2[50];
  int randNum;

	randNum = generateRandNum();
	sprintf(directoryName, "pettiboa.movies.%d", randNum);
	strcpy(directoryName2, directoryName);

	int status;
  // if not possible to make directory create error message
	status = mkdir(directoryName, 0750);
	if(status != 0)
  {
    printf("There was a problem making directoriies to: %s\n", directoryName);
		return;
	}

	int g;
	int kr;
	char line2[] = "/";
	strcat(directoryName2, line2);
	char *str = malloc(10);
  char *string = malloc(10);
  char s1[] = ".txt";
  char *newline = malloc(20);
  char fullDirPath[100];

	for(g=0; g<movieSize; g++)
  {
    sprintf(str,"%d", listYears[g]);
		strcat(str, s1);

		sprintf(fullDirPath, "%s%s", directoryName2, str);

    // recieve the permission for the directory and open up the file
		FILE* fptr = fopen(fullDirPath, "w+");
    if(chmod(fullDirPath, 0640) == -1)
    {
    	printf("There was an error with changing the permissions");
		}
    int looper = 0;
		int hold = 0;
		while(looper < movieSize)
    {
			for(kr = 0; kr<movieCount; kr++)
      {
        if(newYearsList[kr] == listYears[hold])
        {
          // prints titles in the fptr files
  				fprintf(fptr, "%s\n", titleList[kr]);
  				looper++;
        }
			}
				hold++;
		}
			if(fptr == NULL)
      {
        printf("Error creating file\n");
        exit(-1);
			}
      fclose(fptr);

  }
	printf("Created directory with name %s", directoryName);
}


// Process the largest file after finding it
void findLargestFile()
{

	DIR* currDir = opendir(".");
    struct dirent *aDir;
    off_t st_size;
    struct stat dirStat;
    char entryName[256];
    int size =0;

  while((aDir = readdir(currDir)) != NULL)
  {
    char *point = strstr(aDir->d_name, ENDING);

  	// prefix is "movies_" and also contains ".csv
    if((strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) && (point != NULL))
    {

      stat(aDir->d_name, &dirStat);

      if(dirStat.st_size >= size)
      {
        size = dirStat.st_size;
      	strcpy(entryName, aDir->d_name);
      }
    }
  }

// close the current directory
	closedir(currDir);
	printf("Now processing the chosen file named %s \n", entryName);
	processFile(entryName);
}


// Process the smallest file after finding it

void findSmallestFile()
{

	DIR* currDir = opendir(".");
    struct dirent *aDir;
    off_t st_size;
    struct stat dirStat;
    char entryName[256];
    int size=800000;

    while((aDir = readdir(currDir)) != NULL)
    {
      char *point1 = strstr(aDir->d_name, ENDING);

      if((strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) && (point1 != NULL))
      {

        stat(aDir->d_name, &dirStat);

        if(dirStat.st_size < size)
        {
         	size = dirStat.st_size;
        	strcpy(entryName, aDir->d_name);
        }
  	 }
  }
	// close the current directory
	closedir(currDir);
	printf("Now processing the chosen file named for %s\n", entryName);
	processFile(entryName);
}


// get the number entered or send error message
void findFile()
{
  FILE *userFile;
	char userChoice[70];

	printf("Enter the complete file name: ");

	scanf("%s", userChoice);

  if(userFile = fopen(userChoice, "r"))
  {
    	fclose(userFile);

    	if (parseData(userChoice) != 0)
      {
      	printf("Now processing the chosen file named %s \n", userChoice);
      	// process the file given by the user
      	processFile(userChoice);
      }

  }
  else
  {
    printf("%s was not found. Try again \n\n", userChoice);

	int userChoice;

	// print the different choices
	printf("Which file you want to process? \n");
	printf("Enter 1 to pick the largest file \n");
	printf("Enter 2 to pick the smallest file \n");
	printf("Enter 3 to specify the name of a file \n\n");
	printf("Enter a choice from 1 to 3: ");

	// get the number entered or send error message

	if(scanf("%d", &userChoice) == 1)
  {
		if(userChoice > 3)
    {
      printf("That is not an integer between 1-3, please Enter a choice from 1 to 3: \n");
    }
  }
  else
  {
    printf("You did not enter an integer, please Enter a choice from 1 to 3: \n");
  }

  // largest file found and processed
  if(userChoice == 1)
  {
    findLargestFile();
    printf("\n");
	}

  // smallest file found and processed
	else if(userChoice == 2)
  {
		findSmallestFile();
		printf("\n");
	}

  // process file given by user
	else if(userChoice == 3)
  {
		findFile();
		printf("\n");
	}

  else
  {
		exit(0);
	}
  }
}

// random number is generated
int generateRandNum()
{
  time_t t;
	srand((unsigned)time(&t));

	int randomNum = rand() % 99999;

	return randomNum;
}


// list the numbers and have them choose or send error message
void listChoicesTwo()
{

	int userChoice;

	// prints the choices
	printf("Which file you want to process? \n");
	printf("Enter 1 to pick the largest file \n");
	printf("Enter 2 to pick the smallest file \n");
	printf("Enter 3 to specify the name of a file \n\n");
	printf("Enter a choice from 1 to 3: ");

	// get the number entered or send error message
	if(scanf("%d", &userChoice) == 1)
  {
		if(userChoice > 3)
    {
      printf("That is not an integer between 1-3, please Enter a choice from 1 to 3: \n");
    }
  }
  else
  {
    printf("You did not enter an integer, please Enter a choice from 1 to 3: \n");
  }

  // largest file found
  if(userChoice == 1)
  {
  	findLargestFile();
  	printf("\n");
	}

  // smallest file found
	else if(userChoice == 2)
  {
		findSmallestFile();
		printf("\n");
	}

  // process file given by user
	else if(userChoice == 3)
  {
		findFile();
		printf("\n");
	}

	else
  {
		exit(0);
	}
}

// To list the first 2 choices with which users have the option to selct a file to process or exit

void optionActionsMain()
{
	int userChoice;
	while(userChoice != 2)
	{
		listChoicesMain();
		// get the number entered or send error message
		if(scanf("%d", &userChoice) == 1)
    {
			if(userChoice > 2)
      {
        printf("That is not an integer 1 or 2, please Enter a choice 1 or 2 \n");
      }
    }
		else
    {
      printf("You did not enter an integer, please Enter a choice from 1 to 4: ");
    }

    // if user picks 1st option
    if(userChoice == 1)
    {
    	printf("\n");
    	listChoicesTwo();
    	printf("\n");

  	}
	else if(userChoice == 2)
  {
		exit(0);
		printf("\n");
	}
}
}


int main(void)
{
	optionActionsMain();

    printf("\n");

    return EXIT_SUCCESS;
}
