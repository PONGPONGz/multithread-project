#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

const int B = 10;
const int R = 1;
const int S = 1;
const int N = 1;
const int W = 1;

char** buffer;          // Array of strings
int buffer_index = 0;   // Index to track the next available position in the buffer

void* readInput() {
    // 1. Reading Text
    // The program will read text from the standard input. However, 
    // there is a rate limit for reading. We will assume that the reading rate limit is R words per second. 
    // We will also assume that all words are separated by white space(s).
    // Please store words in the buffer.
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
            buffer[buffer_index] = malloc(strlen(word) + 1); // +1 for the null terminator
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

void* spellcheck() {
    pthread_exit(NULL);
}

void* countWords() {
    pthread_exit(NULL);
}

void* saveOutput() {
    pthread_exit(NULL);
}

int main() {
    printf("Program start.\n");

    // Allocate buffer
    buffer = malloc(B * sizeof(char*));

    pthread_t inputThread, spellcheckThread, countWordsThread, outputThread;
    // These four will work synchronously in their thread(s).
    pthread_create(&inputThread, NULL, readInput, NULL);
    pthread_create(&spellcheckThread, NULL, spellcheck, NULL);
    pthread_create(&countWordsThread, NULL, countWords, NULL);
    pthread_create(&outputThread, NULL, saveOutput, NULL);

    pthread_join(inputThread, NULL);
    // Print buffer contents
    printf("Buffer contents:\n");
    for (int i = 0; i < buffer_index; i++) {
        printf("word: %s\n", buffer[i]);
    }

    pthread_join(spellcheckThread, NULL);
    pthread_join(countWordsThread, NULL);
    pthread_join(outputThread, NULL);

    return 0; 
}