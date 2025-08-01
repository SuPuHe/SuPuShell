/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:59:51 by omizin            #+#    #+#             */
/*   Updated: 2025/08/01 16:47:02 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Updates or adds an environment variable in the list.
 *
 * If the key exists, updates its value. Otherwise, adds a new variable
 * to the environment list.
 *
 * @param env Pointer to environment variable list.
 * @param key Variable name.
 * @param val Variable value.
 */
void	update_or_add_env_var(t_env **env, char *key, char *val)
{
	t_env	*cur;
	t_env	*new;

	cur = *env;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			cf_free_one(cur->value);
			cur->value = cf_strdup(val);
			return ;
		}
		cur = cur->next;
	}
	new = cf_malloc(sizeof(t_env));
	new->key = cf_strdup(key);
	new->value = cf_strdup(val);
	new->next = NULL;

	// Добавляем в конец списка
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
 * @brief Parses an export argument into key and value.
 *
 * Splits the argument at '=' and sets key and value pointers.
 * Returns 1 on success, 0 if '=' not found.
 *
 * @param arg Argument string to parse.
 * @param key Pointer to key output.
 * @param val Pointer to value output.
 * @return 1 if parsed, 0 if invalid.
 */
int	parse_export_argument(char *arg, char **key, char **val)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (0);
	*eq = '\0';
	*key = arg;
	*val = eq + 1;
	return (1);
}

/**
 * @brief Removes an environment variable from the list by key.
 *
 * Searches for the key and removes the corresponding node from the
 * environment list.
 *
 * @param env Pointer to environment variable list.
 * @param key Variable name to remove.
 */
void	remove_env_var(t_env **env, const char *key)
{
	t_env	*cur;
	t_env	*prev;

	cur = *env;
	prev = NULL;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*env = cur->next;
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}

/**
 * @brief Creates an environment list from envp array.
 *
 * Allocates and fills a linked list of t_env nodes from envp strings.
 * Each node contains key and value.
 *
 * @param envp Array of environment strings.
 * @return Pointer to head of environment list.
 */
t_env	*create_env(char **envp)
{
	t_env	*head;
	t_env	*tail;
	t_env	*node;
	char	*equal;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (envp[i])
	{
		node = cf_malloc(sizeof(t_env));
		equal = ft_strchr(envp[i], '=');
		node->key = substr_dup(envp[i], equal);
		node->value = cf_strdup(equal + 1);
		node->next = NULL;
		if (!head)
		{
			head = node;
			tail = node;
		}
		else
		{
			tail->next = node;
			tail = node;
		}
		i++;
	}
	return (head);
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

	// Первый символ должен быть буквой или подчеркиванием
	if (!ft_isalpha(*name) && *name != '_')
		return (false);

	// Остальные символы должны быть буквами, цифрами или подчеркиванием
	while (*++name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (false);
	}
	return (true);
}
