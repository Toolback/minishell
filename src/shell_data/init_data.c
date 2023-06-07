#include "minishell.h"


int init_data(t_data *data)
{
    data->in = dup(STDIN);
	data->out = dup(STDOUT);
    return (0);
}