/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_second_part.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:30:27 by omizin            #+#    #+#             */
/*   Updated: 2025/07/23 15:17:48 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_error(char *argv)
{
	int	i;

	i = 0;
	write(2, BOLDRED, 9);
	while (argv[i] != '\0')
	{
		write(2, &argv[i], 1);
		i++;
	}
}

void	run_external_command(char **argv, t_env *env)
{
	char	*path;
	char	**envp;

	envp = build_envp(env);
	if (ft_strncmp(argv[0], "./", 2) == 0 || ft_strncmp(argv[0], "/", 1) == 0)
		path = argv[0];
	else
	{
		path = search_path(argv[0], env);
	}
	if (!path || access(path, X_OK) != 0)
	{
		print_error(argv[0]);
		write(2, ": command not found\n"RESET, 25);
		rl_clear_history();
		cf_free_all();
		exit(127);
	}
	execve(path, argv, envp);
	perror("execve");
	rl_clear_history();
	cf_free_all();
	exit(1);
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
