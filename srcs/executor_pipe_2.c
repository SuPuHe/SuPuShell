/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:33:10 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 18:00:00 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Recursively processes all heredocs in a pipeline.
 *
 * Traverses the AST pipeline nodes and executes any heredocs found in
 * command nodes. Returns false if a heredoc processing fails.
 *
 * @param node Pointer to the AST node (command or pipe).
 * @return true if all heredocs were processed successfully, false otherwise.
 */
bool	handle_heredocs_in_pipeline(t_ast_node *node)
{
	if (!node)
		return (true);
	if (node->type == NODE_CMD && node->command->heredoc_count > 0)
	{
		if (!handle_heredoc(node->command))
			return (false);
	}
	else if (node->type == NODE_PIPE)
	{
		if (!handle_heredocs_in_pipeline(node->left))
			return (false);
		if (!handle_heredocs_in_pipeline(node->right))
			return (false);
	}
	return (true);
}
