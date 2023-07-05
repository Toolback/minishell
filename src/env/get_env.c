#include "../../includes/minishell.h"


t_env   *get_env_with_key(char *key, t_env *env_list)
{
    while(env_list && ft_strcmp(key, env_list->key) != 0)
        env_list = env_list->next;
    return (env_list);
}

t_env   *get_last_env(t_env *env_list)
{
    while(env_list->next)
        env_list = env_list->next;
    return (env_list);
}

int		is_valid_env(const char *env)
{
	int		i;

	i = 0;
	if (ft_isdigit(env[i]) == 1)
		return (0);
	while (env[i] && env[i] != '=')
	{
		if (ft_isalnum(env[i]) == 0)
			return (-1);
		i++;
	}
	if (env[i] != '=')
		return (2);
	return (1);
}
