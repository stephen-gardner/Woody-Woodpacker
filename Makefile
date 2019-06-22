NAME    := woody_woodpacker
CFLAGS  += -g -Wall -Werror -Wextra
CFLAGS	+= -Iinclude
LDFLAGS	+= -Llibft -lft
SRC     := src/main.c
OBJ		:= $(SRC:.c=.o)
LIBFT   := libft/libft.a

all: $(NAME)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	make -C libft

src/decryptor.c:
	cd src; nasm -f bin decryptor.s -o decryptor
	cd src; xxd -i -c 8 decryptor decryptor.c

src/decryptor.o: src/decryptor.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ) src/decryptor.o $(LIBFT) 
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJ) src/decryptor src/decryptor.c src/decryptor.o

fclean:	clean
	rm -f $(NAME)

re: fclean all
