#include "minishell.h"

// Définit les limites pour le nombre d'arguments d'une commande et la taille d'une ligne
#define MAX_ARGS 50
#define MAX_LINE 1024

// Fonction pour exécuter une commande
void execute_command(t_data data)
{
    // Déclare des descripteurs de fichier et des variables pour les entrées et sorties
    int fds[2];
    int in = 0;
    int out;
    t_token *t = data.token;
    pid_t pid;
    // Compteur pour les processus enfants
    int child_processes = 0;

    // Boucle tant que le token n'est pas NULL
    while (t != NULL)
    {
        out = 1; // Réinitialise 'out' pour chaque commande

        // Si le type de token est une commande
        if (t->type == cmd)
        {
            // Crée un tableau pour les arguments de la commande
            char *args[MAX_ARGS] = {t->str};
            int i = 1;
            // Remplit le tableau avec les arguments de la commande jusqu'à un certain type de token ou une certaine limite
            while (t->next && t->next->type != pipeline && t->next->type != simple_redir_right &&
                   t->next->type != simple_redir_left && t->next->type != double_redir_right &&
                   t->next->type != double_redir_left && i < MAX_ARGS - 1)
            {
                t = t->next;
                args[i++] = t->str;
            }
            args[i] = NULL;

            // Gère les redirections après les arguments de la commande
            if (t->next && (t->next->type == simple_redir_right || t->next->type == double_redir_right ||
                            t->next->type == simple_redir_left || t->next->type == double_redir_left))
            {
                t = t->next;
                // Pour les différents types de redirections, ouvre les fichiers appropriés
                if (t->type == simple_redir_right)
                {
                    t = t->next;
                    out = open(t->str, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                }
                else if (t->type == double_redir_right)
                {
                    t = t->next;
                    out = open(t->str, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                }
                else if (t->type == simple_redir_left)
                {
                    t = t->next;
                    in = open(t->str, O_RDONLY);
                }
                else if (t->type == double_redir_left)
                {
                    // Pour la redirection double vers la gauche, crée un fichier temporaire et y écrit les lignes d'entrée
                    t = t->next;
                    char *input_lines = t->str;
                    char *end_word = strtok(input_lines, "\n");
                    char tmp_filename[] = "/tmp/double_redir_left_XXXXXX";
                    in = mkstemp(tmp_filename);
                    char *line = strtok(NULL, "\n");
                    while (line && strcmp(line, end_word) != 0)
                    {
                        write(in, line, strlen(line));
                        write(in, "\n", 1);
                        line = strtok(NULL, "\n");
                    }
                    lseek(in, 0, SEEK_SET); // Retourne au début du fichier temporaire
                }
            }

            // Configure le pipeline
            pipe(fds);

            // Crée un nouveau processus
            if ((pid = fork()) == -1)
            {
                perror("fork");
                exit(1);
            }

            // Si le processus est un processus enfant
            if (pid == 0)
            {
                // Redirige stdin si nécessaire
                if (in != 0)
                {
                    dup2(in, 0);
                    close(in);
                }

                // Redirige stdout si nécessaire
                if (out != 1)
                {
                    dup2(out, 1);
                    close(out);
                }

                // Redirige stdout vers le pipeline si ce n'est pas la dernière commande
                if (t->next && t->next->type == pipeline)
                {
                    dup2(fds[1], 1);
                }
                close(fds[0]);
                close(fds[1]);

                // Prépare les variables d'environnement si nécessaire
                char **envp = NULL; 
                // Exécute la commande
                if (execve(args[0], args, envp) == -1)
                {
                    perror("execve");
                    printf("Commande non trouvée [%s]\n", args[0]);
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
            // Si le processus est le processus parent
            else
            {
                child_processes++; // Incrémente le compteur de processus enfants
                close(fds[1]);     // Ferme l'extrémité d'écriture du pipeline immédiatement
                if (in != 0)
                {
                    close(in); // Ferme le descripteur de fichier d'entrée de la boucle précédente
                }
                in = fds[0]; // Garde la trace de l'entrée pour la prochaine commande
            }
        }
        t = t->next;
    }

    if (in != 0)
    {
        close(in);
    }

    // Attend que tous les processus enfants se terminent
    for (int i = 0; i < child_processes; i++)
    {
        wait(NULL);
    }
}


char *getAbsoluteCommandPath(const char *command)
{
    // Replace these paths with the actual absolute paths for your system
    if (strcmp(command, "cat") == 0)
        return "/bin/cat";
    else if (strcmp(command, "grep") == 0)
        return "/usr/bin/grep";
    else if (strcmp(command, "yes") == 0)
        return "/usr/bin/yes";
    else if (strcmp(command, "ls") == 0)
        return "/bin/ls";
    else if (strcmp(command, "wc") == 0)
        return "/usr/bin/wc";
    else
        return NULL; // Command not found
}

void super_executer(t_data data)
{
/*
    
    // Manually create and link tokens
    t_token *ls_cmd = malloc(sizeof(t_token));
    ls_cmd->str = strdup("/bin/ls"); // Replace with actual path to ls
    ls_cmd->type = cmd;
    ls_cmd->prev = NULL;

    t_token *ls_arg = malloc(sizeof(t_token));
    ls_arg->str = strdup("-l");
    ls_arg->type = arg;
    ls_arg->prev = ls_cmd;

    ls_cmd->next = ls_arg;

    t_token *pipe1 = malloc(sizeof(t_token));
    pipe1->str = strdup("|");
    pipe1->type = pipeline;
    pipe1->prev = ls_arg;

    ls_arg->next = pipe1;

    t_token *grep_cmd = malloc(sizeof(t_token));
    grep_cmd->str = strdup("/usr/bin/grep"); // Replace with actual path to grep
    grep_cmd->type = cmd;
    grep_cmd->prev = pipe1;

    pipe1->next = grep_cmd;

    t_token *grep_arg = malloc(sizeof(t_token));
    grep_arg->str = strdup(".txt");
    grep_arg->type = arg;
    grep_arg->prev = grep_cmd;

    grep_cmd->next = grep_arg;

    t_token *pipe2 = malloc(sizeof(t_token));
    pipe2->str = strdup("|");
    pipe2->type = pipeline;
    pipe2->prev = grep_arg;

    grep_arg->next = pipe2;

    t_token *wc_cmd = malloc(sizeof(t_token));
    wc_cmd->str = strdup("/usr/bin/wc"); // Replace with actual path to wc
    wc_cmd->type = cmd;
    wc_cmd->prev = pipe2;

    pipe2->next = wc_cmd;

    t_token *wc_arg = malloc(sizeof(t_token));
    wc_arg->str = strdup("-l");
    wc_arg->type = arg;
    wc_arg->prev = wc_cmd;

    wc_cmd->next = wc_arg;

    t_token *redir_right = malloc(sizeof(t_token));
    redir_right->str = strdup("<");
    redir_right->type = simple_redir_left;
    redir_right->prev = wc_arg;

    wc_arg->next = redir_right;

    t_token *file_name = malloc(sizeof(t_token));
    file_name->str = strdup("file_count.txt");
    file_name->type = arg;
    file_name->prev = redir_right;

    redir_right->next = file_name;
    file_name->next = NULL; // This is the end of the linked list

    data.token = ls_cmd; // Starting point of the linked list

    t_token *curr = data.token;*/
    
   /*
    t_token *cat_cmd = malloc(sizeof(t_token));
    cat_cmd->str = strdup("/bin/cat"); // Replace with actual path to cat
    cat_cmd->type = cmd;
    cat_cmd->prev = NULL;

    t_token *double_redir_lefte = malloc(sizeof(t_token));
    double_redir_lefte->str = strdup("<<");
    double_redir_lefte->type = double_redir_left;
    double_redir_lefte->prev = cat_cmd;

    cat_cmd->next = double_redir_lefte;

    t_token *heredoc_arg = malloc(sizeof(t_token));
    heredoc_arg->str = strdup("EOF\nCeci est la ligne 1.\nCeci est la ligne 2.\nEOF\n");
    heredoc_arg->type = arg;
    heredoc_arg->prev = double_redir_lefte;

    double_redir_lefte->next = heredoc_arg;
    heredoc_arg->next = NULL; // This is the end of the linked list

    data.token = cat_cmd; // Starting point of the linked list

    t_token *curr = data.token;
    */
   
    t_token *curr = data.token;
    
    int i = 0;
    // char sign[MAX_CMD_LEN];

    // if (fgets(sign, MAX_CMD_LEN, stdin) == NULL)
    // {
    //     // L'utilisateur a tapé CTRL+D
    //     printf("Exit <3\n");
    //     exit(0);
    // }
    // sign[strcspn(sign, "\n")] = 0; // Enlever le retour à la ligne

    while (curr)
    {
        printf("cmd id -> [%d] | value -> [%s] | type -> [%d]\n", i, curr->str, curr->type);
        i++;
        curr = curr->next;
    }

    execute_command(data);
}