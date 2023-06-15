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



void super_parser(t_data data)
{
        char *line;
    	ft_putstr_fd("\033[0;36m\033[1mminishell ▸ \033[0m", STDERR);
        if (get_next_line(0, &line) == -2 && (data.exit = 1))
        {
		    ft_putendl_fd("exit by GNL", STDERR);
            exit(0);
        }
        if (sanitise_args(line) != 0)
        {
            free(line);
            ft_putendl_fd("Error : Sanitise Args, wrong args ? ", STDERR);
        }
        if (tokenize(&data, line) != 0)
            ft_putendl_fd("Error : Tokenize ", STDERR);
        // add_history(line);
        t_token *curr = data.token;
        int i = 0;

        while(curr)
        {
            printf("cmd id -> [%d] | value -> [%s] | type -> [%d]\n", i, curr->str, curr->type);
            i++;
            curr = curr->next;
        } 
        curr = data.token;
        while(curr)
        {
            if (curr->type == variable)
            {
                add_new_env(data.env, parse_env_key(curr->str), parse_env_value(curr->str));
            }
            curr = curr->next;
        } 
        t_env *curr2 = data.env;
        while(curr2)
        {
            printf("ENV -> [%s]\n", curr2->get_joined_env(curr2));
            curr2 = curr2->next;
        }
        

}