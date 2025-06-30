/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/06/30 12:36:42 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	do_pwd(void)
{
	char	buf[512];

	getcwd(buf, sizeof(buf));
	printf(GREEN"%s\n"RESET, buf);
}

void	do_cd(char **commands, t_env **env)
{
	const char	*home = getenv("HOME");
	char		newpwd[512];
	char		oldpwd[512];

	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return ;
	if (ft_strncmp(commands[1], "~", 2) == 0)
		chdir(home);
	else if (commands[1])
	{
		if (chdir(commands[1]) == -1)
			perror(RED"cd"RESET);
	}
	else
		chdir(home);
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
			printf(RED"export: invalid format: %s\n"RESET, argv[i]);
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
			printf(GREEN"%s=%s\n"RESET, env->key, env->value);
		env = env->next;
	}
}

void	command_handler(char **argv, t_env **env)
{
	if (!argv || !argv[0])
		return ;
	if (ft_strncmp(argv[0], "pwd", 4) == 0)
		do_pwd();
	else if (ft_strncmp(argv[0], "cd", 3) == 0)
		do_cd(argv, env);
	else if (ft_strncmp(argv[0], "exit", 5) == 0 || ft_strncmp(argv[0], "q", 2) == 0)
		exit(0);
	else if (ft_strncmp(argv[0], "env", 4) == 0)
		do_env(*env);
	else if (ft_strncmp(argv[0], "export", 7) == 0)
		do_export(argv, env);
	else if (ft_strncmp(argv[0], "unset", 6) == 0)
		do_unset(argv, env);
	else
		printf(RED"I don't know this command: %s\n"RESET, argv[0]);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	char	**commands;
	t_env	*env = create_env(envp);

	(void)argc;
	(void)argv;
	while (1)
	{
		line = readline(CYAN"SuPuShell$ "RESET);
		if (!line)
			break ;
		if (*line)
			add_history(line);
		commands = split_args(line);
		command_handler(commands, &env);
		free_args(commands);
		free(line);
	}
	free_env_list(env);
	return (0);
}
