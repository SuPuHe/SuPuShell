/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:39:33 by omizin            #+#    #+#             */
/*   Updated: 2025/07/09 13:40:09 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	do_pwd(void)
{
	char	buf[512];

	getcwd(buf, sizeof(buf));
	printf(BOLDGREEN"%s\n"RESET, buf);
}

void	do_cd(char **commands, t_env **env)
{
	const char	*home = getenv("HOME");
	char		newpwd[512];
	char		oldpwd[512];

	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return ;
	if (!commands[1])
		chdir(home);
	else if (ft_strncmp(commands[1], "~", 2) == 0)
		chdir(home);
	else if (ft_strncmp(commands[1], "-", 2) == 0)
		goto_prev_dir(env);
	else
	{
		if (chdir(commands[1]) == -1)
			perror(BOLDRED"cd"RESET);
	}
	if (getcwd(newpwd, sizeof(newpwd)))
	{
		update_or_add_env_var(env, "OLDPWD", oldpwd);
		update_or_add_env_var(env, "PWD", newpwd);
	}
}

void	do_export(char **argv, t_env **env)
{
	int		i;
	char	*key;
	char	*val;
	char	*tmp;

	i = 1;
	while (argv[i])
	{
		tmp = ft_strdup(argv[i]);
		if (!tmp)
			return ;
		if (!parse_export_argument(tmp, &key, &val))
		{
			printf(BOLDRED"export: invalid format: %s\n"RESET, argv[i]);
			i++;
			continue ;
		}
		update_or_add_env_var(env, key, val);
		free(tmp);
		i++;
	}
}

void	do_unset(char **argv, t_env **env)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		remove_env_var(env, argv[i]);
		i++;
	}
}

void	do_env(t_env *env)
{
	while (env)
	{
		if (env->value)
			printf("%s=%s\n", env->key, env->value);
		env = env->next;
	}
}
