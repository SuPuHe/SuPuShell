/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_handlers_2.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 15:34:00 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 12:03:57 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	apply_output_redirections(t_input *input)
{
	int	fd;
	int	i;

	i = 0;
	while (i < input->outfiles_count)
	{
		if (input->all_outfiles_append_flags[i])
			fd = open(input->all_outfiles[i], O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(input->all_outfiles[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			perror("minishell");
			return (false);
		}
		if (i == input->outfiles_count - 1)
		{
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else
			close(fd);
		i++;
	}
	return (true);
}
