/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/23 17:29:56 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// volatile sig_atomic_t	g_signal_interrupt = 0;
char	*parse_word(t_input *in);
bool	apply_redirections(t_input *input);
char	*parse_redirect_word(t_input *input);

bool	is_operator(char c);
t_ast_node	*parse_expression(t_list **tokens, t_shell *shell);
char	*extract_command_line_from_tokens(t_list **tokens);
void	free_ast(t_ast_node *node);
void	free_token_list(t_list *tokens);
const char	*get_token_type_str(t_token_type type);

// void	command_handler(char **argv, t_env **env, t_input *input)
// {
// 	pid_t	pid;

// 	if (!argv || !argv[0])
// 		return ;
// 	if (ft_strncmp(argv[0], "cd", 3) == 0)
// 		do_cd(argv, env);
// 	else if (ft_strncmp(argv[0], "export", 7) == 0)
// 		do_export(argv, env);
// 	else if (ft_strncmp(argv[0], "unset", 6) == 0)
// 		do_unset(argv, env);
// 	else if (ft_strncmp(argv[0], "exit", 5) == 0 || ft_strncmp(argv[0], "q", 2) == 0)
// 	{
// 		free_at_exit(input, env);
// 		exit(0);
// 	}
// 	else
// 	{
// 		pid = fork();
// 		if (pid == 0)
// 		{
// 			if (!apply_redirections(input))
// 			{
// 				free_at_exit(input, env);
// 				exit(1);
// 			}
// 			if (ft_strncmp(argv[0], "echo", 5) == 0)
// 				do_echo(argv);
// 			else if (ft_strncmp(argv[0], "pwd", 4) == 0)
// 				do_pwd();
// 			else if (ft_strncmp(argv[0], "env", 4) == 0)
// 				do_env(*env);
// 			else
// 				run_external_command(argv, *env, input);
// 			free_at_exit(input, env);
// 			exit(0);
// 		}
// 		else
// 			waitpid(pid, NULL, 0);
// 	}
// }

char	*parse_env_var(t_input *input)
{
	int		start;
	char	*name;
	char	*value;

	start = ++input->i;
	if (input->line[input->i] == '?')
	{
		input->i++;
		return (ft_itoa(input->shell->last_exit_status));
	}
	while (input->line[input->i]
		&& (ft_isalnum(input->line[input->i]) || input->line[input->i] == '_'))
		input->i++;
	if (input->i == start)
	{
		return (cf_strdup("$"));
	}
	name = cf_substr(input->line, start, input->i - start);
	value = get_env_value(input->env, name);
	if (value)
		return (cf_strdup(value));
	else
		return (cf_strdup(""));
}

// char	*parse_single_quote(t_input *input)
// {
// 	int		start;
// 	char	*result;

// 	start = ++input->i;
// 	while (input->line[input->i] && input->line[input->i] != '\'')
// 		input->i++;
// 	if (input->line[input->i] != '\'')
// 	{
// 		input->syntax_ok = false;
// 		return (NULL);
// 	}
// 	result = ft_substr(input->line, start, input->i - start);
// 	input->i++;
// 	return (result);
// }

// char	*parse_double_quote(t_input *input)
// {
// 	char	*result;
// 	int		start;
// 	char	*tmp;
// 	char	*joined;

// 	input->i++;
// 	result = ft_strdup("");
// 	while (input->line[input->i] && input->line[input->i] != '"')
// 	{
// 		if (input->line[input->i] == '$')
// 		{
// 			tmp = parse_env_var(input);
// 			if (!tmp)
// 				return (free(result), NULL);
// 			result = ft_strjoin_free(result, tmp);
// 		}
// 		else
// 		{
// 			start = input->i;
// 			while (input->line[input->i] && input->line[input->i] != '$'
// 				&& input->line[input->i] != '"')
// 				input->i++;
// 			tmp = ft_substr(input->line, start, input->i - start);
// 			if (!tmp)
// 				return (free(result), NULL);
// 			joined = ft_strjoin_free(result, tmp);
// 			free(tmp);
// 			if (!joined)
// 				return (NULL);
// 			result = joined;
// 		}
// 	}
// 	if (input->line[input->i] == '"')
// 		input->i++;
// 	return (result);
// }

// char	*parse_word(t_input *in)
// {
// 	char	*result;
// 	int		start;
// 	char	*tmp;
// 	char	*joined;

// 	result = ft_strdup("");
// 	while (in->line[in->i] && !ft_isspace(in->line[in->i])
// 		&& in->line[in->i] != '\'' && in->line[in->i] != '"')
// 	{
// 		if (in->line[in->i] == '$')
// 		{
// 			tmp = parse_env_var(in);
// 			if (!tmp)
// 				return (free(result), NULL);
// 			result = ft_strjoin_free(result, tmp);
// 		}
// 		else
// 		{
// 			start = in->i;
// 			while (in->line[in->i] && in->line[in->i] != '$'
// 				&& !ft_isspace(in->line[in->i])
// 				&& in->line[in->i] != '\'' && in->line[in->i] != '"')
// 				in->i++;
// 			tmp = ft_substr(in->line, start, in->i - start);
// 			if (!tmp)
// 				return (free(result), NULL);
// 			joined = ft_strjoin_free(result, tmp);
// 			free(tmp);
// 			if (!joined)
// 				return (NULL);
// 			result = joined;
// 		}
// 	}
// 	return (result);
// }

// bool	parse_redirects(t_input *input)
// {
// 	char next_char;
// 	if (input->line[input->i] == '>')
// 	{
// 		if (input->line[input->i + 1] == '>')
// 		{
// 			input->i += 2;
// 			skip_spaces(input);
// 			next_char = input->line[input->i];
// 			if (!next_char || next_char == '>' || next_char == '<')
// 			{
// 				input->syntax_ok = false;
// 				if (!next_char)
// 					printf("syntax error near unexpected token '\\n'\n");
// 				else
// 					printf("syntax error near unexpected token '%c'\n", next_char);
// 				return false;
// 			}
// 			input->outfile = parse_redirect_word(input);
// 			input->append = true;
// 		}
// 		else
// 		{
// 			input->i++;
// 			skip_spaces(input);
// 			next_char = input->line[input->i];
// 			if (!next_char || next_char == '>' || next_char == '<')
// 			{
// 				input->syntax_ok = false;
// 				if (!next_char)
// 					printf("syntax error near unexpected token '\\n'\n");
// 				else
// 					printf("syntax error near unexpected token '%c'\n", next_char);
// 				return false;
// 			}
// 			input->outfile = parse_redirect_word(input);
// 			input->append = false;
// 		}
// 	}
// 	else if (input->line[input->i] == '<')
// 	{
// 		if (input->line[input->i + 1] == '<')
// 		{
// 			input->i += 2;
// 			skip_spaces(input);
// 			next_char = input->line[input->i];
// 			if (!next_char || next_char == '>' || next_char == '<')
// 			{
// 				input->syntax_ok = false;
// 				if (!next_char)
// 					printf("syntax error near unexpected token '\\n'\n");
// 				else
// 					printf("syntax error near unexpected token '%c'\n", next_char);
// 				return false;
// 			}
// 			input->heredoc = parse_redirect_word(input);
// 		}
// 		else
// 		{
// 			input->i++;
// 			skip_spaces(input);
// 			next_char = input->line[input->i];
// 			if (!next_char || next_char == '>' || next_char == '<')
// 			{
// 				input->syntax_ok = false;
// 				if (!next_char)
// 					printf("syntax error near unexpected token '\\n'\n");
// 				else
// 					printf("syntax error near unexpected token '%c'\n", next_char);
// 				return false;
// 			}
// 			input->infile = parse_redirect_word(input);
// 		}
// 	}
// 	else
// 		return false;
// 	return true;
// }

bool	handle_heredoc(t_input *input)
{
	char	*line;
	int		fd;
	char	*filename = "/tmp/.minishell_heredoc";

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
	input->infile = cf_strdup(filename);
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

// t_input	split_input(char *line, t_env *env, t_shell *shell)
// {
// 	t_input	input;
// 	char	*arg;

// 	input.line = line;
// 	input.i = 0;
// 	input.env = env;
// 	input.args = NULL;
// 	input.syntax_ok = true;
// 	input.infile = NULL;
// 	input.outfile = NULL;
// 	input.heredoc = NULL;
// 	input.shell = shell;
// 	while (input.line[input.i])
// 	{
// 		skip_spaces(&input);
// 		if (parse_redirects(&input))
// 			continue ;
// 		if (!input.line[input.i])
// 			break ;
// 		arg = NULL;
// 		if (input.line[input.i] == '\'')
// 			arg = parse_single_quote(&input);
// 		else if (input.line[input.i] == '"')
// 			arg = parse_double_quote(&input);
// 		else
// 			arg = parse_word(&input);
// 		if (!arg || !input.syntax_ok)
// 		{
// 			input.syntax_ok = false;
// 			free(arg);
// 			return (input);
// 		}
// 		if (arg[0] == '\0')
// 		{
// 			free(arg);
// 			continue;
// 		}
// 		input.args = append_arg(input.args, arg);
// 	}
// 	if ((!input.args || !input.args[0]) && input.heredoc)
// 		{
// 			input.args = malloc(sizeof(char *) * 2);
// 			if (!input.args)
// 				return (input.syntax_ok = false, input);
// 			input.args[0] = ft_strdup(":");
// 			input.args[1] = NULL;
// 		}
// 	return (input);
// }

// char	*parse_redirect_word(t_input *input)
// {
// 	if (input->line[input->i] == '\'')
// 		return (parse_single_quote(input));
// 	else if (input->line[input->i] == '"')
// 		return (parse_double_quote(input));
// 	else
// 		return (parse_word(input));
// }
//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell

// void	handle_pipeline(char **pipe_parts, t_shell *shell, char **many_lines)
// {
// 	int		in_fd = STDIN_FILENO;
// 	int		pipefd[2];
// 	pid_t	pids[128];
// 	int		pid_index = 0;
// 	int		i = 0;
// 	int		status;

// 	while (pipe_parts[i])
// 	{
// 		t_input input = split_input(pipe_parts[i], shell->env, shell);
// 		if (!input.syntax_ok || !input.args)
// 		{
// 			free_input(&input);
// 			i++;
// 			continue ;
// 		}

// 		if (pipe_parts[i + 1] != NULL)
// 			pipe(pipefd);

// 		if (!pipe_parts[i + 1] && i == 0 &&
// 			(ft_strncmp(input.args[0], "cd", 3) == 0 ||
// 			ft_strncmp(input.args[0], "export", 7) == 0 ||
// 			ft_strncmp(input.args[0], "unset", 6) == 0 ||
// 			ft_strncmp(input.args[0], "exit", 5) == 0 ||
// 			ft_strncmp(input.args[0], "q", 2) == 0))
// 		{
// 			if (ft_strncmp(input.args[0], "cd", 3) == 0)
// 				do_cd(input.args, &shell->env);
// 			else if (ft_strncmp(input.args[0], "export", 7) == 0)
// 				do_export(input.args, &shell->env);
// 			else if (ft_strncmp(input.args[0], "unset", 6) == 0)
// 				do_unset(input.args, &shell->env);
// 			else if (ft_strncmp(input.args[0], "exit", 5) == 0 || ft_strncmp(input.args[0], "q", 2) == 0)
// 			{
// 				free_args(many_lines);
// 				free_args(pipe_parts);
// 				free_at_exit(&input, &shell->env);
// 				exit(0);
// 			}
// 			free_input(&input);
// 			i++;
// 			continue;
// 		}

// 		pid_t pid = fork();
// 		if (pid == 0)
// 		{
// 			signal(SIGINT, SIG_DFL);
// 			signal(SIGQUIT, SIG_DFL);
// 			if (in_fd != STDIN_FILENO)
// 			{
// 				dup2(in_fd, STDIN_FILENO);
// 				close(in_fd);
// 			}
// 			if (pipe_parts[i + 1])
// 			{
// 				dup2(pipefd[1], STDOUT_FILENO);
// 				close(pipefd[0]);
// 				close(pipefd[1]);
// 			}

// 			if (!apply_redirections(&input))
// 				exit(1);

// 			if (ft_strncmp(input.args[0], "echo", 5) == 0)
// 				do_echo(input.args);
// 			else if (ft_strncmp(input.args[0], "pwd", 4) == 0)
// 				do_pwd();
// 			else if (ft_strncmp(input.args[0], "env", 4) == 0)
// 				do_env(shell->env);
// 			else if (ft_strncmp(input.args[0], ":", 2) == 0)
// 				;
// 			else
// 			{
// 				free_args(many_lines);
// 				free_args(pipe_parts);
// 				run_external_command(input.args, shell->env, &input);
// 			}
// 			free_args(many_lines);
// 			free_args(pipe_parts);
// 			free_at_exit(&input, &shell->env);
// 			exit(0);
// 		}
// 		else
// 		{
// 			signal(SIGINT, SIG_IGN);
// 			signal(SIGQUIT, SIG_IGN);
// 			pids[pid_index++] = pid;
// 			if (in_fd != STDIN_FILENO)
// 				close(in_fd);
// 			if (pipe_parts[i + 1])
// 			{
// 				close(pipefd[1]);
// 				in_fd = pipefd[0];
// 			}
// 			free_input(&input);
// 		}
// 		i++;
// 	}
// 	int	j = 0;
// 	while (j < pid_index)
// 		waitpid(pids[j++], &status, 0);
// 	if (WIFEXITED(status))
// 		shell->last_exit_status = WEXITSTATUS(status);
// 	else if (WIFSIGNALED(status))
// 		shell->last_exit_status = 128 + WTERMSIG(status);
// }

void	*ft_realloc(void *ptr, size_t new_size)
{
	void	*new_ptr;
	size_t	old_content_len;

	if (ptr == NULL)
	{
		return (malloc(new_size));
	}

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}

	new_ptr = malloc(new_size);
	if (new_ptr == NULL)
	{
		return (NULL);
	}
	old_content_len = ft_strlen((char *)ptr) + 1;

	if (old_content_len < new_size)
	{
		ft_memcpy(new_ptr, ptr, old_content_len);
	}
	else
	{
		ft_memcpy(new_ptr, ptr, new_size);
	}
	free(ptr);
	return (new_ptr);
}

t_string_builder	*sb_create(void)
{
	t_string_builder	*sb;

	sb = cf_malloc(sizeof(t_string_builder));
	if (!sb)
		return (NULL);
	sb->str = cf_strdup("");
	if (!sb->str)
	{
		cf_free_one(sb);
		return (NULL);
	}
	sb->len = 0;
	sb->capacity = 1;
	return (sb);
}

void	sb_append_char(t_string_builder *sb, char c)
{
	size_t	new_capacity;
	char	*new_str;

	if (sb->len + 1 >= sb->capacity)
	{
		new_capacity = sb->capacity * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
			return ;
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	sb->str[sb->len++] = c;
	sb->str[sb->len] = '\0';
}

void	sb_append(t_string_builder *sb, const char *s)
{
	size_t	s_len;
	size_t	new_capacity;
	char	*new_str;

	s_len = ft_strlen(s);
	if (!s)
		return ;
	if (sb->len + s_len >= sb->capacity)
	{
		new_capacity = (sb->len + s_len) * 2;
		new_str = cf_realloc(sb->str, new_capacity);
		if (!new_str)
			return ;
		sb->str = new_str;
		sb->capacity = new_capacity;
	}
	ft_strcpy(sb->str + sb->len, s);
	sb->len += s_len;
}

char	*sb_build_and_destroy(t_string_builder *sb)
{
	char	*final_str;

	final_str = sb->str;
	if (!sb)
		return (NULL);
	cf_free_one(sb);
	return (final_str);
}

void	free_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->left)
		free_ast(node->left);
	if (node->right)
		free_ast(node->right);
	if (node->type == NODE_CMD && node->command)
	{
		cf_free_one(node->command);
	}
	cf_free_one(node);
}

void	free_token_list(t_list *tokens)
{
	t_list *current;
	t_token *token;
	while (tokens)
	{
		current = tokens;
		tokens = tokens->next;
		token = (t_token *)current->content;
		if (token && token->value)
			cf_free_one(token->value);
		if (token)
			cf_free_one(token);
		current->next = NULL;
		cf_free_one(current);
	}
}

char	*extract_command_line_from_tokens(t_list **tokens)
{
	t_list	*current_token_node = *tokens;
	char	*command_line_part = cf_strdup("");
	char	*tmp_str;

	while (current_token_node && ((t_token*)current_token_node->content)->type != TOKEN_PIPE
			&& ((t_token*)current_token_node->content)->type != TOKEN_AND
			&& ((t_token*)current_token_node->content)->type != TOKEN_OR
			&& ((t_token*)current_token_node->content)->type != TOKEN_RPAREN
			&& ((t_token*)current_token_node->content)->type != TOKEN_END)
	{
		t_token *token = (t_token *)current_token_node->content;
		tmp_str = NULL;

		if (token->type == TOKEN_WORD)
			tmp_str = ft_strjoin_free(command_line_part, token->value);
		else if (token->type == TOKEN_REDIR_IN)
			tmp_str = ft_strjoin_free(command_line_part, " <");
		else if (token->type == TOKEN_REDIR_OUT)
			tmp_str = ft_strjoin_free(command_line_part, " >");
		else if (token->type == TOKEN_REDIR_APPEND)
			tmp_str = ft_strjoin_free(command_line_part, " >>");
		else if (token->type == TOKEN_HEREDOC)
			tmp_str = ft_strjoin_free(command_line_part, " <<");

		if (command_line_part[0] != '\0' && tmp_str && token->type == TOKEN_WORD)
		{
			char *with_space = ft_strjoin_free(command_line_part, " ");
			cf_free_one(command_line_part);
			command_line_part = ft_strjoin_free(with_space, token->value);
			cf_free_one(with_space);
		}
		else
		{
			if ((token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT ||
				token->type == TOKEN_REDIR_APPEND || token->type == TOKEN_HEREDOC) && command_line_part[0] != '\0')
			{
				char *temp_concat = ft_strjoin_free(command_line_part, " ");
				cf_free_one(command_line_part);
				char *val_to_add;
				if (token->value)
					val_to_add = token->value;
				else if (token->type == TOKEN_REDIR_IN)
					val_to_add = "<";
				else if (token->type == TOKEN_REDIR_OUT)
					val_to_add = ">";
				else if (token->type == TOKEN_REDIR_APPEND)
					val_to_add = ">>";
				else
					val_to_add = "<<";
				command_line_part = ft_strjoin_free(temp_concat, val_to_add);
				cf_free_one(temp_concat);
			}
			else
			{
				char *val_to_add;
				if (token->value)
					val_to_add = token->value;
				else if (token->type == TOKEN_REDIR_IN)
					val_to_add = "<";
				else if (token->type == TOKEN_REDIR_OUT)
					val_to_add = ">";
				else if (token->type == TOKEN_REDIR_APPEND)
					val_to_add = ">>";
				else
					val_to_add = "<<";
				char *new_str = ft_strjoin_free(command_line_part, val_to_add);
				cf_free_one(command_line_part);
				command_line_part = new_str;
			}
		}
		*tokens = current_token_node->next;
		current_token_node = *tokens;
	}
	return (command_line_part);
}

t_token	*create_token(t_token_type type, const char *value)
{
	t_token	*token;

	token = cf_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = cf_strdup(value);
	else
		token->value = NULL;
	return (token);
}

static char *extract_subsegment(const char *line, int *i, char delimiter)
{
	int	start;
	start = *i;
	while (line[*i] && line[*i] != delimiter)
		(*i)++;
	char *s = cf_substr(line, start, *i - start);
	return s;
}

char	*extract_non_quoted_word(const char *line, int *i)
{
	int	start;
	start = *i;
	while (line[*i] && !ft_isspace(line[*i]) && !is_operator(line[*i]) && line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	char *s = cf_substr(line, start, *i - start);
	return s;
}

t_list	*tokenize(const char *line)
{
	t_list	*tokens = NULL;
	int		i = 0;
	bool	had_space = false;

	while (line[i])
	{
		had_space = false;
		while (ft_isspace(line[i]))
		{
			had_space = true;
			i++;
		}

		if (!line[i])
			break;

		t_token *new_token = NULL;
		if (ft_strncmp(&line[i], "&&", 2) == 0)
		{
			new_token = create_token(TOKEN_AND, NULL);
			i += 2;
		}
		else if (ft_strncmp(&line[i], "||", 2) == 0)
		{
			new_token = create_token(TOKEN_OR, NULL);
			i += 2;
		}
		else if (line[i] == '|')
		{
			new_token = create_token(TOKEN_PIPE, NULL);
			i++;
		}
		else if (line[i] == '(')
		{
			new_token = create_token(TOKEN_LPAREN, NULL);
			i++;
		}
		else if (line[i] == ')')
		{
			new_token = create_token(TOKEN_RPAREN, NULL);
			i++;
		}
		else if (ft_strncmp(&line[i], ">>", 2) == 0)
		{
			new_token = create_token(TOKEN_REDIR_APPEND, NULL);
			i += 2;
		}
		else if (ft_strncmp(&line[i], "<<", 2) == 0)
		{
			new_token = create_token(TOKEN_HEREDOC, NULL);
			i += 2;
		}
		else if (line[i] == '>')
		{
			new_token = create_token(TOKEN_REDIR_OUT, NULL);
			i++;
		}
		else if (line[i] == '<')
		{
			new_token = create_token(TOKEN_REDIR_IN, NULL);
			i++;
		}
		else if (line[i] == '\'')
		{
			i++;
			char *value = extract_subsegment(line, &i, '\'');
			if (line[i] == '\'')
				i++;
			else
			{
				free_token_list(tokens);
				return (NULL);
			}
			new_token = create_token(TOKEN_SINGLE_QUOTE_WORD, value);
		}
		else if (line[i] == '"')
		{
			i++;
			char *value = extract_subsegment(line, &i, '"');
			if (line[i] == '"')
				i++;
			else
			{
				free_token_list(tokens);
				return (NULL);
			}
			new_token = create_token(TOKEN_DOUBLE_QUOTE_WORD, value);
		}
		else
		{
			char *word_str = extract_non_quoted_word(line, &i);
			new_token = create_token(TOKEN_WORD, word_str);
		}
		if (new_token)
		{
			new_token->has_space = had_space;
			t_list *node = cf_lstnew(new_token);
			ft_lstadd_back(&tokens, node);
		}
	}
	t_token *end_token = create_token(TOKEN_END, NULL);
	end_token->has_space = false;
	t_list *end_node = cf_lstnew(end_token);
	ft_lstadd_back(&tokens, end_node);
	return (tokens);
}

char	*expand_string_variables(const char *str, t_env *env, t_shell *shell)
{
	t_string_builder	*sb = sb_create();
	if (!sb)
		return (NULL);
	int i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			i++;
			if (str[i] == '?')
			{
				char *status_str = ft_itoa(shell->last_exit_status);
				sb_append(sb, status_str);
				free(status_str);
				i++;
			}
			else if (ft_isdigit(str[i]))
			{
				if (str[i] == '0')
					sb_append(sb, "SuPuShell");
				i++;
			}
			else if (str[i] == '_' || ft_isalpha(str[i]))
			{
				int var_start = i;
				while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
					i++;
				char *var_name = cf_substr(str, var_start, i - var_start);
				char *var_value = get_env_value(env, var_name);
				if (var_value)
					sb_append(sb, var_value);
			}
			else
			{
				sb_append_char(sb, '$');
			}
		}
		else
		{
			sb_append_char(sb, str[i]);
			i++;
		}
	}
	return (sb_build_and_destroy(sb));
}

char	*expand_token_value(t_token *token, t_env *env, t_shell *shell)
{
	if (!token || !token->value)
		return (cf_strdup(""));

	if (token->type == TOKEN_SINGLE_QUOTE_WORD)
	{
		return (cf_strdup(token->value));
	}
	else if (token->type == TOKEN_DOUBLE_QUOTE_WORD)
	{
		return (expand_string_variables(token->value, env, shell));
	}
	else if (token->type == TOKEN_WORD)
	{
		return (expand_string_variables(token->value, env, shell));
	}
	return (cf_strdup(""));
}

bool	is_operator(char c)
{
	return (c == '|' || c == '&' || c == '(' || c == ')' || c == '<' || c == '>');
}

t_ast_node	*create_cmd_node(t_input *cmd)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	node->command = cmd;
	return (node);
}

t_ast_node	*create_binary_node(t_node_type type, t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->left = left;
	node->right = right;
	node->command = NULL;
	return (node);
}

t_ast_node	*create_subshell_node(t_ast_node *child)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return NULL;
	node->type = NODE_SUBSHELL;
	node->left = child;
	node->right = NULL;
	node->command = NULL;
	return (node);
}

t_input	parse_command_from_tokens(t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_input	input;
	t_token	*current_tok;
	char	*expanded_value;

	ft_memset(&input, 0, sizeof(t_input));
	input.env = env;
	input.syntax_ok = true;
	input.shell = shell;

	while (*current_tokens &&
			((t_token*)(*current_tokens)->content)->type != TOKEN_PIPE &&
			((t_token*)(*current_tokens)->content)->type != TOKEN_AND &&
			((t_token*)(*current_tokens)->content)->type != TOKEN_OR &&
			((t_token*)(*current_tokens)->content)->type != TOKEN_RPAREN &&
			((t_token*)(*current_tokens)->content)->type != TOKEN_END)
	{
		current_tok = (t_token*)(*current_tokens)->content;
		if (current_tok->type >= TOKEN_REDIR_IN && current_tok->type <= TOKEN_HEREDOC)
		{
			t_token_type redir_type = current_tok->type;
			*current_tokens = (*current_tokens)->next;

			if (!*current_tokens)
			{
				input.syntax_ok = false;
				free_token_list(*current_tokens);
				return (input);
			}
			t_token *filename_token = (t_token*)(*current_tokens)->content;

			if (redir_type == TOKEN_HEREDOC)
			{
				if (input.heredoc)
					cf_free_one(input.heredoc);
				input.heredoc = cf_strdup(filename_token->value);
			}
			else if (filename_token->type == TOKEN_SINGLE_QUOTE_WORD)
				expanded_value = cf_strdup(filename_token->value);
			else
				expanded_value = expand_string_variables(filename_token->value, env, shell);

			if (redir_type == TOKEN_REDIR_OUT)
			{
				if (input.outfile)
					cf_free_one(input.outfile);
				input.outfile = expanded_value;
				input.append = false;
			}
			else if (redir_type == TOKEN_REDIR_APPEND)
			{
				if (input.outfile)
					cf_free_one(input.outfile);
				input.outfile = expanded_value;
				input.append = true;
			}
			else if (redir_type == TOKEN_REDIR_IN)
			{
				if (input.infile)
					cf_free_one(input.infile);
				input.infile = expanded_value;
			}
			*current_tokens = (*current_tokens)->next;
			continue;
		}
		else if (current_tok->type == TOKEN_WORD ||
			current_tok->type == TOKEN_SINGLE_QUOTE_WORD ||
			current_tok->type == TOKEN_DOUBLE_QUOTE_WORD)
		{
			expanded_value = expand_token_value(current_tok, env, shell);
			if (!expanded_value)
			{
				input.syntax_ok = false;
				free_token_list(*current_tokens);
				return (input);
			}

			t_list *next_token = (*current_tokens)->next;
			while (next_token &&
					!((t_token*)next_token->content)->has_space &&
					(((t_token*)next_token->content)->type == TOKEN_SINGLE_QUOTE_WORD ||
					((t_token*)next_token->content)->type == TOKEN_DOUBLE_QUOTE_WORD ||
					((t_token*)next_token->content)->type == TOKEN_WORD))
			{
				char *next_expanded = expand_token_value((t_token*)next_token->content, env, shell);
				if (next_expanded)
				{
					char *combined = ft_strjoin_free(expanded_value, next_expanded);
					cf_free_one(expanded_value);
					cf_free_one(next_expanded);
					expanded_value = combined;
					*current_tokens = next_token;
					next_token = next_token->next;
				}
				else
					break;
			}

			if (ft_strlen(expanded_value) > 0)
				input.args = append_arg(input.args, expanded_value);
			else
				cf_free_one(expanded_value);
			*current_tokens = (*current_tokens)->next;
		}
		else
		{
			input.syntax_ok = false;
			free_token_list(*current_tokens);
			return (input);
		}
	}
	if ((!input.args || !input.args[0]) && input.heredoc)
	{
		input.args = cf_malloc(sizeof(char *) * 2);
		if (!input.args)
		{
			free_token_list(*current_tokens);
			return (input.syntax_ok = false, input);
		}
		input.args[0] = cf_strdup(":");
		input.args[1] = NULL;
	}
	return (input);
}

t_ast_node	*parse_primary(t_list **tokens, t_shell *shell)
{
	t_token		*current_token;
	t_ast_node	*node = NULL;
	t_input		*cmd_data = NULL;

	if (!*tokens)
		return (NULL);

	current_token = (t_token*)(*tokens)->content;

	if (current_token->type == TOKEN_LPAREN)
	{
		*tokens = (*tokens)->next;

		t_ast_node *sub_expr = parse_expression(tokens, shell);
		if (!sub_expr)
			return (NULL);

		if (!*tokens || ((t_token*)(*tokens)->content)->type != TOKEN_RPAREN)
		{
			shell->last_exit_status = 2;
			free_ast(sub_expr);
			return (NULL);
		}

		*tokens = (*tokens)->next;
		node = create_subshell_node(sub_expr);
	}
	else if (current_token->type == TOKEN_WORD ||
			current_token->type == TOKEN_REDIR_IN ||
			current_token->type == TOKEN_REDIR_OUT ||
			current_token->type == TOKEN_REDIR_APPEND ||
			current_token->type == TOKEN_HEREDOC)
	{
		cmd_data = cf_malloc(sizeof(t_input));
		if (!cmd_data)
			return (NULL);

		ft_memset(cmd_data, 0, sizeof(t_input));

		*cmd_data = parse_command_from_tokens(tokens, shell->env, shell);

		if (!cmd_data->syntax_ok)
		{
			cf_free_one(cmd_data);
			shell->last_exit_status = 2;
			return (NULL);
		}
		node = create_cmd_node(cmd_data);
	}
	else
	{
		shell->last_exit_status = 0;
		return (NULL);
	}
	return (node);
}

const char	*get_token_type_str(t_token_type type)
{
	if (type == TOKEN_PIPE)
		return ("|");
	if (type == TOKEN_AND)
		return ("&&");
	if (type == TOKEN_OR)
		return ("||");
	if (type == TOKEN_LPAREN)
		return ("(");
	if (type == TOKEN_RPAREN)
		return (")");
	if (type == TOKEN_REDIR_IN)
		return ("<");
	if (type == TOKEN_REDIR_OUT)
		return (">");
	if (type == TOKEN_REDIR_APPEND)
		return (">>");
	if (type == TOKEN_HEREDOC)
		return ("<<");
	if (type == TOKEN_END)
		return ("newline");
	return ("unknown");
}

t_ast_node	*parse_pipeline(t_list **tokens, t_shell *shell)
{
	t_ast_node	*left = parse_primary(tokens, shell);
	if (!left)
		return NULL;

	while ((*tokens)->content && ((t_token*)(*tokens)->content)->type == TOKEN_PIPE)
	{
		*tokens = (*tokens)->next;
		t_ast_node *right = parse_primary(tokens, shell);
		if (!right)
			return (NULL);
		left = create_binary_node(NODE_PIPE, left, right);
	}
	return (left);
}

t_ast_node	*parse_and_or(t_list **tokens, t_shell *shell)
{
	t_ast_node	*left = parse_pipeline(tokens, shell);
	if (!left)
		return (NULL);

	while ((*tokens)->content && (((t_token*)(*tokens)->content)->type == TOKEN_AND || ((t_token*)(*tokens)->content)->type == TOKEN_OR))
	{
		t_token_type op_type = ((t_token*)(*tokens)->content)->type;
		*tokens = (*tokens)->next;
		t_ast_node *right = parse_pipeline(tokens, shell);
		if (!right)
			return NULL;
		t_node_type node_type;
		if (op_type == TOKEN_AND)
			node_type = NODE_AND;
		else
			node_type = NODE_OR;
		left = create_binary_node(node_type, left, right);
	}
	return (left);
}

t_ast_node	*parse_expression(t_list **tokens, t_shell *shell)
{
	return (parse_and_or(tokens, shell));
}

t_ast_node	*parse(const char *line, t_shell *shell)
{
	t_list		*tokens;
	// t_list		*tokens_head;
	t_ast_node	*ast;

	tokens = tokenize(line);
	if (!tokens)
		return (NULL);
	// tokens_head = tokens;
	ast = parse_expression(&tokens, shell);

	if (ast && ((t_token*)tokens->content)->type == TOKEN_END)
		;
	else
	{
		free_ast(ast);
		ast = NULL;
	}
	if (tokens)
		free_token_list(tokens);
	tokens = NULL;
	return (ast);
}

int	execute_node(t_ast_node *node, t_shell *shell)
{
	int		last_status = 0;
	pid_t	pid;
	int		status;
	int		original_stdin = dup(STDIN_FILENO);
	int		original_stdout = dup(STDOUT_FILENO);

	if (!node)
		return (1);

	if (node->type == NODE_CMD)
	{
		if (!apply_redirections(node->command))
			last_status = 1;
		else if (node->command->args && node->command->args[0])
		{
			if (ft_strncmp(node->command->args[0], "cd", 3) == 0)
				do_cd(node->command->args, &shell->env);
			else if (ft_strncmp(node->command->args[0], "export", 7) == 0)
				do_export(node->command->args, &shell->env);
			else if (ft_strncmp(node->command->args[0], "unset", 6) == 0)
				do_unset(node->command->args, &shell->env);
			else if (ft_strncmp(node->command->args[0], "exit", 5) == 0 || ft_strncmp(node->command->args[0], "q", 2) == 0)
			{
				shell->should_exit = 1;
				return (0);
			}
			else if (ft_strncmp(node->command->args[0], "echo", 5) == 0)
				do_echo(node->command->args);
			else if (ft_strncmp(node->command->args[0], "env", 4) == 0)
				do_env(shell->env);
			else if (ft_strncmp(node->command->args[0], "pwd", 4) == 0)
				do_pwd();
			else if (ft_strncmp(node->command->args[0], ":", 2) == 0)
				last_status = 0;
			else
			{
				signal(SIGINT, SIG_IGN);
				signal(SIGQUIT, SIG_IGN);
				pid = fork();
				if (pid == 0)
				{
					signal(SIGINT, SIG_DFL);
					signal(SIGQUIT, SIG_DFL);
					run_external_command(node->command->args, shell->env);
					rl_clear_history();
					cf_free_all();
					exit(127);
				}
				else if (pid < 0)
				{
					perror("fork");
					last_status = 1;
				}
				else
				{
					waitpid(pid, &status, 0);
					if (WIFEXITED(status))
						last_status = WEXITSTATUS(status);
					else if (WIFSIGNALED(status))
						last_status = 128 + WTERMSIG(status);
				}
			}
		}
	}
	else if (node->type == NODE_PIPE)
	{
		int pipefd[2];
		pipe(pipefd);

		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);

		pid_t left_pid = fork();
		if (left_pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			int status = execute_node(node->left, shell);
			t_ast_node *child_ast = node;
			free_ast(child_ast);
			rl_clear_history();
			cf_free_all();
			exit(status);
		}

		pid_t right_pid = fork();
		if (right_pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			int status = execute_node(node->right, shell);
			t_ast_node *child_ast = node;
			free_ast(child_ast);
			rl_clear_history();
			cf_free_all();
			exit(status);
		}

		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(left_pid, &status, 0);
		waitpid(right_pid, &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
	}
	else if (node->type == NODE_AND)
	{
		last_status = execute_node(node->left, shell);
		if (last_status == 0)
			last_status = execute_node(node->right, shell);
	}
	else if (node->type == NODE_OR)
	{
		last_status = execute_node(node->left, shell);
		if (last_status != 0)
			last_status = execute_node(node->right, shell);
	}
	else if (node->type == NODE_SUBSHELL)
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		pid = fork();
		if (pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			int status = execute_node(node->left, shell);
			t_ast_node *child_ast = node;
			free_ast(child_ast);
			rl_clear_history();
			cf_free_all();
			exit(status);
		}
		else if (pid < 0)
		{
			perror("fork subshell");
			last_status = 1;
		}
		else
		{
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
	}

	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);

	shell->last_exit_status = last_status;
	return (last_status);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_shell	shell;

	(void)argc;
	(void)argv;
	billy_print();
	shell.env = create_env(envp);
	shell.last_exit_status = 0;
	shell.should_exit = 0;
	disable_echoctl();

	t_ast_node *ast = NULL;

	while (1)
	{
		setup_signal();
		line = readline(SHELLNAME);
		if (line)
			ast = parse(line, &shell);
		if (!line)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
		if (*line)
			add_history(line);
		if (!check_for_input(line))
		{
			free(line);
			continue ;
		}


		//t_ast_node *ast = parse(line, &shell);

		if (ast)
		{
			execute_node(ast, &shell);
			free_ast(ast);
			ast = NULL;
		}
		free(line);
		if (shell.should_exit)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
	}
	shell.env = NULL;
	if (ast)
		free_ast(ast);
	ast = NULL;
	cf_free_all();
	return (0);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
// ➜  SuPuShell git:(master) ✗ echo hi >> out.txt
// ➜  SuPuShell git:(master) ✗ export FILE=test.txt
// ➜  SuPuShell git:(master) ✗ echo "$FILE" > $FILE
// ➜  SuPuShell git:(master) ✗ echo "$FILE" >> "$FILE"
// ➜  SuPuShell git:(master) ✗ echo "$FILE 'asda'" >> " $FILE"
// ➜  SuPuShell git:(master) ✗ echo '"$FILE 'asda'"' >> " $FILE"
// ➜  SuPuShell git:(master) ✗ q
