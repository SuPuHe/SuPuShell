/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/07 12:50:47 by vpushkar         ###   ########.fr       */
=======
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/06 17:48:10 by omizin           ###   ########.fr       */
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	do_pwd(void)
{
	char	buf[512];

	getcwd(buf, sizeof(buf));
	printf(BOLDGREEN"%s\n"RESET, buf);
}

char *get_env_value(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return env->value;
		env = env->next;
	}
	return NULL;
}

void	goto_prev_dir(t_env **env)
{
	char	*old;

	old = get_env_value(*env, "OLDPWD");
	if (old && chdir(old) == 0)
		do_pwd();
	else
		perror(BOLDRED"cd"RESET);
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

<<<<<<< HEAD

=======
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
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

<<<<<<< HEAD

=======
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
void	do_env(t_env *env)
{
	while (env)
	{
		if (env->value)
			printf(BOLDMAGENTA"%s=%s\n"RESET, env->key, env->value);
		env = env->next;
	}
}

void	do_echo(char **argv)
{
	int	i;
	int	j;
	int	new_line_flag;

	i = 1;
	new_line_flag = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n')
	{
		j = 2;
		while (argv[i][j] == 'n')
			j++;
		if (argv[i][j] != '\0')
			break;
		new_line_flag = 0;
		i++;
	}
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
	if (new_line_flag)
		printf("\n");
}

<<<<<<< HEAD
=======
char	*ft_strcpy(char *dest, const char *src)
{
	int	i;

	i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}

char	*ft_strcat(char *dest, const char *src)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (dest[i])
		i++;

	while (src[j])
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';
	return dest;
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
	return envp;
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
	while ((token = ft_strsep(&paths, ":")) != NULL)
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
	}
	free(save);
	return (NULL);
}

void run_external_command(char **argv, t_env *env)
{
	pid_t	pid;
	int		status;
	char	*path;
	char	**envp;
	int		i;

	pid = fork();
	if (pid == 0)
	{
		envp = build_envp(env);
		if (ft_strncmp(argv[0], "./", 2) == 0 || ft_strncmp(argv[0], "/", 1) == 0)
			path = argv[0];
		else
			path = search_path(argv[0], env);

		if (!path || access(path, X_OK) != 0)
		{
			printf(BOLDRED"%s: command not found\n"RESET, argv[0]);
			exit(1);
		}
		execve(path, argv, envp);
		i = 0;
		while (envp[i])
		{
			free(envp[i]);
			i++;
		}
		free(envp);
		perror("execve");
		exit(1);
	}
	else
		waitpid(pid, &status, 0);
}
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607

void	command_handler(char **argv, t_env **env)
{
	if (!argv || !argv[0])
		return ;
	if (ft_strncmp(argv[0], "pwd", 4) == 0)
		do_pwd();
	else if (ft_strncmp(argv[0], "cd", 3) == 0)
		do_cd(argv, env);
	else if (ft_strncmp(argv[0], "exit", 5) == 0 || ft_strncmp(argv[0], "q", 2) == 0)
<<<<<<< HEAD
		exit(0);
=======
	{
		free_env_list(*env);
		exit(0);
	}
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
	else if (ft_strncmp(argv[0], "env", 4) == 0)
		do_env(*env);
	else if (ft_strncmp(argv[0], "export", 7) == 0)
		do_export(argv, env);
	else if (ft_strncmp(argv[0], "unset", 6) == 0)
		do_unset(argv, env);
	else if (ft_strncmp(argv[0], "echo", 5) == 0)
		do_echo(argv);
	else
<<<<<<< HEAD
		printf(BOLDRED"I don't know this command: %s\n"RESET, argv[0]);
=======
		run_external_command(argv, *env);
		//printf(BOLDRED"I don't know this command: %s\n"RESET, argv[0]);
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	char	**commands;
	t_env	*env;

	(void)argc;
	(void)argv;
	env = create_env(envp);
	while (1)
	{
<<<<<<< HEAD
		// line = readline(BOLDCYAN"SuPuShell$ "RESET);
=======
		//line = readline(BOLDCYAN"SuPuShell$ "RESET);
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
		line = readline(BOLDGREEN"➜  "RESET BOLDCYAN"SuPuShell "RESET BOLDBLUE"git:("RESET BOLDRED"master"RESET BOLDBLUE")"RESET BOLDYELLOW" ✗ "RESET);
		if (!line)
			break ;
		if (*line)
			add_history(line);
		commands = split_args(line);
<<<<<<< HEAD
		command_handler(commands, &env);
		free_args(commands);
		free(line);
	}
	free_env_list(env);
=======
		command_handler(commands, &env);;
		free_args(commands);
		free(line);
	}
	// free_env_list(env);
>>>>>>> e232eafeb2de7f04f2f1a007f9aec3b0c80ad607
	return (0);
}
