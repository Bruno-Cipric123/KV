#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_LINE 512
#define NUM_QUESTIONS 10
#define SAFE_FREE(p) if (p) { free(p); p = NULL; }

typedef struct {
    char* question;
    char* options[4];
    char correct;
} Question;

typedef struct {
    int polaPolaUsed;
    int pitajPublikuUsed;
    int nazoviPrijateljaUsed;
} Jokers;

typedef enum {
    IZAĐI = 0,
    IGRAJ = 1,
    DODAJ,
    PREGLED,
    OBRIŠI,
    UREDI
} MenuOption;

void trimNewline(char* str);
int loadQuestions(const char* filename, Question** questionsOut);
void saveQuestions(const char* filename, Question* questions, int count);
void printQuestion(const Question* q, int index);
void printAllQuestions(const Question* questions, int count);
void usePolaPola(const Question* q);
void usePitajPubliku(const Question* q);
void useNazoviPrijatelja(const Question* q);
void freeQuestions(Question* questions, int count);
void editQuestion(const char* filename);
void addQuestion(const char* filename);
void deleteQuestion(const char* filename);
void shuffleQuestions(Question* questions, int count);
void runGame(Question* questions, int count);
void sortQuestions(Question* questions, int count);
int compareQuestions(const void* a, const void* b);
void searchQuestion(const char* filename);




#endif

