/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:42:14 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 18:22:14 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Changes to the previous working directory.
 *
 * Uses OLDPWD from the environment. Prints the new directory or an error
 * if the change fails.
 *
 * @param env Pointer to environment variable list.
 */
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
/**
 * @brief Counts the number of environment variables with values.
 *
 * Iterates through the environment list and counts variables that have
 * both key and value set.
 *
 * @param env Pointer to environment variable list.
 * @return Number of variables with values.
 */
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

/**
 * @brief Builds an array of environment strings for execve.
 *
 * Allocates and fills an array of 'KEY=VALUE' strings for all environment
 * variables with values. Uses centralized allocation for memory management.
 *
 * @param env Pointer to environment variable list.
 * @return Array of environment strings, or NULL on error.
 */
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
