#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "Functions.h"


void trimNewline(char* str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

int loadQuestions(const char* filename, Question** questionsOut) {
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
            if (!fgets(line, MAX_LINE, fp)) {
                valid = 0; break;
            }
            trimNewline(line);
            q.options[i] = _strdup(line);
        }

        if (!fgets(line, MAX_LINE, fp)) { valid = 0; }
        trimNewline(line);
        if (strlen(line) < 1) valid = 0;
        q.correct = line[0];

        if (!valid) {
            fprintf(stderr, "Upozorenje: Pitanje nije potpuno, preskacem...\n");
            SAFE_FREE(q.question);
            for (int i = 0; i < 4; ++i) SAFE_FREE(q.options[i]);
            continue;
        }

        Question* temp = realloc(questions, sizeof(Question) * (count + 1));
        if (!temp) {
            perror("realloc neuspjesan");
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
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Greska pri spremanju pitanja");
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

void printQuestion(const Question* q, int index) {
    printf("\nPitanje %d: %s\n", index + 1, q->question);
    for (int i = 0; i < 4; ++i)
        printf("%c) %s\n", 'A' + i, q->options[i]);
}

void printAllQuestions(const Question* questions, int count) {
    for (int i = 0; i < count; ++i)
        printQuestion(&questions[i], i);
}

void usePolaPola(const Question* q) {
    int shown = 0;
    printf("\nJOKER POLA POLA:\n");
    for (int i = 0; i < 4; ++i) {
        if ('A' + i == q->correct || shown < 1) {
            printf("%c) %s\n", 'A' + i, q->options[i]);
            if ('A' + i != q->correct) shown++;
        }
    }
}

void usePitajPubliku(const Question* q) {
    printf("\nJOKER PITAJ PUBLIKU:\nPublika misli da je tocan odgovor: %c\n", q->correct);
}

void useNazoviPrijatelja(const Question* q) {
    printf("\nJOKER NAZOVI PRIJATELJA:\nPrijatelj misli da je odgovor: %c\n", q->correct);
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

void editQuestion(const char* filename) {
    Question* questions = NULL;
    int total = loadQuestions(filename, &questions);
    if (total <= 0) {
        printf("Nema pitanja za uredivanje.\n");
        return;
    }

    printAllQuestions(questions, total);
    printf("\nUnesite broj pitanja za uredivanje (1-%d): ", total);
    int index;
    scanf("%d", &index);
    getchar();

    if (index < 1 || index > total) {
        printf("Nevazeci broj.\n");
        freeQuestions(questions, total);
        return;
    }

    Question* q = &questions[index - 1];
    char buffer[MAX_LINE];

    printf("Novo pitanje (trenutno: %s):\n", q->question);
    fgets(buffer, MAX_LINE, stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) {
        SAFE_FREE(q->question);
        q->question = _strdup(buffer);
    }

    for (int i = 0; i < 4; ++i) {
        printf("Opcija %c (trenutno: %s):\n", 'A' + i, q->options[i]);
        fgets(buffer, MAX_LINE, stdin); trimNewline(buffer);
        if (strlen(buffer) > 0) {
            SAFE_FREE(q->options[i]);
            q->options[i] = _strdup(buffer);
        }
    }

    printf("Tocan odgovor (trenutno: %c): ", q->correct);
    fgets(buffer, MAX_LINE, stdin); trimNewline(buffer);
    if (strlen(buffer) == 1 && buffer[0] >= 'A' && buffer[0] <= 'D') {
        q->correct = buffer[0];
    }

    saveQuestions(filename, questions, total);
    printf("Pitanje azurirano.\n");
    freeQuestions(questions, total);
}

void addQuestion(const char* filename) {
    Question q = { 0 };
    char buffer[MAX_LINE];

    printf("Unesite pitanje:\n");
    fgets(buffer, MAX_LINE, stdin); trimNewline(buffer);
    q.question = _strdup(buffer);

    for (int i = 0; i < 4; ++i) {
        printf("Opcija %c:\n", 'A' + i);
        fgets(buffer, MAX_LINE, stdin); trimNewline(buffer);
        q.options[i] = _strdup(buffer);
    }

    printf("Tocan odgovor (A-D): ");
    fgets(buffer, MAX_LINE, stdin); trimNewline(buffer);
    q.correct = buffer[0];

    Question* questions = NULL;
    int total = loadQuestions(filename, &questions);

    Question* temp = realloc(questions, sizeof(Question) * (total + 1));
    if (!temp) {
        perror("realloc");
        freeQuestions(questions, total);
        return;
    }

    questions = temp;
    questions[total++] = q;
    saveQuestions(filename, questions, total);
    freeQuestions(questions, total);
    printf("Pitanje dodano.\n");
}

void deleteQuestion(const char* filename) {
    Question* questions = NULL;
    int total = loadQuestions(filename, &questions);
    if (total <= 0) {
        printf("Nema pitanja za brisanje.\n");
        return;
    }

    printAllQuestions(questions, total);
    printf("\nUnesite broj pitanja za brisanje (1-%d): ", total);
    int index;
    scanf("%d", &index); getchar();
    if (index < 1 || index > total) {
        printf("Nevažeci broj.\n");
        freeQuestions(questions, total);
        return;
    }

    SAFE_FREE(questions[index - 1].question);
    for (int i = 0; i < 4; ++i) {
        SAFE_FREE(questions[index - 1].options[i]);
    }

    for (int i = index; i < total; ++i)
        questions[i - 1] = questions[i];

    saveQuestions(filename, questions, total - 1);
    freeQuestions(questions, total - 1);
    printf("Pitanje obrisano.\n");
}

void shuffleQuestions(Question* questions, int count) {
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

void runGame(Question* questions, int count) {
    char input;
    int score = 0;
    int limit = count < NUM_QUESTIONS ? count : NUM_QUESTIONS;
    Jokers jokers = { 0, 0, 0 };

    for (int i = 0; i < limit; ++i) {
        printQuestion(&questions[i], i);
        printf("Dostupni jokeri: [1] Pola-Pola %s  [2] Pitaj publiku %s  [3] Nazovi prijatelja %s\n",
            jokers.polaPolaUsed ? "(iskoristen)" : "", jokers.pitajPublikuUsed ? "(iskoristen)" : "", jokers.nazoviPrijateljaUsed ? "(iskoristen)" : "");

        printf("Unesite odgovor (A-D) ili broj jokera (1-3): ");
        scanf(" %c", &input);

        if (input >= '1' && input <= '3') {
            switch (input) {
            case '1':
                if (!jokers.polaPolaUsed) {
                    usePolaPola(&questions[i]);
                    jokers.polaPolaUsed = 1;
                }
                else printf("Joker Pola-Pola je vec iskoristen.\n");
                --i; continue;
            case '2':
                if (!jokers.pitajPublikuUsed) {
                    usePitajPubliku(&questions[i]);
                    jokers.pitajPublikuUsed = 1;
                }
                else printf("Joker Pitaj publiku je vec iskoristen.\n");
                --i; continue;
            case '3':
                if (!jokers.nazoviPrijateljaUsed) {
                    useNazoviPrijatelja(&questions[i]);
                    jokers.nazoviPrijateljaUsed = 1;
                }
                else printf("Joker Nazovi prijatelja je vec iskoristen.\n");
                --i; continue;
            }
        }

        if (input == questions[i].correct) {
            printf("Tocno!\n");
            score += 1000;
        }
        else {
            printf("Netocno. Tocan odgovor je %c.\n", questions[i].correct);
            break;
        }
    }

    printf("\nKraj igre. Ukupan broj bodova: %d\n", score);
}
