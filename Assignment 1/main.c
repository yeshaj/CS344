#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

/*
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/studentsc#main.c
* Prints data for a single movie
*/
void printAMovie(struct movies* aMovie)
{
  printf("%s, %s, %s, %s\n", aMovie->title,
              aMovie->year,
              aMovie->languages,
              aMovie->ratingValue);
}


/*
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/studentsc#main.c
* Option 1 which shows the movies release in a specified year
* Print the linked list of movies for a given year
*/
void printMoviesYear(struct movies *list)
{
  bool manyMovies = false;
  int uInputYear = 0;

  printf("Enter the year for which you want to see movies: ");
  // will read the inputted year
  scanf("%d", &uInputYear);

  list = list->next;
  int yearInt = 0;
  // if the movies match when going through the linked, print it
  while (list != NULL)
  {
    // This will convert the string to an integer
    yearInt = atoi(list->year);

    //If they match, print that movie
    if (yearInt == uInputYear)
    {
      manyMovies = true;
      printAMovie(list);
    }
      list = list->next;
    }
    //if none of the moies match the inputted movies, print that no movies were released then
    if (!manyMovies)
    {
      printf("No movies were released in %d", uInputYear);
    }
}


/*
* For option 2
* Received help from https://www.geeksforgeeks.org/difference-between-malloc-and-calloc-with-examples/
* Received help from https://www.geeksforgeeks.org/strlen-function-in-c/
* Create space for structs.
*/
void newcalloc(struct movies *changestruct, struct movies copystruct)
{
  changestruct->title = calloc(strlen(copystruct.title) + 1, sizeof(char));
  strcpy(changestruct->title, copystruct.title);

  changestruct->year = calloc(strlen(copystruct.year) + 1, sizeof(char));
  strcpy(changestruct->year, copystruct.year);

  changestruct->languages = calloc(strlen(copystruct.languages) + 1, sizeof(char));
  strcpy(changestruct->languages, copystruct.languages);

  changestruct->ratingValue = calloc(strlen(copystruct.ratingValue) + 1, sizeof(char));
  strcpy(changestruct->ratingValue, copystruct.ratingValue);
}

// find the highest rating movie

void highRatedArr(int* differentNums, struct movies* list, struct movies* movies)
{
  // convert chars to doubles and see if the years are the same or not
  int i;
  for (i = 0; i < *differentNums; i++)
  {
    //if the years don't match
    if (atoi(movies[i].year) != atoi(list->year))
    {
      if (i == ((*differentNums) - 1))
      {
        newcalloc(&movies[(*differentNums)], *list);
        ((*differentNums)++);
      }
    }
        else
        {
            //compare to make sure the titles are the same
            if (strcmp(movies[i].title, list->title) != 0)
            {
                // convert to double if not the same
                double listArrRat = strtod(list->ratingValue, NULL);
                double mvNewArrRat = strtod((&movies[i])->ratingValue, NULL);
                if (listArrRat > mvNewArrRat)
                {
                  // Received examples on how to use from https://www.tutorialspoint.com/c_standard_library/c_function_free.htm
                  free((&movies[i])->year);
                  free((&movies[i])->ratingValue);
                  free((&movies[i])->title);
                  free((&movies[i])->languages);

                  newcalloc((&movies[i]), *list);
                }
                break;
            }
          }
  }
}


// Print the array of movies that hold the highest rated movie for each year
void highestRatingYr(int numMovies, struct movies *list)
{
  int otherYears = 0;

  //Skip to the second line of the list because of the header row
  list = list->next;

  //Received examples on how to use from https://www.geeksforgeeks.org/difference-between-malloc-and-calloc-with-examples/
  //A struct will have the high rated movies ready to print out
  struct movies *highRating = (struct movies*) malloc(numMovies * sizeof(struct movies));

  while (list != NULL)
  {
    // list out the highest rated movie from different years
    // it will change which is the highest if necessary
    highRatedArr(&otherYears, list, highRating);

    // the first year will be added before everything else
    if (otherYears == 0)
    {
      newcalloc(&highRating[0], *list);
      otherYears++;
    }
    //once again, the list will head to the next one on the list
    list = list->next;
  }

  printf("\nHere are the highest rated movies by year: \n");
  int i = 0;
  for (i = 0; i < otherYears; i++)
    {
      double doubleOperat = atof(highRating[i].ratingValue);
      printf("%.1f %s %s\n", doubleOperat, highRating[i].year, highRating[i].title);
    }
}

// Option 3 Show movies and their year of release for a specific language

void printLanguage (struct movies *list)
{
  char* langptr;
  char *userLanguage = calloc(21, sizeof(char)); // can assume that the maximum length of a language string is 20 characters.

    // Get the user input of languages they want to see, find languages in file and print
  printf("What language movies do you want to see?: ");
  scanf("%s", userLanguage);

  printf("Here are the movies in %s: \n", userLanguage);

    //Traverse through the linked list
  while (list != NULL)
  {
    // create variable for each movie that has the language the user wants
    char *oneMovieLang = calloc(strlen(list->languages) + 1, sizeof(char));

    strcpy(oneMovieLang, list->languages);

    char *langToken = strtok_r(oneMovieLang, ";[]", &langptr);

    while (langToken != NULL)
    {
      if (strcmp(langToken, userLanguage) == 0)
      {
        //If language is there, print that with the year and the title of movie
        printf("%s %s\n", list->year, list->title);
      }
        langToken = strtok_r(NULL, ";[]", &langptr);
    }
        list = list->next;
  }
}

// user will choose which prompt they want to pick from 1-4
int promptUser(struct movies *list, const int numMovies)
{
  int userChoice;

  //Get user's choice
  do
  {
    printf("1: See movies released in a specific year\n");
    printf("2: Show the highest rated movie for each year\n");
    printf("3: Show the title and year for all movies in a specified language\n");
    printf("4: Exit the program\n");
    printf("Please enter your choice 1-4: ");

    scanf("%d", &userChoice);

    //if they choose 0 or greater than 4 then error message
    if (userChoice == 0)
    {
      printf("You entered an invalid choice. Try again.\n");
    }
    else if (userChoice > 4)
    {
      printf("You entered an invalid choice. Try again.\n");
    }

  }

  while (userChoice < 1 || userChoice > 4);

  //User specifies year
  if (userChoice == 1)
  {
    printMoviesYear(list);
  }
  //Highest rated movie
  else if (userChoice == 2)
  {
    highestRatingYr(numMovies, list);
  }
  //User inputs year
  else if (userChoice == 3)
  {
    printLanguage(list);
  }
    //Quit if user picks 4
    return userChoice;
}

// Adapted from <CS 344> <main.c> https://repl.it/@cs344/studentsc#main.c
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("You must provide the name of the file to process\n");
    printf("Example usage: movie moviesample.csv\n");
    return EXIT_FAILURE;
  }

  struct movies *list = processFile(argv[1]);
  int numMovies = initMovies(list);
  printf("Processed file %s and parsed data for %d movies", argv[1], numMovies);

  int decision = 0;
  do
  {
      decision = promptUser(list, numMovies);
    }
    while (decision != 4);
    //User chose to quit
    return EXIT_SUCCESS;
}
