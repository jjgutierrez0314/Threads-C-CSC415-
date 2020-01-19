/***********************

  File: fileStats.c
  Compile: gcc fileStats.c -o fileStats
  Run: ./fileStats [file1] [file2] ...

  Description: Similar to Unix utility wc
               [file1], [file2] etc are text files. fileStats counts the
               number of lines, words and characters in each file, reports
               the counts for each file, and total counts over all the files.

***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>


#define MAXCHARS 80

struct FileInfo {
  char *name;    /* name of file */
  int numLines;  /* number of lines in file */
  int numWords;  /* number of words in file */
  int numChars;  /* number of characters in file */
} fileInfo;

struct Parameters {
  char **name;
  int count;
} parameters;

int countWords(char *);
void * printResult (void *);
int i = 0;
struct FileInfo *info; /* array of counts for each file */
struct Parameters * twoArgs;

int main(int argc, char **argv) {
  int numLines = 0, numWords = 0, numChars = 0; /* total counts */
  //char inString[MAXCHARS];
  //char *rs;

  /* allocate array of structs containing counts for each file */
  info = (struct FileInfo *) malloc((argc-1) * sizeof(struct FileInfo));

  //threads
  int iret[argc];
  pthread_t thread[argc];
  struct Parameters *twoArgs;
  twoArgs = (struct Parameters *) malloc((argc-1) * sizeof(struct Parameters));
  (*twoArgs).name = argv;
  (*twoArgs).count = argc;
  //printf("%d", argc);

  //create
  for(int i = 0; i < argc-1; i++) {
    if ((iret[i] = pthread_create(&thread[i], NULL, printResult, (void *) twoArgs))) {
      printf("Error creating thread %d\n", i);
      exit(0);
      }
  }

  //join when a thread is done
  for(int i=0; i < argc-1; i++) {
    if ((iret[i] = pthread_join( thread[i], NULL))) {
      printf("Error terminating thread %d\n", i);
      exit(0);
      }
  }


  for (int i=0; i<argc-1; i++) {
    numLines += info[i].numLines;
    numWords += info[i].numWords;
    numChars += info[i].numChars;
  }

  printf("Total: %d lines, %d words, %d characters\n",
	 numLines, numWords, numChars);

}

/***********************

  int countWords(char *inS)

  inS: input null-terminated string
  
  returns number of words in string, delimited by spaces

***********************/

int countWords(char *inS) {
  char *token;
  int numTokens = 0;
  int i=0;

  for (i=1; i<strlen(inS); i++) {
    if ((isalnum(inS[i-1]) || ispunct(inS[i-1])) && (inS[i] == ' ')) {
      numTokens++;
    }
  }
      
  if (isalnum(inS[strlen(inS)-2]) || ispunct(inS[strlen(inS)-2])) {
    numTokens++;
  }
  return numTokens;
}

void * printResult (void * arg) {
  struct Parameters *passedPair = (struct Parameters *) arg;
  int numOfFiles = (*passedPair).count;
  char ** nameOfFiles = (*passedPair).name;
  char inString[MAXCHARS];
  char *rs;
  FILE *fp;

    for (int i=0; i<numOfFiles-1; i++) {
    /* open an input file, initialize struct of counts */
      fp = fopen(nameOfFiles[i+1], "r");
      if (fp == NULL) {
        printf("Error: cannot open file\n");
        return 0;
      }

    info[i].name = (char *) malloc(MAXCHARS * sizeof(char));
    strncpy(info[i].name, nameOfFiles[i+1], MAXCHARS);
    info[i].numLines = 0;
    info[i].numWords = 0;
    info[i].numChars = 0;

    /* read each line, update counts */
    rs = fgets(inString, MAXCHARS, fp);
    
    while (rs != NULL) {
      info[i].numLines++;
      info[i].numChars += strlen(inString);
      info[i].numWords += countWords(inString);
      rs = fgets(inString, MAXCHARS, fp);
    }
  }
    printf("Thread Id is: %ld %s: %d lines, %d words, %d characters\n", pthread_self(),info[i].name,
	   info[i].numLines, info[i].numWords, info[i].numChars);
}
