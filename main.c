#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

const int B = 10;
const int R = 1;
const int S = 10;
const int N = 4;
const int W = 1;

char dictionary[10001][30];
int dictSize = 0;

char **buffer; // Array of strings
int buffer_index =
    0; // Index to track the next available position in the buffer
int buffer_max = 0;

char **misspelledWords;
int misspelledWordsIndex = 0;

char **correctWords;
int correctWordsIndex = 0;

void loadDictionary() {
  FILE *file = fopen("dictionary.txt", "r");
  if (file == NULL) {
    perror("Error opening dictionary file.");
    exit(EXIT_FAILURE);
  }

  while (fscanf(file, "%s", dictionary[dictSize]) != EOF)
    dictSize++;

  fclose(file);
}

void *readInput() {
  // 1. Reading Text
  // The program will read text from the standard input. However,
  // there is a rate limit for reading. We will assume that the reading rate
  // limit is R words per second. We will also assume that all words are
  // separated by white space(s). Please store words in the buffer.
  printf("Reading Text started.\n");

  char word[300];
  while (1) {
    // Read a word
    if (scanf("%s", word) != EOF) {
      // Inputting ":q" will terminate the loop.
      if (strcmp(word, ":q") == 0) {
        break;
      }

      // Store the word in the buffer
      buffer[buffer_index] =
          malloc(strlen(word) + 1); // +1 for the null terminator
      if (buffer[buffer_index] == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
      }
      strcpy(buffer[buffer_index], word);
      buffer_index++;

      // Sleep to simulate reading rate limit
      sleep(1 / R);
    } else {
      break;
    }
  }

  printf("Reading Text completed.\n");
  pthread_exit(NULL);
}

int isWordMisspelled(const char *word) {
  for (int i = 0; i < dictSize; i++) {
    if (strcmp(word, dictionary[i]) == 0)
      return 0;
  }

  return 1;
}

void *spellcheck() {
  while (buffer_index < buffer_max) {
    pthread_mutex_lock(buffer);

    const char *word = buffer[buffer_index++];
    printf("Thread %d checking %s\n", pthread_self(), word);
    if (isWordMisspelled(word))
      misspelledWords[misspelledWordsIndex++] = word;
    else
      correctWords[correctWordsIndex++] = word;

    pthread_mutex_unlock(buffer);
    sleep(1 / S);
  }

  pthread_exit(NULL);
}

void *countWords() {
  int totalCharacters = 0;
  int totalWords = correctWordsIndex + misspelledWordsIndex;
  int totalMisspelled = misspelledWordsIndex;

  // total characters
  for (int i = 0; i < correctWordsIndex; i++) {
    totalCharacters += strlen(correctWords[i]);
  }

  // total misspell words
  for (int i = 0; i < misspelledWordsIndex; i++) {
    totalCharacters += strlen(misspelledWords[i]);
  }

  // Output
  printf("Total characters: %d\n", totalCharacters);
  printf("Total words: %d\n", totalWords);
  printf("Total misspelled words: %d\n", totalMisspelled);

  pthread_exit(NULL);
}

void *saveOutput(double runtime) {
  FILE *textOutputFile = fopen("text_output.txt", "a");
  if (textOutputFile == NULL) {
    perror("Error opening text output file.");
    exit(EXIT_FAILURE);
  }
  fclose(textOutputFile);

  for (int i = 0; i < correctWordsIndex; i++) {
    fprintf(textOutputFile, "%s ", correctWords[i]);
  }

  for (int i = 0; i < misspelledWordsIndex; i++) {
    fprintf(textOutputFile, "[MS]%s[/MS] ", misspelledWords[i]);
  }
  int totalCharacters = 0;
  int totalWords = correctWordsIndex + misspelledWordsIndex;
  int totalMisspelled = misspelledWordsIndex;

  // total characters
  for (int i = 0; i < correctWordsIndex; i++) {
    totalCharacters += strlen(correctWords[i]);
  }

  // total misspell words
  for (int i = 0; i < misspelledWordsIndex; i++) {
    totalCharacters += strlen(misspelledWords[i]);
  }
  double reading_throughput = (double)totalWords / runtime;
  double spellcheck_throughput = (double)totalWords / (runtime / N);
  double writing_throughput = (double)totalWords / (runtime * W);

  // Write statistics to a file
  FILE *statsOutputFile = fopen("statistics.txt", "w");
  if (statsOutputFile == NULL) {
    perror("Error opening statistics output file.");
    exit(EXIT_FAILURE);
  }
  //write stats :)

  fprintf(statsOutputFile, "Runtime: %.9f seconds\n", runtime);
  fprintf(statsOutputFile, "Reading Throughput: %.9f words per second\n", reading_throughput);
  fprintf(statsOutputFile, "Spell-Checking Throughput: %.9f words per second\n", spellcheck_throughput);
  fprintf(statsOutputFile, "Writing Throughput: %.9f words per second\n", writing_throughput);
  fprintf(statsOutputFile, "Total characters: %d\n", totalCharacters);
  fprintf(statsOutputFile, "Total words: %d\n", totalWords);
  fprintf(statsOutputFile, "Total misspelled words: %d\n", totalMisspelled);

  fclose(statsOutputFile);
  pthread_exit(NULL);
}


int main() {
  printf("Program start.\n");

  // initialize timer
  clock_t start, end;
  // start clock
  start = clock();

  // Allocate buffer
  buffer = malloc(B * sizeof(char *));

  pthread_t spellcheckThreads[N];
  pthread_t inputThread, countWordsThread, outputThread;
  // These four will work synchronously in their thread(s).
  pthread_create(&inputThread, NULL, readInput, NULL);
  pthread_join(inputThread, NULL);
  // Print buffer contents
  printf("Buffer contents:\n");
  for (int i = 0; i < buffer_index; i++) {
    printf("word: %s\n", buffer[i]);
  }

  loadDictionary();
  buffer_max = buffer_index;
  buffer_index = 0;

  // Allocate memory for array of misspelled and correct words
  misspelledWords = malloc(buffer_max * sizeof(char *));
  correctWords = malloc(buffer_max * sizeof(char *));

  // Start spellchecks
  int spellcheckThreadCount;
  for (spellcheckThreadCount = 0; spellcheckThreadCount < N;
       spellcheckThreadCount++)
    pthread_create(&spellcheckThreads[spellcheckThreadCount], NULL, spellcheck,
                   NULL);

  // Wait for spellcheck to finish
  for (int i = 0; i < spellcheckThreadCount; i++)
    pthread_join(spellcheckThreads[i], NULL);

  // List all words
  printf("Spellcheck done!\n");
  for (int i = 0; i < misspelledWordsIndex; i++)
    printf("[MS]%s[/MS]\n", misspelledWords[i]);

  for (int i = 0; i < correctWordsIndex; i++)
    printf("%s\n", correctWords[i]);

  // stop clock
  end = clock();
  double runtime = ((double)(end - start))/CLOCKS_PER_SEC;

  pthread_create(&countWordsThread, NULL, countWords, NULL);
  pthread_create(&outputThread, NULL, saveOutput(runtime), NULL);
  pthread_join(countWordsThread, NULL);
  pthread_join(outputThread, NULL);
  
  return 0;
}