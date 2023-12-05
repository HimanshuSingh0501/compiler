#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROD 10
#define MAX_LEN 10

int isNonTerminal(char symbol) {
    return isupper(symbol);
}

int isTerminal(char symbol) {
    return islower(symbol) || symbol == '$';
}

int findIndex(char symbol) {
    return symbol - 'A';
}

int isNullable(char nonTerminal, int nullable[]) {
    return nullable[findIndex(nonTerminal)];
}

void markNullable(char production[][MAX_LEN], int nullable[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (production[i][2] == '$') {
            nullable[findIndex(production[i][0])] = 1;
        }
    }
}

void computeFirst(char production[][MAX_LEN], char nonTerm, char firstSet[][MAX_LEN], int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (production[i][0] == nonTerm) {
            if (isTerminal(production[i][2])) {
                strncat(firstSet[findIndex(nonTerm)], &production[i][2], 1);
            } else if (isNonTerminal(production[i][2])) {
                computeFirst(production, production[i][2], firstSet, n);
            }
        }
    }
}

void computeFollow(char production[][MAX_LEN], char nonTerm, char followSet[][MAX_LEN], int n, int nullable[]) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 2; j < strlen(production[i]); j++) {
            if (production[i][j] == nonTerm) {
                if (j == strlen(production[i]) - 1) {
                    computeFollow(production, production[i][0], followSet, n, nullable);
                } else {
                    if (isTerminal(production[i][j + 1])) {
                        strncat(followSet[findIndex(nonTerm)], &production[i][j + 1], 1);
                    } else if (isNonTerminal(production[i][j + 1])) {
                        char firstOfNext[MAX_LEN];
                        computeFirst(production, production[i][j + 1], firstOfNext, n);
                        int k;
                        for (k = 0; k < strlen(firstOfNext); k++) {
                            if (firstOfNext[k] != '$') {
                                strncat(followSet[findIndex(nonTerm)], &firstOfNext[k], 1);
                            } else {
                                computeFollow(production, production[i][0], followSet, n, nullable);
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    int n, i;
    printf("Enter the number of productions: ");
    scanf("%d", &n);

    char production[MAX_PROD][MAX_LEN];
    printf("Enter the productions:\n");
    for (i = 0; i < n; i++) {
        scanf("%s", production[i]);
    }

    char nonTerminals[MAX_PROD];
    int numNonTerminals = 0;

    for (i = 0; i < n; i++) {
        if (!isNonTerminal(production[i][0])) {
            printf("Invalid production. Please enter productions for a context-free grammar.\n");
            return 1;
        }

        if (!strchr(nonTerminals, production[i][0])) {
            nonTerminals[numNonTerminals++] = production[i][0];
        }
    }

    nonTerminals[numNonTerminals] = '\0';

    int nullable[26] = {0};

    markNullable(production, nullable, n);

    char firstSet[26][MAX_LEN] = {0};
    char followSet[26][MAX_LEN] = {0};

    for (i = 0; i < numNonTerminals; i++) {
        computeFirst(production, nonTerminals[i], firstSet, n);
    }

    for (i = 0; i < numNonTerminals; i++) {
        computeFollow(production, nonTerminals[i], followSet, n, nullable);
    }

    printf("\nFirst Set:\n");
    for (i = 0; i < numNonTerminals; i++) {
        printf("First(%c) = {%s}\n", nonTerminals[i], firstSet[i]);
    }

    printf("\nFollow Set:\n");
    for (i = 0; i < numNonTerminals; i++) {
        printf("Follow(%c) = {%s}\n", nonTerminals[i], followSet[i]);
    }

    printf("\nLL(1) Parsing Table:\n");
    printf("NonTerminal\t\t");
    for (i = 0; i < 26; i++) {
        if (nonTerminals[i] != '\0') {
            printf("%c\t", nonTerminals[i]);
        }
    }
    printf("\n");

    for (i = 0; i < numNonTerminals; i++) {
        printf("%c\t\t\t", nonTerminals[i]);
        int j;
        for (j = 0; j < numNonTerminals; j++) {
            if (nonTerminals[j] != '\0') {
                int k;
                for (k = 0; k < n; k++) {
                    if (production[k][0] == nonTerminals[i]) {
                        if (isTerminal(production[k][2]) && strchr(firstSet[j], production[k][2])) {
                            printf("%s\t", production[k]);
                            break;
                        } else if (isNonTerminal(production[k][2]) && strchr(firstSet[j], production[k][2])) {
                            printf("%s\t", production[k]);
                        } else if (isNullable(production[k][2], nullable) && strchr(followSet[j], nonTerminals[i])) {
                            printf("%s\t", production[k]);
                        }
                    }
                }
            }
        }
        printf("\n");
    }

    return 0;
}
