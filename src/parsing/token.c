#include "minishell.h"

int		next_alloc(char *line, int *i)
{
	int		count;
	int		j;
	char	c;

	count = 0;
	j = 0;
	c = ' ';
	while (line[*i + j] && (line[*i + j] != ' ' || c != ' '))
	{
		if (c == ' ' && (line[*i + j] == '\'' || line[*i + j] == '\"'))
			c = line[*i + j++];
		else if (c != ' ' && line[*i + j] == c)
		{
			count += 2;
			c = ' ';
			j++;
		}
		else
			j++;
		if (line[*i + j - 1] == '\\')
			count--;
	}
	return (j - count + 1);
}

t_token *add_token(char *line, int *i)
{
    t_token *token;
    char    c;
    int     j;

    c = ' ';
    j = 0;
	if (!(token = malloc(sizeof(t_token)))
	|| !(token->str = malloc(sizeof(char) * next_alloc(line, i))))
		return (NULL);
    while (line[*i] && line[*i] != ' ')
	{
		if (c == ' ' && (line[*i] == '\'' || line[*i] == '\"'))
			c = line[(*i)++];
		else if (c != ' ' && line[*i] == c)
		{
			c = ' ';
			(*i)++;
		}
		else if (line[*i] == '\\' && (*i)++)
			token->str[j++] = line[(*i)++];
		else
			token->str[j++] = line[(*i)++];
	}
	token->str[j] = '\0';
	return (token);
}

void	set_token_type(t_token *token)
{
	// if (ft_strcmp(token->str, "") == 0)
	// 	token->type = EMPTY;
	if (ft_strcmp(token->str, ">") == 0)
		token->type = simple_redir_right;
	else if (ft_strcmp(token->str, "<") == 0)
		token->type = simple_redir_left;
	else if (ft_strcmp(token->str, ">>") == 0)
		token->type = double_redir_right;
	else if (ft_strcmp(token->str, "<<") == 0)
		token->type = double_redir_left;
	else if (ft_strcmp(token->str, "|") == 0)
		token->type = pipeline;
	else if (ft_strchr(token->str, '=') != NULL)
		token->type = variable;
	else if (token->str[0] == '-')
		token->type = option_cmd;
	else if (token->prev == NULL || token->prev->type == pipeline)
		token->type = cmd;
	else
		token->type = arg;
}

int tokenize(t_data *data, char *line)
{
    t_token *curr;
    t_token *next;
    int i;

    i = 0;
    curr = add_token(line, &i);
	set_token_type(curr);
    next = NULL;
    data->token = curr;
    while(line[i])
    {
        if(line[i] == ' ')
        {
            i++;
            continue;
        }
        next = add_token(line, &i);
		set_token_type(next);  
        next->prev = curr;
        next->next = NULL;
        curr->next = next;
        curr = next;
    }
    return (0);
}