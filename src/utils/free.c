#include "../../includes/minishell.h"
void	free_env(t_env *env)
{
	if (env == NULL)
		return ;
	if (env->key != NULL)
		free(env->key);
	if (env->value != NULL)
		free(env->value);
	free(env);
}

void	free_all_tokens(t_token *token)
{
	if (token == NULL)
		return ;
	free_all_tokens(token->next);
	if (token->str != NULL)
		free(token->str);
	if (token->heredoc_EOF != NULL)
		free(token->heredoc_EOF);
	free(token);
}

void    free_arr(char **arr)
{
    int i;

    i = 0;
    while(arr[i])
    {
		if (arr[i])
        	ft_memdel(arr[i]);
        i++;
    }
	if (arr)
		ft_memdel(arr);
}