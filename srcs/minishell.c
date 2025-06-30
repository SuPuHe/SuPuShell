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
	printf("%s\n", buf);
}

void	do_cd(char **argv)
{
	const char	*home = getenv("HOME");

	if (argv[1])
	{
		if (chdir(argv[1]) == -1)
			perror("cd");
	}
	else
		chdir(home);
}

void	command_handler(char **argv)
{
	if (strncmp(argv[0], "pwd", 4) == 0)
		do_pwd();
	else if (strncmp(argv[0], "cd", 3) == 0)
		do_cd(argv);
	else if (strncmp(argv[0], "exit", 5) == 0)
		exit(0);
	else
		printf("I dont know this command\n");
}

static char	*substr_dup(const char *start, const char *end)
{
	size_t	len;
	char	*s;

	len = end - start;
	s = malloc(len + 1);
	if (!s)
		return NULL;
	ft_memcpy(s, start, len);
	s[len] = '\0';
	return (s);
}

char	**split_args(const char *input)
{
	char		**args;
	int			i;
	const char	*p;
	const char	*start;

	args = malloc(64 * sizeof(char *));
	i = 0;
	p = input;
	while (*p)
	{
		while (*p && (*p == ' ' || *p == '\t' || *p == '\n'))
			p++;
		if (*p == '\0')
			break;
		start = p;
		while (*p && *p != ' ' && *p != '\t' && *p != '\n')
			p++;
		args[i++] = substr_dup(start, p);
		if (i >= 63)
			break;
	}
	args[i] = NULL;
	return (args);
}

void	free_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

int	main(void)
{
	char	*line;
	char	**args;

	while (1)
	{
		line = readline("SuPuShell$ ");
		if (!line)
			break ;
		if (*line)
			add_history(line);
		args = split_args(line);
		command_handler(args);
		free_args(args);
		free(line);
	}
	return (0);
}
