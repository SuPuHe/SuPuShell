/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/09 13:07:53 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*parse_word(t_input *in);
bool	apply_redirections(t_input *input);

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
	pid_t	pid;

	if (!argv || !argv[0])
		return ;
	if (ft_strncmp(argv[0], "cd", 3) == 0)
		do_cd(argv, env);
	else if (ft_strncmp(argv[0], "export", 7) == 0)
		do_export(argv, env);
	else if (ft_strncmp(argv[0], "unset", 6) == 0)
		do_unset(argv, env);
	else if (ft_strncmp(argv[0], "exit", 5) == 0 || ft_strncmp(argv[0], "q", 2) == 0)
	{
		free_at_exit(input, env);
		exit(0);
	}
	else
	{
		pid = fork();
		if (pid == 0)
		{
			if (!apply_redirections(input))
			{
				rl_clear_history();
				free_args(argv);
				free_env_list(*env);
				exit(1);
			}
			if (ft_strncmp(argv[0], "echo", 5) == 0)
				do_echo(argv);
			else if (ft_strncmp(argv[0], "pwd", 4) == 0)
				do_pwd();
			else if (ft_strncmp(argv[0], "env", 4) == 0)
				do_env(*env);
			else
				run_external_command(argv, *env, input);
			free_at_exit(input, env);
			exit(0);
		}
		else
			waitpid(pid, NULL, 0);
	}
}

// INPUT with double quotes and sing quotes
// INPUT
// INPUT

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
	}
	else
		return (false);
	return (true);
}

bool	apply_redirections(t_input *input)
{
	int	fd;
	int	flags;

	if (input->infile)
	{
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
//	t_input	empty;
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
		// if (!arg || !input.syntax_ok)
		// {
		// 	// free(arg);
		// 	// free_args(input.args);
		// 	ft_bzero(&empty, sizeof(t_input));
		// 	empty.syntax_ok = false;
		// 	return (empty);
		// 	// return (NULL);

		// }
		if (!arg || !input.syntax_ok)
		{
			input.syntax_ok = false;
			free(arg);
			return (input);
		}
		if (arg[0] == '\0')
		{
			free(arg);
			continue;
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
		if (!input.syntax_ok || !input.args)
		{
			free(line);
			free_input(&input);
			printf("ERRR\n");
			continue ;
		}
		for (int i = 0; input.args[i]; i++)
			printf("|%s|\n", input.args[i]);
		command_handler(input.args, &env, &input);
		free_input(&input);
		free(line);
	}
	return (0);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
