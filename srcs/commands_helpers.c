/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:42:14 by omizin            #+#    #+#             */
/*   Updated: 2025/07/29 11:54:18 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	goto_prev_dir(t_env **env)
{
	char	*old;

	old = get_env_value(*env, "OLDPWD");
	if (old && chdir(old) == 0)
		do_pwd();
	else
		perror(BOLDRED"cd"RESET);
}

// Helper function to count variables in env
static int	count_env_vars(t_env *env)
{
	int		count;
	t_env	*tmp;

	count = 0;
	tmp = env;
	while (tmp)
	{
		if (tmp->key && tmp->value)
			count++;
		tmp = tmp->next;
	}
	return (count);
}

char	**build_envp(t_env *env)
{
	int		count;
	t_env	*tmp;
	char	**envp;
	int		i;
	char	*entry;

	count = count_env_vars(env);
	envp = cf_malloc(sizeof(char *) * (count + 1));
	i = 0;
	tmp = env;
	while (tmp)
	{
		if (tmp->key && tmp->value)
		{
			entry = cf_malloc(ft_strlen(tmp->key) + ft_strlen(tmp->value) + 2);
			ft_strcpy(entry, tmp->key);
			ft_strcat(entry, "=");
			ft_strcat(entry, tmp->value);
			envp[i++] = entry;
		}
		tmp = tmp->next;
	}
	envp[i] = NULL;
	return (envp);
}
