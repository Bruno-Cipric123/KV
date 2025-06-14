#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "Functions.h"


#define MAX_LINE 512
#define NUM_QUESTIONS 10
#define SAFE_FREE(p) if (p) { free(p); p = NULL; }

#ifndef _MSC_VER
#define _strdup strdup
#endif

int main() {
    srand((unsigned int)time(NULL));
    const char* filename = "pitanja.txt";

    while (1) {
        printf("\n===== MILIJUNAS =====\n");
        printf("1. Pokreni igru\n");
        printf("2. Dodaj pitanje\n");
        printf("3. Pregledaj pitanja\n");
        printf("4. Obrisi pitanje\n");
        printf("5. Uredi pitanje\n");
        printf("0. Izlaz\n");
        printf("Odabir: ");

        int izbor;
        char buffer[16];
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &izbor) != 1) continue;

        switch (izbor) {
        case IGRAJ: {
            Question* questions = NULL;
            int total = loadQuestions(filename, &questions);
            if (total > 0) {
                shuffleQuestions(questions, total);
                runGame(questions, total);
                freeQuestions(questions, total);
            }
            else {
                printf("Nema dostupnih pitanja.\n");
            }
            break;
        }
        case DODAJ:
            addQuestion(filename);
            break;
        case PREGLED: {
            Question* questions = NULL;
            int total = loadQuestions(filename, &questions);
            if (total > 0) {
                printAllQuestions(questions, total);
                freeQuestions(questions, total);
            }
            else {
                printf("Nema pitanja za prikaz.\n");
            }
            break;
        }
        case OBRIŠI:
            deleteQuestion(filename);
            break;
        case UREDI:
            editQuestion(filename);
            break;
        case IZAĐI:
            return EXIT_SUCCESS;
        default:
            printf("Nepoznata opcija.\n");
        }
    }
}
