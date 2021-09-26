#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

char *get_token(int fd)
{
	int current_word_buffer_size = 50;
	int new_word_buffer_size;
	int word_size = 0;

	//State flags
	int is_word_flag = 0;
	int is_start_flag = 1;
	//Input
	int x = 0;
	int is_end = 0;
	//Output
	int y = 0;

	//Create initial 50 byte buffer
	char * word_buffer = (char *) malloc(current_word_buffer_size);
	if(word_buffer == NULL)
	{
		perror("Error message: ");
		exit(EXIT_FAILURE);
	}

	char * current_buf_ptr = word_buffer;
	static int static_fd_position = 0;

	//Read from file
	int read_return;
	while(1)
	{
		if(word_size > current_word_buffer_size)
		{
			new_word_buffer_size = current_word_buffer_size + 10;
			printf("Resizing word_buffer from %d to %d\n", current_word_buffer_size, new_word_buffer_size);
			char * realloc_buffer = (char *) realloc(word_buffer, new_word_buffer_size);
			if(realloc_buffer == NULL)
			{
				perror("Error message: ");
				exit(EXIT_FAILURE);
			}
			word_buffer = realloc_buffer;
			current_buf_ptr = realloc_buffer + current_word_buffer_size;
			current_word_buffer_size = new_word_buffer_size;
		}

		read_return = read(fd, current_buf_ptr, 1);
                word_size++;
		//printf("read_return: %d\n", read_return);

		//Check return value
		if(read_return == -1)
                {
                        perror("Error");
                        exit(EXIT_FAILURE);
                }
                else if(read_return == 0)
                {
                        //End of file reached
                        //printf("returns Null\n")
                	int current_fd_position = lseek(fd, 0, SEEK_CUR);
			if(current_fd_position == static_fd_position){
				//have been at this position before -> no new word
				return NULL;
			}
			if(current_fd_position > static_fd_position){
				static_fd_position = current_fd_position;
			}
			is_end = 1;
		}
		//printf("word_buffer: %s\n", word_buffer);
		//printf("is_word_flag: %d\n", is_word_flag);
		//printf("is_start_flag: %d\n", is_start_flag);
		//printf("current character: %c, int:%d\n", current_buf_ptr[0], current_buf_ptr[0]);
		//printf("word size: %d\n", word_size);

		if(current_buf_ptr[0] != '\n' &&
		current_buf_ptr[0] != ' ' &&
		current_buf_ptr[0] != '\t')
		{
			//Non-whitespace character
			x = 1;
		}
		else
		{
			//Whitespace character
			x = 0;
		}

		//Calculate result
		y = (!is_start_flag && is_word_flag && is_end)
		 	|| (!is_start_flag && is_word_flag && !x);
		if(y==1)
		{
			if(is_end)
			{
				current_buf_ptr[0] = '\n';
			}
			else
			{
				current_buf_ptr[0] = '\n';
			}
			//printf("Return word_buffer\n");
			return word_buffer;
		}

		//Calculate state transitions
		is_word_flag = (is_start_flag && is_word_flag && x)
			|| (!is_start_flag && is_word_flag && x)
			|| (!is_start_flag && is_word_flag && !x)
			|| (!is_start_flag && !is_word_flag && x);
		is_start_flag = 0;
		//write_return = write(1, &buffer, 1);

		if(x == 1)
		{
			current_buf_ptr++;
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
                fprintf(stderr, "Error: number of arguments\n");
                return EXIT_FAILURE;
        }

	int fd = open(argv[1], O_RDONLY);
	if(fd == -1)
	{
		perror("Error");
		return EXIT_FAILURE;
	}

	int num_token = 0;
	char *read_return;
	char *buffer_begin_ptr;
	while(1)
	{
		read_return = (char *)get_token(fd);
		buffer_begin_ptr = read_return;
		//read_return = NULL;
		if(read_return != NULL)
		{
			//Received a token/word
			while(read_return[0] != '\n')
			{
				write(1, read_return, 1);
				read_return ++;

			}
			write(STDOUT_FILENO, "\n", 1);
			//Reset the file descriptor by one position
			lseek(fd, -1, SEEK_CUR);
			//Free the buffer memory
			free(buffer_begin_ptr);
		}
		else
		{
			break;
		}
		//write(1, "test output\n", sizeof("test output")+1);
		//token should be printed with syscall
 		num_token ++;
	}

	printf("number of tokens: %d\n", num_token);

	close(fd);

	return EXIT_SUCCESS;
}

