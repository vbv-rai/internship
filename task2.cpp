#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_VARS 100

typedef enum {
    TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_SEMICOLON, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LEN];
} Token;

typedef struct {
    char name[MAX_TOKEN_LEN];
    int value;
} Variable;

Variable symbolTable[MAX_VARS];
int varCount = 0;

void storeVariable(char *name, int value) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            symbolTable[i].value = value;
            return;
        }
    }
    strcpy(symbolTable[varCount].name, name);
    symbolTable[varCount].value = value;
    varCount++;
}

int getVariableValue(char *name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].value;
        }
    }
    printf("Error: Undefined variable %s\n", name);
    exit(1);
}

void getNextToken(FILE *file, Token *token) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) continue;

        if (isalpha(c)) {
            int len = 0;
            token->text[len++] = c;
            while (isalnum(c = fgetc(file))) {
                if (len < MAX_TOKEN_LEN - 1) token->text[len++] = c;
            }
            ungetc(c, file);
            token->text[len] = '\0';
            token->type = TOKEN_IDENTIFIER;
            return;
        }

        if (isdigit(c)) {
            int len = 0;
            token->text[len++] = c;
            while (isdigit(c = fgetc(file))) {
                if (len < MAX_TOKEN_LEN - 1) token->text[len++] = c;
            }
            ungetc(c, file);
            token->text[len] = '\0';
            token->type = TOKEN_NUMBER;
            return;
        }

        switch (c) {
            case '=': token->type = TOKEN_ASSIGN; token->text[0] = '='; token->text[1] = '\0'; return;
            case '+': token->type = TOKEN_PLUS; token->text[0] = '+'; token->text[1] = '\0'; return;
            case '-': token->type = TOKEN_MINUS; token->text[0] = '-'; token->text[1] = '\0'; return;
            case '*': token->type = TOKEN_MULTIPLY; token->text[0] = '*'; token->text[1] = '\0'; return;
            case '/': token->type = TOKEN_DIVIDE; token->text[0] = '/'; token->text[1] = '\0'; return;
            case ';': token->type = TOKEN_SEMICOLON; token->text[0] = ';'; token->text[1] = '\0'; return;
        }
    }
    token->type = TOKEN_EOF;
    token->text[0] = '\0';
}

int parseExpression(FILE *file, Token *token);

int parseTerm(FILE *file, Token *token) {
    int value;
    if (token->type == TOKEN_NUMBER) {
        value = atoi(token->text);
        getNextToken(file, token);
    } else if (token->type == TOKEN_IDENTIFIER) {
        value = getVariableValue(token->text);
        getNextToken(file, token);
    } else {
        printf("Syntax Error: Expected number or identifier\n");
        exit(1);
    }
    return value;
}

int parseExpression(FILE *file, Token *token) {
    int value = parseTerm(file, token);
    while (token->type == TOKEN_PLUS || token->type == TOKEN_MINUS) {
        TokenType op = token->type;
        getNextToken(file, token);
        int right = parseTerm(file, token);
        if (op == TOKEN_PLUS) value += right;
        else value -= right;
    }
    return value;
}

void parseStatement(FILE *file, Token *token) {
    if (token->type == TOKEN_IDENTIFIER) {
        char varName[MAX_TOKEN_LEN];
        strcpy(varName, token->text);
        getNextToken(file, token);
        if (token->type != TOKEN_ASSIGN) {
            printf("Syntax Error: Expected '='\n");
            exit(1);
        }
        getNextToken(file, token);
        int value = parseExpression(file, token);
        storeVariable(varName, value);
        if (token->type != TOKEN_SEMICOLON) {
            printf("Syntax Error: Expected ';'\n");
            exit(1);
        }
        getNextToken(file, token);
    } else {
        printf("Syntax Error: Expected identifier\n");
        exit(1);
    }
}

void parse(FILE *file) {
    Token token;
    getNextToken(file, &token);
    while (token.type != TOKEN_EOF) {
        parseStatement(file, &token);
    }
}

int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    parse(file);
    fclose(file);

    printf("\nSymbol Table:\n");
    for (int i = 0; i < varCount; i++) {
        printf("%s = %d\n", symbolTable[i].name, symbolTable[i].value);
    }

    return 0;
}
