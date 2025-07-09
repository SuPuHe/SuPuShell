/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:42:14 by omizin            #+#    #+#             */
/*   Updated: 2025/07/09 12:35:20 by omizin           ###   ########.fr       */
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

char	**build_envp(t_env *env)
{
	int		count;
	t_env	*tmp;
	char	**envp;
	int		i;
	char	*entry;

	tmp = env;
	count = 0;
	while (tmp)
	{
		if (tmp->key && tmp->value)
			count++;
		tmp = tmp->next;
	}
	envp = malloc(sizeof(char *) * (count + 1));
	i = 0;
	tmp = env;
	while (tmp)
	{
		if (tmp->key && tmp->value)
		{
			entry = malloc(ft_strlen(tmp->key) + ft_strlen(tmp->value) + 2);
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

char	*ft_strsep(char **str, const char *sep)
{
	char		*start;
	char		*ptr;
	const char	*s;

	if (!str || !*str)
		return (NULL);
	start = *str;
	ptr = start;
	while (*ptr)
	{
		s = sep;
		while (*s)
		{
			if (*ptr == *s)
			{
				*ptr = '\0';
				*str = ptr + 1;
				return (start);
			}
			s++;
		}
		ptr++;
	}
	*str = NULL;
	return (start);
}

char	*search_path(const char *cmd, t_env *env)
{
	char	*path_var;
	char	*paths;
	char	*save;
	char	*token;
	char	full_path[1024];
	char	*result;

	path_var = get_env_value(env, "PATH");
	if (!path_var)
		return (NULL);
	paths = ft_strdup(path_var);
	save = paths;
	token = ft_strsep(&paths, ":");
	while (token != NULL)
	{
		ft_strcpy(full_path, token);
		ft_strcat(full_path, "/");
		ft_strcat(full_path, cmd);
		if (access(full_path, X_OK) == 0)
		{
			result = ft_strdup(full_path);
			free(save);
			return (result);
		}
		token = ft_strsep(&paths, ":");
	}
	free(save);
	return (NULL);
}
