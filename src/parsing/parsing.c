# include "minishell.h"

// flag 0 = quote close 
// flag 1 = double quote open
// flag 2 = single quote open
int is_quote_open(char *line, int max)
{
    int i;
    int flag;

    i = 0;
    flag = 0;
    while(line[i] && i < max)
	{
		if (i > 0 && line[i - 1] == '\\')
			;
		else if (flag == 0 && line[i] == '\"')
			flag = 1;
		else if (flag == 0 && line[i] == '\'')
			flag = 2;
		else if (flag == 1 && line[i] == '\"')
			flag = 0;
		else if (flag == 2 && line[i] == '\'')
			flag = 0;
		i++;
	}
    return (flag);
}

int sanitise_args(char *line)
{
    if (is_quote_open(line, MAX_INT) != 0)
        return (1);
    // space_line(line);
    return (0);
}

// FREE LEAKS WIP
char    *get_bin(char *cmd, t_env *env_list)
{
    int     i;
    t_env   *curr_env;
    char    **env_arr;
    char    *binary_path;
    char    *temp;

    i = -1;
    curr_env = get_env_with_key("PATH", env_list);
    env_arr = ft_split(curr_env->value, ':');
    while(env_arr[++i])
    {
        temp = ft_strjoin(env_arr[i], "/");
        binary_path = ft_strjoin(temp, cmd);
        // printf("\nBIN TESTED : [%s]\n", binary_path);
        if(access(binary_path, X_OK) == 0)
            return(binary_path); // we found the correct path for given CMD
    }
    return (NULL);
}

// int is_heredoc_delimiter(char *delimiter, char *line)
// {
//     // here doc delimiter present
//     int i;
//     int j;
//     int res;

//     i = 0;
//     res = 0;
//     while(line[i])
//     {
//         if (res == 0)
//             j = 0;
//         // if(delimiter[j] != '\0')
//         // {
//             if (line[i] == delimiter[j])
//             {
//                 // ft_printf("\nSameCaract J Increment [%d], for i [%d] (is heredoc deliniter)\n",j,i);
//                 res = 1;
//                 j++;
//             }
//             else
//                 res = 0;
//         // }
//         // else  //arrived to end of delimiter, match found
//         if(delimiter[j] == '\0')
//         {
//             // ft_printf("\ndELIMITER FOUND !!!! (is heredoc deliniter)\n");
//             return (1);
//         }
//         i++;
//     }
//     // ft_printf("\nNO DELIMITER FOUND (is heredoc deliniter [%s])\n", delimiter);
//     // no delimiter 
//     return (0);
// }

int handle_heredoc(t_data *data, t_token *curr)
{
    char *line;
    char **args_arr;
    ft_putstr_fd("\033[0;36m\033[1mHereDoc ▸ \033[0m", STDERR);
    if (get_next_line(0, &line) == -2 && (data->exit = 1))
    {
        ft_putendl_fd("exit by GNL", STDERR);
        exit(0); // or return to shell with clear data
    }
    // ft_printf("\nHEREDOC Line Received : [%s]\n", line);
    // if(!line)
    //     return (0);
    curr->str = ft_strjoin(curr->str, "\n");
    curr->str = ft_strjoin(curr->str, line);
    // ft_printf("\nActuel Curr STR : [%s]\n", curr->str);
    // if(is_heredoc_delimiter(curr->heredoc_EOF, line) == 1)
    if(ft_strcmp(curr->heredoc_EOF, line) == 0)
        return 0; //delimiter present, last here doc line
    else
        return 1;
}

int is_builtin_cmd(char *cmd)
{
    // ft_printf("CMD COMPARE : [%s]\n", cmd);
    if (ft_strcmp("echo", cmd) == 0)
        return (1);
    if (ft_strcmp("cd", cmd) == 0)
        return (1);
    if (ft_strcmp("pwd", cmd) == 0)
        return (1);
    if (ft_strcmp("export", cmd) == 0)
        return (1);
    if (ft_strcmp("unset", cmd) == 0)
        return (1);
    if (ft_strcmp("env", cmd) == 0)
        return (1);
    if (ft_strcmp("exit", cmd) == 0)
        return (1);
    return (0);
}

void super_parser(t_data *data)
{
        char *line;
        int     heredoc_open;
        if (get_next_line(0, &line) == -2 && (data->exit = 1))
        {
		    // ft_putendl_fd("exit by GNL", STDERR);
            exit(0);
        }
        if (sanitise_args(line) != 0)
        {
            free(line);
            ft_putendl_fd("Error : Sanitise Args, wrong args ? ", STDERR);
        }
        if (tokenize(data, line) != 0)
            ft_putendl_fd("Error : Tokenize ", STDERR);
        // add_history(line);
        t_token *curr = data->token;
        while(curr)
        {
            if (curr->type == new_variable) // new env received, add it to local env
            {
                if(curr->next == NULL)
                {
                    add_new_env(data->env, parse_env_key(curr->str), parse_env_value(curr->str));
                    delete_token(data, curr);
                    break; // Bash only parse 1 cmd when adding ENV var 
                }
                else
                    delete_token(data, curr); // if more than 1 cmd, just delete curr node and continue;
                    curr = curr->next;
                    set_token_type(curr);
                    continue;
            }
            else if (curr->type == variable) // variable to replace by env in cmd line
            {
                t_env *curr_env = get_env_with_key((curr->str + 1), data->env);
                if (curr_env == NULL) // no env occurance found, replace token variable with empty string 
                {
                    ft_printf("\nNO VARIABLE IN ENV FOUND\n");
                    free(curr->str);
                    curr->str = "";
                }
                else if (ft_strcmp(curr_env->key, (curr->str + 1)) == 0) // env found - token variable replace with env value
                {
                    free(curr->str);
                    curr->str = curr_env->value;

                }
            }
            else if (curr->type == cmd && is_builtin_cmd(curr->str) == 0 && ft_strcmp(curr->str, "") != 0) // bin received (not builtin), check for path
            // else if (curr->type == cmd && ft_strcmp(curr->str, "") != 0) // bin received (not builtin), check for path
            {
                if (ft_strchr(curr->str, '/') == NULL) // bin with no path, fetch ENV paths
                {
                    curr->str = get_bin(curr->str, data->env); //NULL if no path found
                    // if(curr->str != NULL)
                    //     ft_printf("Bin Found : [%s]", curr->str);
                    // else
                    //     ft_printf("No Bin Path Found in ENV");
                    if(curr->str == NULL)
                    {
                        free(curr->str);
                        curr->str = "Path for cmd not Found !";
                    }
                    // ft_printf("\nPATH RETRIEVED :[%s]\n", curr->str);
                }
                if(access(curr->str, X_OK) == -1) // path is not executable
                {
                    free(curr->str);
                    curr->str = "Cmd not found or executable";
                }
            }
                // ft_printf("\nFINAL PATH RETRIEVED :[%s]\n", curr->str);
            else if (curr->type == double_redir_left)
            {
                // WIP : check if end delimiter is in the same line (one line here doc)
                heredoc_open = 1;
                curr = curr->next;
                break;
            }
            curr = curr->next;
        } 
        while(heredoc_open == 1)
            heredoc_open = handle_heredoc(data, curr);


        curr = data->token;
        int i = 0;
        // return; // NATH EST PASSÉ PAR LA tmtc
        // while(curr)
        // {
        //     ft_printf("\ncmd id -> [%d] | value -> [%s] | type -> [%d]\n", i, curr->str, curr->type);
        //     i++;
        //     curr = curr->next;
        // } 
        // t_env *curr2 = data->env;
        // while(curr2)
        // {
        //     ft_printf("ENV -> [%s]\n", curr2->get_joined_env(curr2));
        //     curr2 = curr2->next;
        // }
        

}