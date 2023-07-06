


#include "minishell.h"


static void		free_node(t_data *data, t_env *env)
{
	if (data->env == env && env->next == NULL)
	{
		ft_memdel(data->env->key);
		ft_memdel(data->env->value);
		data->env->key = NULL;
		data->env->value = NULL;
		data->env->next = NULL;
		return ;
	}
	ft_memdel(env->key);
	ft_memdel(env->value);
	ft_memdel(env);
}

int				ft_unset(char *key, t_data *data)
{
	t_env	*env;
	t_env	*tmp;

	env = data->env;
	if (!(key))
		return (0);
	if (ft_strcmp(key, env->key) == 0)
	{
		data->env = (env->next) ? env->next : data->env;
		free_node(data, env);
		return (0);
	}
	while (env && env->next)
	{
		if (ft_strcmp(key, env->next->key) == 0)
		{
			tmp = env->next->next;
			free_node(data, env->next);
			env->next = tmp;
			t_env *curr2 = env;
			while(curr2)
			{
				ft_printf("ENV -> [%s]\n", curr2->get_joined_env(curr2));
				curr2 = curr2->next;
			}
			return (0);
		}
		env = env->next;
	}
	return (0);
}
