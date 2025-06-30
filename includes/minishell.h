/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:06:41 by omizin            #+#    #+#             */
/*   Updated: 2025/06/30 11:21:43 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_printf.h"
# include "get_next_line.h"
# include "libft.h"

//readline + history
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
//malloc / free / printf
# include <stdio.h>
# include <stdlib.h>
// open / read / write / close
# include <fcntl.h>
# include <unistd.h>
//fork / execve / wait
# include <unistd.h>
# include <sys/wait.h>
// dup2 / pipe
# include <unistd.h>
# include <stdio.h>
//signal / kill / sigaction
# include <signal.h>
# include <unistd.h>
# include <stdio.h>
// access / stat
# include <unistd.h>
# include <sys/stat.h>
# include <stdio.h>
// opendir / readdir
# include <dirent.h>
# include <stdio.h>
//getcwd / chdir
# include <unistd.h>
# include <stdio.h>
// isatty / ttyname
# include <stdio.h>
# include <unistd.h>
//getenv / export
# include <stdio.h>
# include <stdlib.h>
//tcgetattr / tcsetattr
# include <termios.h>
# include <stdio.h>
# include <unistd.h>
//tgetent / tgetstr / tputs (termcap)
# include <term.h>
# include <unistd.h>

#endif
