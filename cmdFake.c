#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 22 // 20 + \n and \0
#define MAX_FILE_PATH_LENGTH 102 // 100 + \n and \0

#define freealloc(...) do { \
    void* tobefreed[] = {__VA_ARGS__}; \
    for (int i = 0; i < sizeof(tobefreed) / sizeof(tobefreed[0]); i++) { \
        if (tobefreed[i] != NULL) { \
            free(tobefreed[i]); \
            tobefreed[i] = NULL; \
        } \
    } \
} while (0)

#define clearBuffer(variable, lengthOfInput) do { \
    if (strlen(variable) == (lengthOfInput - 1) && variable[lengthOfInput - 2] != '\n') { /* strlen ignores \0 */ \
        int c; \
        while ((c = getchar()) != '\n' && c != EOF) {} \
    } \
} while (0)

const char* COMMANDS[] = {"--help", "--sendhello", "--swap2textfiles", "--merge2textfiles", "--tictactoe", "--clear", "--exit"};

int commandsCount = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

int send(int true_false),
    filesSwap(),
    filesSwap_share(char* path1, char* path2, FILE** file1, FILE** file2,
                    char** file1Content, char** file2Content, size_t* charsRead1, size_t* charsRead2, int true_false),
    filesMerge(),
    filesMerge_sub(FILE** firstFile, FILE** secondFile, char* firstPath, char* secondPath);

int commandIndex(char* compare) {
    for (int i = 0; i < commandsCount; i++) {
        if (strcmp(compare, COMMANDS[i]) == 0) {
            return i;
        }
    }
    return 1;
}

int main() {
    printf("===== Fake CMD =====\n");
    send(1);
    return 0;
}

int send(int true_false) {
    char *command = malloc(MAX_COMMAND_LENGTH * sizeof(char));

    if (!command) {
        printf("Memory allocation failed");
        return 1;
    }

    if (true_false) {
        printf("\nAvailable commands:\n");
        for (int i = 0; i < commandsCount; i++) {
            printf("%s\n", COMMANDS[i]);
        }
    }
    printf("\n> ");
    
    fgets(command, MAX_COMMAND_LENGTH, stdin); // 20 characters + \n and \0

    clearBuffer(command, MAX_COMMAND_LENGTH);

    command[strcspn(command, "\n")] = '\0';

    switch(commandIndex(command)) {
        case 0:
            send(1);
            break;
        case 1:
            printf("\nHello\n");
            send(0);
            break;
        case 2:
            filesSwap();
            break;
        case 3:
            filesMerge();
            break;
        case 4:
            printf("\nnot yet\n");
            send(0);
            break;
        case 5:
            #if defined(_WIN32)
                system("cls");
            #elif defined(__unix__)
                system("clear");
            #endif
            printf("===== Fake CMD =====\n");
            send(0);
            break;
        case 6:
            break;
        default:
            send(0);
            break;
    }

    freealloc(command);
}

int filesSwap() {
    char* path1 = malloc(MAX_FILE_PATH_LENGTH * sizeof(char));
    char* path2 = malloc(MAX_FILE_PATH_LENGTH * sizeof(char)); // 100 characters + \n and \0
    char *file1Content = NULL, *file2Content = NULL;
    FILE *file1, *file2;
    size_t charsRead1 = 0, charsRead2 = 0;

    if (filesSwap_share(path1, path2, &file1, &file2, &file1Content, &file2Content, &charsRead1, &charsRead2, 1)) {
        return send(0);
    }

    if (!freopen(path1, "wb", file1) || !freopen(path2, "wb", file2)) {
        if (file1) {
            fclose(file1);
        }
        freealloc(file1Content, file2Content, path1, path2);
        perror("\nFailed to open file(s) for writing");
        return send(0);
    }

    if (fwrite(file2Content, sizeof(char), charsRead2, file1) != charsRead2 || fwrite(file1Content, sizeof(char), charsRead1, file2) != charsRead1) {
        perror("\nAn error has occurred");
    } else {
        printf("\nSuccess!\n");
    }

    freealloc(file1Content, file2Content);

    fclose(file1);
    fclose(file2);

    send(0);
}

int filesSwap_share(char* path1, char* path2, FILE** file1, FILE** file2, char** file1Content, char** file2Content, size_t* charsRead1, size_t* charsRead2, int true_false) {
    if (!path1 || !path2) {
        freealloc(path1, path2);
        perror("\nAn error has occurred");
        return 1;
    }
    
    if (true_false) {
        printf("\nPlease Input the First File Path\n>> ");
        fgets(path1, MAX_FILE_PATH_LENGTH, stdin);
        clearBuffer(path1, MAX_FILE_PATH_LENGTH);
        printf("\nPlease Input the Second File Path\n>> ");
        fgets(path2, MAX_FILE_PATH_LENGTH, stdin);
        clearBuffer(path2, MAX_FILE_PATH_LENGTH);

        path1[strcspn(path1, "\n")] = '\0';
        path2[strcspn(path2, "\n")] = '\0';
    }

    if (!(*file1 = fopen(path1, "rb")) || !(*file2 = fopen(path2, "rb"))) {
        if (*file1) {
            fclose(*file1);
        }
        freealloc(path1, path2);
        perror("\nFailed to open file(s) for reading");
        return 1;
    }

    fseek(*file1, 0, SEEK_END);
    long allocated1 = ftell(*file1);
    *file1Content = malloc(allocated1);
    fseek(*file1, 0, SEEK_SET);

    fseek(*file2, 0, SEEK_END);
    long allocated2 = ftell(*file2);
    *file2Content = malloc(allocated2);
    fseek(*file2, 0, SEEK_SET);

    if (!*file1Content || !*file2Content) {
        freealloc(*file1Content, *file2Content, path1, path2);
        fclose(*file1);
        fclose(*file2);
        perror("\nAn error has occurred");
        return 1;
    }

    *charsRead1 = fread(*file1Content, sizeof(char), allocated1 / sizeof(char), *file1);
    *charsRead2 = fread(*file2Content, sizeof(char), allocated2 / sizeof(char), *file2);

    if (*charsRead1 != allocated1 / sizeof(char) || *charsRead2 != allocated2 / sizeof(char)) {
        freealloc(*file1Content, *file2Content, path1, path2);
        fclose(*file1);
        fclose(*file2);
        perror("\nAn error has occurred");
        return 1;
    }

    freealloc(path1, path2);
    return 0;
}

int filesMerge() {
    char* path1 = malloc(MAX_FILE_PATH_LENGTH * sizeof(char));
    char* path2 = malloc(MAX_FILE_PATH_LENGTH * sizeof(char)); // 100 characters + \n and \0
    char charNum[3];
    FILE *file1, *file2;

    if (!path1 || !path2) {
        freealloc(path1, path2);
        perror("\nAn error has occurred");
        return send(0);
    }
            
    printf("\nPlease Input the First File Path\n>> ");
    fgets(path1, MAX_FILE_PATH_LENGTH, stdin);
    clearBuffer(path1, MAX_FILE_PATH_LENGTH);
    printf("\nPlease Input the Second File Path\n>> ");
    fgets(path2, MAX_FILE_PATH_LENGTH, stdin);
    clearBuffer(path2, MAX_FILE_PATH_LENGTH);

    path1[strcspn(path1, "\n")] = '\0';
    path2[strcspn(path2, "\n")] = '\0';

    while (atoi(charNum) == 0 || atoi(charNum) > 3) {
        printf("\n[1] File1 + File2\n[2] File2 + File1\n[3] File3 = File1 + File2\n\n>> ");
        fgets(charNum, 3, stdin); // 1 number and \n + \0
        clearBuffer(charNum, 3);
    }

    switch(atoi(charNum)) {
        case 1:
            filesMerge_sub(&file1, &file2, path1, path2);
            break;
        case 2:
            filesMerge_sub(&file2, &file1, path2, path1);
            break;

        case 3:
            char *file1Content = NULL, *file2Content = NULL;
            size_t charsRead1 = 0, charsRead2 = 0;
            char* path3 = malloc(MAX_FILE_PATH_LENGTH * sizeof(char));
            FILE* file3;

            if (!path3) {
                perror("\nMemory allocation failed");
                break;
            }

            printf("\nEnter the desired path of File3, which doesn't exist\n>> ");
            fgets(path3, MAX_FILE_PATH_LENGTH, stdin);
            clearBuffer(path3, MAX_FILE_PATH_LENGTH);
            path3[strcspn(path3, "\n")] = '\0';

            file3 = fopen(path3, "ab");
            freealloc(path3);

            if (!file3) {
                perror("\nFailed to create File3");
                break;
            } else if (filesSwap_share(path1, path2, &file1, &file2, &file1Content, &file2Content, &charsRead1, &charsRead2, 0)) {
                fclose(file3);
                break;
            } else if (fwrite(file1Content, sizeof(char), charsRead1, file3) != charsRead1) {
                fclose(file3);
                perror("\nAn error has occurred, File2 hasn't been added");
                break;
            } else if (fwrite(file2Content, sizeof(char), charsRead2, file3) != charsRead2) {
                fclose(file3);
                perror("\nAn error has occurred");
                break;
            } else {
                printf("\nSuccess!\n");
            }

            freealloc(file1Content, file2Content);
            fclose(file3);
            break;
        
        default:
            printf("\nAn unexpected error has occurred\n");
            send(0);
            break;
    }
    
    freealloc(path1, path2);
    return send(0);
}

int filesMerge_sub(FILE** firstFile, FILE** secondFile, char* firstPath, char* secondPath) {
    if (!(*secondFile = fopen(secondPath, "rb"))) {
        perror("\nFailed to open file for reading");
        freealloc(firstPath, secondPath);
        return 1;
    }

    fseek(*secondFile, 0, SEEK_END);
    long allocated2 = ftell(*secondFile);
    char* fileContent = malloc(allocated2);
    fseek(*secondFile, 0, SEEK_SET);

    if (!fileContent) {
        perror("\nMemory allocation failed");
        fclose(*secondFile);
        freealloc(firstPath, secondPath);
        return 1;
    }

    size_t charsRead2 = fread(fileContent, sizeof(char), allocated2 / sizeof(char), *secondFile);
    fclose(*secondFile);

    if (charsRead2 != allocated2 / sizeof(char)) {
        freealloc(fileContent, firstPath, secondPath);
        perror("\nAn error has occurred");
        return 1;
    }

    if (!(*firstFile = fopen(firstPath, "ab"))) {
        perror("\nFailed to open file for appending");
        freealloc(fileContent, firstPath, secondPath);
        return 1;
    }

    freealloc(firstPath, secondPath);

    if (fwrite(fileContent, sizeof(char), charsRead2, *firstFile) != charsRead2) {
        perror("\nAn error has occurred");
    } else {
        printf("\nSuccess!\n");
    }

    fclose(*firstFile);
    freealloc(fileContent);

    return 0;
}