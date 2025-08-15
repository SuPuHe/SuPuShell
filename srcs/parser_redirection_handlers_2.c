/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_handlers_2.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:34:00 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:18:30 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	apply_output_redirections(t_input *input)
{
	int	fd;
	int	flags;

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
