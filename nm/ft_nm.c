/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ademenet <ademenet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/13 15:49:57 by ademenet          #+#    #+#             */
/*   Updated: 2017/12/19 11:57:02 by ademenet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/mman.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

/*
enum	error
{
	EXIT_FAIL = -1,
	EXIT_SUCC
};

typedef enum error	t_error;

void			print_error(t_error err)
{
	if (err == EXIT_FAIL)
		printf("%s\n")
	else if (err == EXIT_SUCC)
		printf("%s\n")
}
*/

/*
** TODO :
** [ ] lire les sections pour Mach-O
** [ ] Gerer les fat
** [ ] load file, gestion d'erreurs
*/

void			print_output(int nsyms, int symoff, int stroff, char *ptr)
{
	int					i;
	char				*stringtable;
	struct  nlist_64	*array;

	array = (void *)ptr + symoff;
	stringtable = (void *)ptr + stroff;
	for (i = 0; i < nsyms; ++i)
	{
		printf("%s\n", stringtable + array[i].n_un.n_strx);
	}
}

void			handle_64(char *ptr)
{
	int						ncmds;
	int						i;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;

	header = (struct mach_header_64 *)ptr;
	ncmds = header->ncmds;
	lc = (void *)ptr + sizeof(*header);
	for (i = 0 ; i < ncmds; ++i)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			print_output(sym->nsyms, sym->symoff, sym->stroff, ptr);
			break ;
		}
		lc = (void *)lc + lc->cmdsize;
	}
}

void			nm(char *ptr)
{
	// Nous regardons les 4 premiers octets du fichier afin de déterminer
	// sur quel type d'architecture nous nous trouvons : 64 bits, fat
	// binaries, etc.
	// `hexdump -C -n 4 a.out`
	int			magic_number;

	magic_number = *(int *)ptr;
	if ((int)magic_number == MH_MAGIC_64)
	{
		handle_64(ptr);
	}
}

int				main(int ac, char **av)
{
	int			fd;
	char		*ptr;
	struct stat	buf;

	if (ac != 2)
	{
		fprintf(stderr, "Please give me arg\n");
		return (EXIT_FAILURE);
	}
	if ((fd = open(av[1], O_RDONLY)) < 0)
	{
		perror("open");
		return (EXIT_FAILURE);
	}
	if (fstat(fd, &buf) < 0)
	{
		perror("fstat");
		return(EXIT_FAILURE);
	}
	if ((ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		perror("mmap");
		return (EXIT_FAILURE);
	}
	nm(ptr);
	if (munmap(ptr, buf.st_size) < 0)
	{
		perror("munmap");
		return(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}