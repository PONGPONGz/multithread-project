#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int B = 10;
const int R = 1;
const int S = 1;
const int N = 1;
const int W = 1;

char** buffer;          // Array of strings

void* readInput() {
    // 1. Reading Text
    // The program will read text from the standard input. However, 
    // there is a rate limit for reading. We will assume that the reading rate limit is R words per second. 
    // We will also assume that all words are separated by white space(s).
    // Please store words in the buffer.
}

void* spellcheck() {
    
}

void* countWords() {
    
}

void* saveOutput() {
    
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
    pthread_join(spellcheckThread, NULL);
    pthread_join(countWordsThread, NULL);
    pthread_join(outputThread, NULL);
    return 0;
}