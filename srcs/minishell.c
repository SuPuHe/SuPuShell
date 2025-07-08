/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/08 18:08:53 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*parse_word(t_input *in);
bool	apply_redirections(t_input *input);

void	do_pwd(void)
{
	char	buf[512];

	getcwd(buf, sizeof(buf));
	printf(BOLDGREEN"%s\n"RESET, buf);
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
			break ;
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
	return (dest);
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
	return (dest);
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

// void	run_external_command(char **argv, t_env *env, t_input *input)
// {
// 	pid_t	pid;
// 	int		status;
// 	char	*path;
// 	char	**envp;
// 	int		i;
// 	int		need_free;

// 	path = NULL;
// 	need_free = 0;
// 	printf("HERE");
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		if (!apply_redirections(input))
// 			exit(EXIT_FAILURE);
// 		envp = build_envp(env);
// 		if (ft_strncmp(argv[0], "./", 2) == 0
// 			|| ft_strncmp(argv[0], "/", 1) == 0)
// 			path = argv[0];
// 		else
// 		{
// 			path = search_path(argv[0], env);
// 			need_free = 1;
// 		}
// 		printf("PATH |%s|\n", path);
// 		if (!path || access(path, X_OK) != 0)
// 		{
// 			printf(BOLDRED"%s: command not found\n"RESET, argv[0]);
// 			if (need_free && path)
// 				free(path);
// 			i = 0;
// 			while (envp[i])
// 				free(envp[i++]);
// 			free(envp);
// 			free_args(argv);
// 			free_env_list(env);
// 			rl_clear_history();
// 			exit(1);
// 		}
// 		printf("PATH |%s|\n", path);
// 		execve(path, argv, envp);
// 		perror("execve");
// 		if (need_free)
// 			free(path);
// 		i = 0;
// 		while (envp[i])
// 			free(envp[i++]);
// 		free(envp);
// 		exit(1);
// 	}
// 	else
// 		waitpid(pid, &status, 0);
// }
void	run_external_command(char **argv, t_env *env)
{
	char	*path;
	char	**envp;

	envp = build_envp(env);
	if (ft_strncmp(argv[0], "./", 2) == 0 || ft_strncmp(argv[0], "/", 1) == 0)
		path = argv[0];
	else
		path = search_path(argv[0], env);

	if (!path || access(path, X_OK) != 0)
	{
		printf(BOLDRED"%s: command not found\n"RESET, argv[0]);
		// while (envp[i])
			// free(envp[i++]);
		exit(127);
	}
	execve(path, argv, envp);
	perror("execve");
	// free_envp(envp);
	exit(1);
}


// void	command_handler(char **argv, t_env **env, t_input *input)
// {
// 	if (!argv || !argv[0])
// 		return ;
// 	if (ft_strncmp(argv[0], "pwd", 4) == 0)
// 		do_pwd();
// 	else if (ft_strncmp(argv[0], "cd", 3) == 0)
// 		do_cd(argv, env);
// 	else if (ft_strncmp(argv[0], "exit", 5) == 0 || ft_strncmp(argv[0], "q", 2) == 0)
// 	{
// 		rl_clear_history();
// 		free_args(argv);
// 		free_env_list(*env);
// 		exit(0);
// 	}
// 	else if (ft_strncmp(argv[0], "env", 4) == 0)
// 		do_env(*env);
// 	else if (ft_strncmp(argv[0], "export", 7) == 0)
// 		do_export(argv, env);
// 	else if (ft_strncmp(argv[0], "unset", 6) == 0)
// 		do_unset(argv, env);
// 	else if (ft_strncmp(argv[0], "echo", 5) == 0)
// 		do_echo(argv);
// 	else
// 		run_external_command(argv, *env, input);
// 		//printf(BOLDRED"I don't know this command: %s\n"RESET, argv[0]);
// }

void	command_handler(char **argv, t_env **env, t_input *input)
{
	if (!argv || !argv[0])
		return ;

	// Выполняем builtin-команды в основном процессе
	if (ft_strncmp(argv[0], "cd", 3) == 0)
		do_cd(argv, env);
	else if (ft_strncmp(argv[0], "export", 7) == 0)
		do_export(argv, env);
	else if (ft_strncmp(argv[0], "unset", 6) == 0)
		do_unset(argv, env);
	else if (ft_strncmp(argv[0], "exit", 5) == 0 || ft_strncmp(argv[0], "q", 2) == 0)
	{
		rl_clear_history();
		free_args(argv);
		free_env_list(*env);
		exit(0);
	}
	else
	{
		// fork только для echo, env и внешних команд
		pid_t pid = fork();
		if (pid == 0)
		{
			// в дочернем процессе: применить редиректы
			if (!apply_redirections(input))
				exit(1);

			// запустить нужную команду
			if (ft_strncmp(argv[0], "echo", 5) == 0)
				do_echo(argv);
			else if (ft_strncmp(argv[0], "pwd", 4) == 0)
				do_pwd();
			else if (ft_strncmp(argv[0], "env", 4) == 0)
				do_env(*env);
			else
				run_external_command(argv, *env);

			// завершаем дочерний процесс
			exit(0);
		}
		else
		{
			// родительский процесс ждёт
			waitpid(pid, NULL, 0);
		}
	}
}


int	check_for_input(char *line)
{
	int	i;
	int	quot;
	int	d_quot;

	i = 0;
	quot = 0;
	d_quot = 0;
	while (line[i])
	{
		if (line[i] == '\\' || line[i] == ';')
			return (printf("Error input\n"), 0);
		else if (line[i] == '"')
			d_quot++;
		else if (line[i] == '\'')
			quot++;
		i++;
	}
	if (d_quot % 2 != 0 || quot % 2 != 0)
		return (printf("Error input\n"), 0);
	return (1);
}


// INPUT with double quotes and sing quotes
// INPUT
// INPUT

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

void	skip_spaces(t_input *input)
{
	while (input->line[input->i]
		&& (input->line[input->i] == ' ' || input->line[input->i] == '\t'))
		input->i++;
}

char	*parse_env_var(t_input *input)
{
	int		start;
	char	*name;
	char	*value;

	start = ++input->i;
	while (input->line[input->i]
		&& (ft_isalnum(input->line[input->i]) || input->line[input->i] == '_'))
		input->i++;
	if (input->i == start)
		return (ft_strdup("$"));
	name = ft_substr(input->line, start, input->i - start);
	value = get_env_value(input->env, name);
	free(name);
	if (value)
		return (ft_strdup(value));
	else
		return (ft_strdup(""));
}

char	*parse_single_quote(t_input *input)
{
	int		start;
	char	*result;

	start = ++input->i;
	while (input->line[input->i] && input->line[input->i] != '\'')
		input->i++;
	if (input->line[input->i] != '\'')
	{
		input->syntax_ok = false;
		return (NULL);
	}
	result = ft_substr(input->line, start, input->i - start);
	input->i++;
	return (result);
}

char	*parse_double_quote(t_input *input)
{
	char	*result;
	int		start;
	char	*tmp;
	char	*joined;

	input->i++;
	result = ft_strdup("");
	while (input->line[input->i] && input->line[input->i] != '"')
	{
		if (input->line[input->i] == '$')
		{
			tmp = parse_env_var(input);
			if (!tmp)
				return (free(result), NULL);
			result = ft_strjoin_free(result, tmp);
		}
		else
		{
			start = input->i;
			while (input->line[input->i] && input->line[input->i] != '$'
				&& input->line[input->i] != '"')
				input->i++;
			tmp = ft_substr(input->line, start, input->i - start);
			if (!tmp)
				return (free(result), NULL);
			joined = ft_strjoin_free(result, tmp);
			free(tmp);
			if (!joined)
				return (NULL);
			result = joined;
		}
	}
	if (input->line[input->i] == '"')
		input->i++;
	return (result);
}

char	*parse_word(t_input *in)
{
	char	*result;
	int		start;
	char	*tmp;
	char	*joined;

	result = ft_strdup("");
	while (in->line[in->i] && !ft_isspace(in->line[in->i])
		&& in->line[in->i] != '\'' && in->line[in->i] != '"')
	{
		if (in->line[in->i] == '$')
		{
			tmp = parse_env_var(in);
			if (!tmp)
				return (free(result), NULL);
			result = ft_strjoin_free(result, tmp);
		}
		else
		{
			start = in->i;
			while (in->line[in->i] && in->line[in->i] != '$'
				&& !ft_isspace(in->line[in->i])
				&& in->line[in->i] != '\'' && in->line[in->i] != '"')
				in->i++;
			tmp = ft_substr(in->line, start, in->i - start);
			if (!tmp)
				return (free(result), NULL);
			joined = ft_strjoin_free(result, tmp);
			free(tmp);
			if (!joined)
				return (NULL);
			result = joined;
		}
	}
	return (result);
}

static int	ft_arrlen(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return (0);
	while (arr[i])
		i++;
	return (i);
}

char	**append_arg(char **args, char *new_arg)
{
	int		len;
	char	**new_args ;
	int		i;

	len = ft_arrlen(args);
	new_args = malloc(sizeof(char *) * (len + 2));
	i = 0;
	if (!new_args)
		return (NULL);
	while (i < len)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[len] = new_arg;
	new_args[len + 1] = NULL;
	free(args);
	return (new_args);
}

bool	parse_redirects(t_input *input)
{
	if (input->line[input->i] == '>' && input->line[input->i + 1] == '>')
	{
		input->i += 2;
		skip_spaces(input);
		input->outfile = parse_word(input);
		input->append = true;
	}
	else if (input->line[input->i] == '>')
	{
		input->i++;
		skip_spaces(input);
		input->outfile = parse_word(input);
		input->append = false;
	}
	else if (input->line[input->i] == '<' && input->line[input->i + 1] == '<')
	{
		input->i += 2;
		skip_spaces(input);
		input->heredoc = parse_word(input);
	}
	else if (input->line[input->i] == '<')
	{
		input->i++;
		skip_spaces(input);
		input->infile = parse_word(input);
		printf("INFILE |%s|\n", input->infile);
	}
	else
	{
		printf("LINE |%s|\n", input->line);
		return (false);
	}
	return (true);
}

bool	apply_redirections(t_input *input)
{
	int	fd;
	int	flags;

	printf("APPLy\n");
	if (input->infile)
	{
		printf ("INFILE\n");
		fd = open(input->infile, O_RDONLY);
		if (fd < 0)
		{
			perror(input->infile);
			return (false);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (input->outfile)
	{
		printf("DAUNFILE\n");
		flags = O_CREAT | O_WRONLY;
		if (input->append)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
		fd = open(input->outfile, flags, 0644);
		if (fd < 0)
		{
			perror(input->outfile);
			return (false);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return (true);
}

t_input	split_input(char *line, t_env *env)
{
	t_input	input;
	t_input	empty;
	char	*arg;

	input.line = line;
	input.i = 0;
	input.env = env;
	input.args = NULL;
	input.syntax_ok = true;
	input.infile = NULL;
	input.outfile = NULL;
	input.heredoc = NULL;
	while (input.line[input.i])
	{
		skip_spaces(&input);
		// if (parse_redirects(&input))
		// 	continue ;
		parse_redirects(&input);
		if (!input.line[input.i])
			break ;
		arg = NULL;
		if (input.line[input.i] == '\'')
			arg = parse_single_quote(&input);
		else if (input.line[input.i] == '"')
			arg = parse_double_quote(&input);
		else
			arg = parse_word(&input);
		if (!arg || !input.syntax_ok)
		{
			// free(arg);
			// free_args(input.args);
			ft_bzero(&empty, sizeof(t_input));
			empty.syntax_ok = false;
			return (empty);
			// return (NULL);

		}
		input.args = append_arg(input.args, arg);
	}
	return (input);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	// char	**commands;
	t_env	*env;
	t_input	input;

	(void)argc;
	(void)argv;
	env = create_env(envp);
	disable_echoctl();
	while (1)
	{
		setup_signal();
		//line = readline(BOLDGREEN"➜  "RESET BOLDCYAN"SuPuShell "RESET BOLDBLUE"git:("RESET BOLDRED"master"RESET BOLDBLUE")"RESET BOLDYELLOW" ✗ "RESET);
		line = readline(SHELLNAME);
		if (!line)
		{
			rl_clear_history();
			free_env_list(env);
			break ;
		}
		if (*line)
			add_history(line);
		if (!check_for_input(line))
		{
			free(line);
			continue ;
		}
		input = split_input(line, env);
		for (int i = 0; input.args[i]; i++)
			printf("|%s|\n", input.args[i]);
		if (!input.syntax_ok)
		{
			free(line);
			continue ;
		}
		command_handler(input.args, &env, &input);
		// free_args(commands);
		free(line);
	}
	return (0);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
