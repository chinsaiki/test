
NAME=		strace

SRC=		main.c \
		error.c \
		child.c \
		parent.c \
		print_args.c \
		spe_args.c

OBJ=		$(SRC:.c=.o)

CFLAGS+=	-W -Wall -ansi -pedantic $(DEFINES) -std=c99

DEFINES=	-D _BSD_SOURCE -D _XOPEN_SOURCE -D _GNU_SOURCE

SYSCALL_DB=	syscall_db

$(NAME):	$(OBJ)
		gcc -o $(NAME) $(OBJ)
		./getSyscalls.sh

all:		$(NAME)

clean:
		rm -rf $(OBJ)

fclean:		clean
		rm -rf $(NAME)
		rm -rf $(SYSCALL_DB)

re:		fclean all