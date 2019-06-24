NAME    := woody_woodpacker
CFLAGS  += -g -Wall -Werror -Wextra
CFLAGS	+= -Iinclude -Ilibft/inc
LDFLAGS	+= -Llibft -lft
SRC     := \
	        load \
			main \
			encrypt \
			segment
OBJ		:= $(patsubst %, src/%.o, $(SRC))
LIBFT   := libft/libft.a

all: $(NAME)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	make -C libft

src/g_decryptor.c:
	cd src; nasm -f bin decryptor.s -o g_decryptor
	cd src; xxd -i -c 8 g_decryptor g_decryptor.c

src/g_decryptor.o: src/g_decryptor.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ) src/g_decryptor.o $(LIBFT) 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJ) src/g_decryptor src/g_decryptor.c src/g_decryptor.o
	#make -C libft clean

fclean:	clean
	rm -f $(NAME)
	#make -C libft fclean

re: fclean all
