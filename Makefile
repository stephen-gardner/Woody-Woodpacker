NAME	= woody_woodpacker
CFLAGS	+= -Ofast -Wall -Werror -Wextra
CFLAGS	+= -Iinc -Ilibft/inc
LDFLAGS	+= -Llibft -lft
SRCDIR	= src
SRC		= \
			decrypt \
			encrypt \
			load \
			main \
			segment
OBJDIR	= obj
OBJ		= $(patsubst %, $(OBJDIR)/%.o, $(SRC))
LIBFT	= libft/libft.a

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	make -C libft

$(OBJDIR)/g_decryptor.c:
	nasm -f bin $(SRCDIR)/decryptor.s -o $(OBJDIR)/g_decryptor
	cd $(OBJDIR) && xxd -i -c 8 g_decryptor g_decryptor.c

$(OBJDIR)/g_decryptor.o: $(OBJDIR)/g_decryptor.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ) $(OBJDIR)/g_decryptor.o $(LIBFT) 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJDIR)
	make -C libft clean

fclean:	clean
	rm -f $(NAME)
	make -C libft fclean

re: fclean all
