#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "Functions.h"

static inline void trimNewline(char* str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

int loadQuestions(const char* filename, Question** questionsOut) {
    if (!filename || !questionsOut) return 0;

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Greska pri otvaranju datoteke");
        return 0;
    }

    int count = 0;
    Question* questions = NULL;
    char line[MAX_LINE];

    while (1) {
        Question q = { 0 };
        int valid = 1;

        if (!fgets(line, MAX_LINE, fp)) break;
        trimNewline(line);
        if (strlen(line) == 0) continue;
        q.question = _strdup(line);

        for (int i = 0; i < 4; ++i) {
            if (!fgets(line, MAX_LINE, fp)) { valid = 0; break; }
            trimNewline(line);
            q.options[i] = _strdup(line);
        }

        if (!fgets(line, MAX_LINE, fp)) { valid = 0; }
        trimNewline(line);
        if (strlen(line) < 1) valid = 0;
        q.correct = line[0];

        if (!valid) {
            fprintf(stderr, "Upozorenje: neispravno pitanje, preskacem...\n");
            SAFE_FREE(q.question);
            for (int i = 0; i < 4; ++i) SAFE_FREE(q.options[i]);
            continue;
        }

        Question* temp = (Question*)realloc(questions, sizeof(Question) * (count + 1));
        if (!temp) {
            perror("realloc nije uspio");
            SAFE_FREE(q.question);
            for (int i = 0; i < 4; ++i) SAFE_FREE(q.options[i]);
            break;
        }

        questions = temp;
        questions[count++] = q;
    }

    fclose(fp);
    *questionsOut = questions;
    return count;
}

void saveQuestions(const char* filename, Question* questions, int count) {
    if (!filename || !questions) return;

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Greska pri spremanju datoteke");
        return;
    }

    for (int i = 0; i < count; ++i) {
        fprintf(fp, "%s\n", questions[i].question);
        for (int j = 0; j < 4; ++j)
            fprintf(fp, "%s\n", questions[i].options[j]);
        fprintf(fp, "%c\n", questions[i].correct);
    }

    fclose(fp);
}

void freeQuestions(Question* questions, int count) {
    if (!questions) return;
    for (int i = 0; i < count; ++i) {
        SAFE_FREE(questions[i].question);
        for (int j = 0; j < 4; ++j)
            SAFE_FREE(questions[i].options[j]);
    }
    SAFE_FREE(questions);
}

static int compareQuestions(const void* a, const void* b) {
    const Question* q1 = (const Question*)a;
    const Question* q2 = (const Question*)b;
    return strcmp(q1->question, q2->question);
}

void sortQuestions(Question* questions, int count) {
    if (!questions || count <= 0) return;
    qsort(questions, count, sizeof(Question), compareQuestions);
}

static int compareQuestionText(const void* a, const void* b) {
    const Question* qa = (const Question*)a;
    const char* text = (const char*)b;
    return strcmp(qa->question, text);
}

void searchQuestion(const char* filename) {
    if (!filename) return;

    Question* questions = NULL;
    int total = loadQuestions(filename, &questions);
    if (total <= 0) {
        printf("Nema dostupnih pitanja.\n");
        return;
    }

    sortQuestions(questions, total);

    char input[MAX_LINE];
    printf("Unesite tekst pitanja: ");
    fgets(input, MAX_LINE, stdin);
    trimNewline(input);

    Question* found = (Question*)bsearch(input, questions, total, sizeof(Question), compareQuestionText);

    if (found) {
        printf("Pitanje: %s\n", found->question);
        printf("Tocan odgovor: %c) %s\n", found->correct, found->options[found->correct - 'A']);
    } else {
        printf("Pitanje nije pronadjeno.\n");
    }

    freeQuestions(questions, total);
}
