#include "minishell.h"


int init_data(t_data *data)
{
    data->in = dup(STDIN);
	data->out = dup(STDOUT);
    data->exit = 0;
    data->ret = 0;
    return (0);
}