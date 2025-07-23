/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:59:51 by omizin            #+#    #+#             */
/*   Updated: 2025/07/23 17:06:11 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

t_env	*create_env(char **envp)
{
	t_env	*head;
	t_env	*node;
	char	*equal;
	int		i;

	head = NULL;
	i = 0;
	while (envp[i])
	{
		node = cf_malloc(sizeof(t_env));
		equal = ft_strchr(envp[i], '=');
		node->key = substr_dup(envp[i], equal);
		node->value = cf_strdup(equal + 1);
		node->next = head;
		head = node;
		i++;
	}
	return (head);
}

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
