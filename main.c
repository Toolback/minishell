#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CMD_LEN 256
#define MAX_NUM_ARGS 10

// Fonction pour exécuter une commande
void execute_command(char *cmd) {
    char *args[MAX_NUM_ARGS];
    int i = 0;

    // Diviser la commande en arguments
    args[i] = strtok(cmd, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    // Vérifier la commande
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            // Nous sommes dans le processus fils
            execvp(args[0], args);
            // Si execvp retourne, c'est qu'une erreur s'est produite
            printf("minishell: %s: command not found\n", args[0]);
            exit(1);
        } else {
            // Nous sommes dans le processus parent
            wait(NULL);
        }
    }
}

// Gestionnaire de signaux
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nminishell ▸ ");
        fflush(stdout);
    }
}

int main() {
    char cmd[MAX_CMD_LEN];
    signal(SIGINT, handle_signal);

    while (1) {
        printf("minishell ▸ ");
        if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
            // L'utilisateur a tapé CTRL+D
            printf("\n");
            exit(0);
        }
        cmd[strcspn(cmd, "\n")] = 0;  // Enlever le retour à la ligne
d    }
    return 0;
}
