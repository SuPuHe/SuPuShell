/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_helpers_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:44:48 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:47:03 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_new_env_var(t_env **env, char *key, char *val)
{
	t_env	*cur;
	t_env	*new;

	new = cf_malloc(sizeof(t_env));
	new->key = cf_strdup(key);
	new->value = cf_strdup(val);
	new->next = NULL;
	if (!*env)
		*env = new;
	else
	{
		cur = *env;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
}

/**
 * @brief Gets the value of an environment variable by key.
 *
 * Searches the environment list for the key and returns its value,
 * or NULL if not found.
 *
 * @param env Pointer to environment variable list.
 * @param key Variable name to search for.
 * @return Value string, or NULL if not found.
 */
char	*get_env_value(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/**
 * @brief Checks if a string is a valid environment variable name.
 *
 * Valid names start with a letter or underscore and contain only
 * letters, digits, and underscores.
 *
 * @param name String to check.
 * @return true if valid, false otherwise.
 */
bool	is_valid_var_name(const char *name)
{
	if (!name || !*name)
		return (false);
	if (!ft_isalpha(*name) && *name != '_')
		return (false);
	while (*++name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (false);
	}
	return (true);
}
