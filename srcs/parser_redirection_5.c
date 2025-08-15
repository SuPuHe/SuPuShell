/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_5.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:19:49 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:20:05 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*collect_heredoc_delimiter(t_list **current_tokens, bool *quoted)
{
	t_list				*start;
	t_string_builder	*sb;

	if (!current_tokens || !*current_tokens || !(*current_tokens)->content)
		return (NULL);
	start = *current_tokens;
	sb = sb_create();
	if (!sb)
		return (NULL);
	*current_tokens = collect_d_tokens_and_sb(start, sb, quoted);
	return (sb_build_and_destroy(sb));
}
