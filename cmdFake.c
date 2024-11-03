#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define freealloc(...) do { \
    void* tobefreed[] = {__VA_ARGS__}; \
    for (int i = 0; i < sizeof(tobefreed) / sizeof(tobefreed[0]); i++) { \
        free(tobefreed[i]); \
        tobefreed[i] = NULL; \
    } \
} while (0)

#define clearBuffer(variable, lengthOfInput) do { \
    if (strlen(variable) == (lengthOfInput - 1) && variable[lengthOfInput - 2] != '\n') { /* strlen ignores \0 */ \
        int c; \
        while ((c = getchar()) != '\n' && c != EOF) {} \
    } \
} while (0)

const char* COMMANDS[] = {"--help", "--sendhello", "--swap2textfiles", "--tictactoe", "--clear", "--exit"};

int commandsCount = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void send(char* cmd, int true_false);

void fileSwap(char* cmd);

int commandIndex(char* compare) {
    for (int i = 0; i < commandsCount; i++) {
        if (strcmp(compare, COMMANDS[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char *command = malloc(22 * sizeof(char));

    if (command == NULL) {
        printf("Memory allocation failed");
        return 1;
    }

    printf("===== Fake CMD =====\n");
    send(command, 1);

    freealloc(command);
    return 0;
}

void send(char* cmd, int true_false) {
    if (true_false) {
        printf("\nAvailable commands:\n");
        for (int i = 0; i < commandsCount; i++) {
            printf("%s\n", COMMANDS[i]);
        }
    }
    printf("\n> ");
    
    fgets(cmd, 22, stdin); // 20 characters + \n and \0

    clearBuffer(cmd, 22);

    cmd[strcspn(cmd, "\n")] = '\0';

    switch(commandIndex(cmd)) {
        case 0:
            send(cmd, 1);
            break;
        case 1:
            printf("\nHello\n");
            send(cmd, 0);
            break;
        case 2:
            fileSwap(cmd);
            break;
        case 3:
            printf("\nnot yet\n");
            send(cmd, 0);
            break;
        case 4:
            #if defined(_WIN32)
                system("cls");
            #elif defined(__unix__)
                system("clear");
            #endif
            printf("===== Fake CMD =====\n");
            send(cmd, 0);
            break;
        case 5:
            break;
        default:
            send(cmd, 0);
            break;
    }
}

void fileSwap(char* cmd) {
    char* path1 = malloc(102 * sizeof(char));
    char* path2 = malloc(102 * sizeof(char)); // 100 characters + \n and \0
    if (path1 == NULL || path2 == NULL) {
        freealloc(path1, path2);
        perror("\nAn error has occurred");
        return send(cmd, 0);
    }

    FILE *file1, *file2;
            
    printf("\nPlease Input the First File Path\n>> ");
    fgets(path1, 102, stdin);
    clearBuffer(path1, 102);
    printf("\nPlease Input the Second File Path\n>> ");
    fgets(path2, 102, stdin);
    clearBuffer(path2, 102);

    path1[strcspn(path1, "\n")] = '\0';
    path2[strcspn(path2, "\n")] = '\0';

    if ((file1 = fopen(path1, "rb")) == NULL || (file2 = fopen(path2, "rb")) == NULL) {
        if (file1) {
            fclose(file1);
        }
        freealloc(path1, path2);
        perror("\nFailed to open file(s) for reading");
        return send(cmd, 0);
    }

    fseek(file1, 0, SEEK_END);
    long allocated1 = ftell(file1);
    char* file1Content = malloc(allocated1);
    fseek(file1, 0, SEEK_SET);

    fseek(file2, 0, SEEK_END);
    long allocated2 = ftell(file2);
    char* file2Content = malloc(allocated2);
    fseek(file2, 0, SEEK_SET);

    if (file1Content == NULL || file2Content == NULL) {
        freealloc(file1Content, file2Content, path1, path2);
        fclose(file1);
        fclose(file2);
        perror("\nAn error has occurred");
        return send(cmd, 0);
    }

    size_t charsRead1 = fread(file1Content, sizeof(char), allocated1 / sizeof(char), file1);
    size_t charsRead2 = fread(file2Content, sizeof(char), allocated2 / sizeof(char), file2);

    if (charsRead1 != allocated1 / sizeof(char) || charsRead2 != allocated2 / sizeof(char)) {
        freealloc(file1Content, file2Content, path1, path2);
        fclose(file1);
        fclose(file2);
        perror("\nAn error has occurred");
        return send(cmd, 0);
    }

    if (freopen(path1, "wb", file1) == NULL || freopen(path2, "wb", file2) == NULL) {
        if (file1) {
            fclose(file1);
        }
        freealloc(file1Content, file2Content, path1, path2);
        perror("\nFailed to open file(s) for writing");
        return send(cmd, 0);
    }

    freealloc(path1, path2);

    if (fwrite(file2Content, sizeof(char), charsRead2, file1) != charsRead2 || fwrite(file1Content, sizeof(char), charsRead1, file2) != charsRead1) {
        perror("\nAn error has occurred");
    } else {
        printf("\nSuccess!\n");
    }

    freealloc(file1Content, file2Content);

    fclose(file1);
    fclose(file2);

    send(cmd, 0);
}
