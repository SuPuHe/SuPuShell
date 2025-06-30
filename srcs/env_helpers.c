/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:59:51 by omizin            #+#    #+#             */
/*   Updated: 2025/06/30 20:05:54 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_or_add_env_var(t_env **env, char *key, char *val)
{
	t_env *cur;
	t_env *new;

	cur = *env;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			free(cur->value);
			cur->value = ft_strdup(val);
			return ;
		}
		cur = cur->next;
	}
	new = malloc(sizeof(t_env));
	new->key = ft_strdup(key);
	new->value = ft_strdup(val);
	new->next = *env;
	*env = new;
}

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

void	remove_env_var(t_env **env, const char *key)
{
	t_env	*cur = *env;
	t_env	*prev = NULL;

	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*env = cur->next;
			free(cur->key);
			free(cur->value);
			free(cur);
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}

t_env	*create_env(char **envp)
{
	t_env	*head = NULL;
	t_env	*node;
	char	*equal;
	int		i = 0;

	while (envp[i])
	{
		node = malloc(sizeof(t_env));
		equal = ft_strchr(envp[i], '=');
		node->key = substr_dup(envp[i], equal);
		node->value = ft_strdup(equal + 1);
		node->next = head;
		head = node;
		i++;
	}
	return (head);
}
