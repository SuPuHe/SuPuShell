/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:42:14 by omizin            #+#    #+#             */
/*   Updated: 2025/07/31 10:28:44 by omizin           ###   ########.fr       */
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

void	bubble_sort_env_array(t_env **array, int count)
{
	int		i;
	int		j;
	t_env	*temp;

	if (!array || count <= 1)
		return ;
	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(array[j]->key, array[j + 1]->key) > 0)
			{
				temp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

// Modified to print environment variables in ascending order using bubble sort
t_env	**create_sorted_env_array(t_env *env, int *count)
{
	t_env	**env_array;
	t_env	*cur;
	int		i;

	*count = 0;
	cur = env;
	while (cur)
	{
		(*count)++;
		cur = cur->next;
	}
	if (*count == 0)
		return (NULL);
	env_array = (t_env **)cf_malloc(sizeof(t_env *) * (*count));
	if (!env_array)
		return (NULL);
	i = 0;
	cur = env;
	while (cur)
	{
		env_array[i++] = cur;
		cur = cur->next;
	}
	bubble_sort_env_array(env_array, *count);
	return (env_array);
}
