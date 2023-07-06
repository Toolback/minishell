#include "minishell.h"

char	*get_joined_env(t_env *curr_env)
{
	char		*res;
	char		*temp;
	
	temp = ft_strjoin(curr_env->key, "=");
	res = ft_strjoin(temp, curr_env->value);
	free(temp);
	return (res);
}

char	*parse_env_value(char *env_val)
{
	char *value;
	int 	i;
	int		j;
	int		temp;
	
	i = 0;
	j = 0;
	temp = 0;
	while(env_val && env_val[j] != '=')
		j++;
	j++;
	temp = j;
	while(env_val[j])
	{
		i++;
		j++;
	}
	j = temp;
	value = (char *)malloc(sizeof(char) * i + 1);
	if (!value)
		return NULL;
	i = 0;
	while(env_val[j])
	{
		value[i] = env_val[j];
		i++;
		j++;
	}
	value[i] = '\0';
	return (value);
}

char	*parse_env_key(char *env_val)
{
	char *key;
	int 	i;
	int 	end;

	end = 0;
	i = 0;
	while (env_val && env_val[end] != '=')
		end++;
	key = (char *)malloc(sizeof(char) * (end + 1));
	while(i < end)
	{
		key[i] = env_val[i];
		i++;
	}
	key[i] = '\0';
	return (key);
}


int	init_env(t_data *data, char **local_env, int flag)
{
	t_env	*env;
	t_env	*new;
	int		i;

	if (!local_env | !local_env[0])
	{
		printf("Better to have ENV to start a shell, right ?");
		return (1);
	}
	if (!(env = malloc(sizeof(t_env))))
		return (1);
	env->key = parse_env_key(local_env[0]);
	env->value = parse_env_value(local_env[0]);
	env->get_joined_env = &get_joined_env;
	env->next = NULL;
	if (flag == 1)
		data->env = env;
	else if (flag == 2)
		data->secret_env = env;
	i = 1;
	while (local_env[i])
	{
		if (!(new = malloc(sizeof(t_env))))
			return (1);
		new->key = parse_env_key(local_env[i]);
		new->value = parse_env_value(local_env[i]);
		new->get_joined_env = &get_joined_env;
		new->next = NULL;
		env->next = new;
		env = new;
		i++;
	}
	return (0);
}

