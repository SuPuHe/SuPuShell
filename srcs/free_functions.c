/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_functions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:01:15 by omizin            #+#    #+#             */
/*   Updated: 2025/07/09 19:38:50 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_at_exit(t_input *input, t_env **env)
{
	rl_clear_history();
	free_input(input);
	free_env_list(*env);
}

void	free_input(t_input *input)
{
	if (input->infile)
		free(input->infile);
	if (input->outfile)
		free(input->outfile);
	if (input->heredoc)
	{
		free(input->heredoc);
		unlink("/tmp/.minishell_heredoc");
	}
	if (input->args)
		free_args(input->args);
}

void	free_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_env_list(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = tmp;
	}
}
