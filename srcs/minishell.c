/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/09 20:18:53 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*parse_word(t_input *in);
bool	apply_redirections(t_input *input);

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
				free_at_exit(input, env);
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

bool	handle_heredoc(t_input *input)
{
	char	*line;
	int		fd;
	char	*filename = "/tmp/.minishell_heredoc"; // или с уникальным суффиксом

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (perror("heredoc file"), false);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, input->heredoc) == 0)
			break;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	input->infile = ft_strdup(filename); // перенаправим stdin позже
	return (true);
}


bool	apply_redirections(t_input *input)
{
	int	fd;
	int	flags;

	if (input->heredoc)
	{
		if (!handle_heredoc(input))
			return (false);
	}
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
		if (parse_redirects(&input))
			continue ;
		// parse_redirects(&input);
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
//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell

void	handle_pipeline(char **pipe_parts, t_env **env)
{
	int		in_fd = STDIN_FILENO;
	int		pipefd[2];
	pid_t	pids[128];
	int		pid_index = 0;
	int		i = 0;

	while (pipe_parts[i])
	{
		t_input input = split_input(pipe_parts[i], *env);
		if (!input.syntax_ok || !input.args)
		{
			free_input(&input);
			i++;
			continue ;
		}

		if (pipe_parts[i + 1] != NULL)
			pipe(pipefd);

		if (!pipe_parts[i + 1] && i == 0 &&
			(ft_strncmp(input.args[0], "cd", 3) == 0 ||
			ft_strncmp(input.args[0], "export", 7) == 0 ||
			ft_strncmp(input.args[0], "unset", 6) == 0 ||
			ft_strncmp(input.args[0], "exit", 5) == 0 ||
			ft_strncmp(input.args[0], "q", 2) == 0))
		{
			if (ft_strncmp(input.args[0], "cd", 3) == 0)
				do_cd(input.args, env);
			else if (ft_strncmp(input.args[0], "export", 7) == 0)
				do_export(input.args, env);
			else if (ft_strncmp(input.args[0], "unset", 6) == 0)
				do_unset(input.args, env);
			else if (ft_strncmp(input.args[0], "exit", 5) == 0 || ft_strncmp(input.args[0], "q", 2) == 0)
			{
				free_at_exit(&input, env);
				exit(0);
			}
			free_input(&input);
			i++;
			continue;
		}

		pid_t pid = fork();
		if (pid == 0)
		{
			if (in_fd != STDIN_FILENO)
			{
				dup2(in_fd, STDIN_FILENO);
				close(in_fd);
			}
			if (pipe_parts[i + 1])
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[0]);
				close(pipefd[1]);
			}

			if (!apply_redirections(&input))
				exit(1);

			if (ft_strncmp(input.args[0], "echo", 5) == 0)
				do_echo(input.args);
			else if (ft_strncmp(input.args[0], "pwd", 4) == 0)
				do_pwd();
			else if (ft_strncmp(input.args[0], "env", 4) == 0)
				do_env(*env);
			else
				run_external_command(input.args, *env, &input);

			free_at_exit(&input, env);
			exit(0);
		}
		else
		{
			pids[pid_index++] = pid;
			if (in_fd != STDIN_FILENO)
				close(in_fd);
			if (pipe_parts[i + 1])
			{
				close(pipefd[1]);
				in_fd = pipefd[0];
			}
			free_input(&input);
		}
		i++;
	}
	int	j = 0;
	while (j < pid_index)
		waitpid(pids[j++], NULL, 0);
}


int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_env	*env;
	char	**many_lines;
	int		i;

	(void)argc;
	(void)argv;
	billy_print();
	env = create_env(envp);
	disable_echoctl();

	while (1)
	{
		setup_signal();
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

		many_lines = ft_split(line, '\n');
		i = 0;
		while (many_lines && many_lines[i])
		{
			if (*many_lines[i])
			{
				char **pipe_parts = ft_split(many_lines[i], '|');
				handle_pipeline(pipe_parts, &env);
				free_args(pipe_parts);
			}
			i++;
		}
		free_args(many_lines);
		free(line);
	}
	return (0);
}

