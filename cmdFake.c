#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

const char* COMMANDS[] = {"--help", "--sendhello", "--fileswap", "--ttt", "--exit"};

void send(char* store, int true_false);

int commandIndex(char* compare) {
    for (int i = 0; i < 5; i++) {
        if (strcmp(compare, COMMANDS[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char *command = malloc(21 * sizeof(char));

    if (command == NULL) {
        printf("Memory allocation failed");
        return 1;
    }

    printf("===== Fake CMD =====\n");
    send(command, 1);

    free(command);
    return 0;
}

void send(char* store, int true_false) {
    if (true_false) {
        printf("\nAvailable commands:\n--help\n--sendhello\n--fileswap\n--ttt\n--exit");
    }
    printf("\n\n> ");
    
    fgets(store, 21, stdin);

    if (strlen(store) == 20 && store[19] != '\n') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    store[strcspn(store, "\n")] = '\0';

    switch(commandIndex(store)) {
        case 0:
            send(store, 1);
            break;
        case 1:
            printf("\nHello");
            send(store, 0);
            break;
        case 2:
            char* path1 = malloc(101 * sizeof(char));
            char* path2 = malloc(101 * sizeof(char));
            if (path1 == NULL || path2 == NULL) {
                perror("\nAn error has occurred:");
                send(store, 0);
            }

            FILE *file1, *file2;
            
            printf("\nPlease Input the First File Path\n>> ");
            fgets(path1, 101, stdin);
            printf("\nPlease Input the Second File Path\n>> ");
            fgets(path2, 101, stdin);

            path1[strcspn(path1, "\n")] = '\0';
            path2[strcspn(path2, "\n")] = '\0';

            if ((file1 = fopen(path1, "r")) == NULL || (file2 = fopen(path2, "r")) == NULL) {
                if (file1) {
                    fclose(file1);
                }
                free(path1);
                free(path2);
                perror("\nFailed to open file(s) for reading: ");
                send(store, 0);
            }

            fseek(file1, 0, SEEK_END);
            size_t allocated1 = ftell(file1);
            char* file1Content = malloc(allocated1 + 1);
            fseek(file1, 0, SEEK_SET);

            fseek(file2, 0, SEEK_END);
            size_t allocated2 = ftell(file2);
            char* file2Content = malloc(allocated2 + 1);
            fseek(file2, 0, SEEK_SET);

            if (file1Content == NULL || file2Content == NULL) {
                free(file1Content);
                free(file2Content);
                perror("\nAn error has occurred:");
                send(store, 0);
            }

            size_t bytesRead1 = fread(file1Content, sizeof(char), allocated1 / sizeof(char), file1);
            size_t bytesRead2 = fread(file2Content, sizeof(char), allocated2 / sizeof(char), file2);
            file1Content[bytesRead1] = '\0';
            file2Content[bytesRead2] = '\0';

            fclose(file1);
            fclose(file2);

            if ((file1 = fopen(path1, "w")) == NULL || (file2 = fopen(path2, "w")) == NULL) {
                if (file1) {
                    fclose(file1);
                }
                free(file1Content);
                free(file2Content);
                perror("\nFailed to open file(s) for writing:");
                send(store, 0);
            }

            free(path1);
            free(path2);

            if (fputs(file2Content, file1) == EOF || fputs(file1Content, file2) == EOF) {
                perror("\nAn error has occurred:");
            } else {
                printf("\nSuccess!");
            }

            free(file1Content);
            free(file2Content);

            fclose(file1);
            fclose(file2);

            send(store, 0);
            break;
        case 3:
            printf("\nnot yet");
            send(store, 0);
            break;
        case 4:
            exit(EXIT_SUCCESS);
        default:
            send(store, 0);
            break;
    }
}