/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:23:19 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/20 11:33:04 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

/**
 * @brief Creates an AST node for a command.
 *
 * Allocates and initializes a node of type NODE_CMD, storing the command
 * input and setting child pointers to NULL.
 *
 * @param cmd Pointer to command input structure.
 * @return Pointer to the created AST node, or NULL on allocation failure.
 */
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

/**
 * @brief Creates a binary AST node for operators (e.g., &&, ||, |).
 *
 * Allocates and initializes a node with the given type and child nodes.
 * Sets command pointer to NULL.
 *
 * @param type Type of binary node (operator).
 * @param left Pointer to left child node.
 * @param right Pointer to right child node.
 * @return Pointer to the created AST node, or NULL on allocation failure.
 */
t_ast_node	*create_binary_node(t_node_type type,
	t_ast_node *left, t_ast_node *right)
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

/**
 * @brief Creates an AST node for a subshell.
 *
 * Allocates and initializes a node of type NODE_SUBSHELL, with the child
 * node as its left child. Sets right and command pointers to NULL.
 *
 * @param child Pointer to the subshell's child AST node.
 * @return Pointer to the created AST node, or NULL on allocation failure.
 */
t_ast_node	*create_subshell_node(t_ast_node *child)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_SUBSHELL;
	node->left = child;
	node->right = NULL;
	node->command = NULL;
	return (node);
}

/**
 * @brief Recursively frees an AST and all its nodes.
 *
 * Traverses the AST, freeing left and right children, command input,
 * and the node itself.
 *
 * @param node Pointer to the root AST node to free.
 */
void	free_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->left)
		free_ast(node->left);
	if (node->right)
		free_ast(node->right);
	if (node->command)
	{
		free_heredoc_delimiters(node->command);
		cf_free_one(node->command);
	}
	cf_free_one(node);
}
