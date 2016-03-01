#include <wait.h>     /* wait */
//#include <sys/stat.h> /* File mode bits (permissions) */
#include <fcntl.h>    /* creat, open, close */
#include <unistd.h>   /* pipe, dup2 */
#include <stdlib.h>   /* malloc, realloc, free, NULL */
#include <stdio.h>    /* puts, printf, dprintf */
#include <string.h>   /* strchr, strlen, strcpy, strcmp */
//#include <signal.h>   /* signal, SIGINT, SIG_IGN */

static const char *alloc_error = "rash: allocation error\n";
#define IN '\a'
#define OUT '\b'
#define PIPE '\f'

/* Declarations for builtin shell commands: */
//static int rash_cd(char **args);
static int rash_help(char **args);
static int rash_exit(char **args);

/* List of builtin commands and their corresponding functions. */
static char *builtin_str[] =
{
//    "cd",
    "help",
    "exit"
};

static int (*builtin_func[]) (char **) =
{
//    &rash_cd,
    &rash_help,
    &rash_exit
};

static const int rash_num_builtins = sizeof(builtin_str) / sizeof(char*);
/*
static int rash_cd(char **args)
{
    if (args[1] == NULL)
        fprintf(stderr, "rash: expected argument to \"cd\"\n");
    
    else if (chdir(args[1]) != 0)
            perror("rash");
    
    return 1;
}
*/
static int rash_help(char **args)
{
    int i;
    puts("Rash, a shell for RazOS.");
    puts("Type a program name and any possible arguments, then press enter.");
    puts("List of shell builtin commands:");

    for (i = 0; i < rash_num_builtins; i++)
    {
        printf("\t%s\n", builtin_str[i]);
    }

    return 1;
}

static int rash_exit(char **args)
{
    return 0;
}

static int do_commands(char **args, int pipes, int *commandStarts, char *input_name, char *output_name)
{
    int commands = pipes + 1;
    int pid;
    int status;
    int i, k;
    int input_fp;
    int output_fp;
    int *pipefds = malloc(sizeof(int) * 2 * pipes);
    
    for(i = 0; i < pipes; i++)
    {
        if (pipe(pipefds + i*2) < 0)
            return -1;
    }

    for (i = 0; i < commands; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            /* if not last command */
            if (i < pipes) /* Connect pipe output to stdout. */
            {
                if(dup2(pipefds[2*i + 1], 1) < 0)
                    return -1;
            }
            else if (output_name != NULL)
            {
                output_fp = creat(output_name, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
                if (dup2(output_fp, 1) < 0)
                    return -1;
            }
            /* if not first command */
            if (i != 0) /* Connect pipe input to stdin. */
            {
                if (dup2(pipefds[(2*i)-2], 0) < 0)
                    return -1;
            }
            else if (input_name != NULL)
            {
                input_fp = open(input_name, O_RDONLY);
                if (dup2(input_fp, 0) < 0)
                    return -1;
            }

            for (k = 0; k < 2*pipes; k++)
            {
                    close(pipefds[k]);
            }

            /* The commands are executed here. */
            if (execv(args[commandStarts[i]], args+commandStarts[i]) < 0)
				return -1;

        }
		else if(pid < 0)
			return -1;
    }

    for (i = 0; i < 2 * pipes; i++)
    {
        close(pipefds[i]);
    }

    for (i = 0; i < pipes + 1; i++)
    {
        wait(&status);
    }

    free(pipefds);
    return 0;
}

/* Launch a program and wait for it to terminate. */
static int rash_launch(char **args)
{
    int pipes = 0;
    int cmd_buf = 8;
    int *commandStarts = malloc(cmd_buf * sizeof(int));
    char *input_name = NULL;
    char *output_name = NULL;
     
    int i;
    int s = 1;
    commandStarts[0] = 0;
    /* Counts the pipes, sets them to NULL and sets the command boundaries. */
    for (i = 0; args[i] != NULL; i++)
    {
        if (strchr(args[i], PIPE))
        {
            free(args[i]);
            args[i] = NULL;
            commandStarts[s++] = i+1;
            ++pipes;
        }
        else if (strchr(args[i], IN))
        {
            free(args[i]);
            args[i++] = NULL;
            input_name = malloc(strlen(args[i]) + 1);
            strcpy(input_name, args[i]);
        }
        else if (strchr(args[i], OUT))
        {
            free(args[i]);
            args[i++] = NULL;
            output_name = malloc(strlen(args[i]) + 1);
            strcpy(output_name, args[i]);
        }
        
        if (s >= cmd_buf)
        {
            cmd_buf *= 2;
            commandStarts = realloc(commandStarts, cmd_buf * sizeof(int));
            if (commandStarts == NULL)
            {
                dprintf(STDERR_FILENO, "%s", alloc_error);
                exit(EXIT_FAILURE);
            }
        }
    }
    
    s = do_commands(args, pipes, commandStarts, input_name, output_name);
    
    free(input_name);
    free(output_name);
    free(commandStarts);
    
    if (s != 0) /* Error in the do_commands function */
    {
        //perror("rash");
        exit(EXIT_FAILURE);
    }

    return 1;
}

/* Execute a shell builtin or launch a program. */
static int rash_execute(char **args)
{
    int i;

    if (args[0] == NULL)
        /* An empty command was entered; return to command loop. */
        return 1;
    
    /* Check if the command is a builtin. */
    for (i = 0; i < rash_num_builtins; i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    }

    return rash_launch(args);
}

/* Split a line into tokens and return them. */
#define DEFAULT 0x01
#define QUOTED  0x02
#define ESCAPED 0x04
static char **rash_split_line(char *line, int *arg_len)
{
    int argbuf = 16;
    char **tokens = malloc(argbuf * sizeof(char*));
    
    int tokbuf = 16;
    char *token = malloc(tokbuf);
    int token_len = 0;
    int token_pos = 0;
    
    int state = DEFAULT;

    do
    {
        switch(state)
        {
        case DEFAULT:
            if (strchr(" \n\t|<>", *line))
            {
                if (token_len > 0)
                {
                    token[token_len] = '\0';
                    tokens[token_pos] = malloc(token_len + 1);
                    strcpy(tokens[token_pos++], token);
                    free(token);
                    token = malloc(tokbuf);
                    token_len = 0;
                }
                if (*line == '|' || *line == '<' || *line == '>')
                {
                    tokens[token_pos] = malloc(2);
					tokens[token_pos][0] = (*line == '|') ? PIPE : ((*line == '<') ? IN : OUT);
					tokens[token_pos++][1] = '\0';
                }
            }
            else if (*line == '\'')
                state = QUOTED;
            else if (*line == '\\')
                state = ESCAPED;
            else
                token[token_len++] = *line;
            break;

        case QUOTED:
            if (*line == '\'')
                state = DEFAULT;
            else
                token[token_len++] = *line;
            break;

        case ESCAPED:
            token[token_len++] = *line;
            state = DEFAULT;
        }
        if (token_pos >= argbuf)
        {
            argbuf *= 2;
            tokens = realloc(tokens, argbuf * sizeof(char*));
			if (tokens == NULL)
            {
                dprintf(STDERR_FILENO, "%s", alloc_error);
                exit(EXIT_FAILURE);
            }
        }
        if (token_len >= tokbuf)
        {
            tokbuf *= 2;
            token = realloc(token, tokbuf);
			if (tokens == NULL)
            {
                dprintf(STDERR_FILENO, "%s", alloc_error);
                exit(EXIT_FAILURE);
            }
        }
    } while (*line++ != '\0');
    
    free(token);
    tokens[token_pos] = NULL;
    *arg_len = token_pos;
    
    return tokens;
}

static void print_tokens(char **args, int len)
{
	printf("number of tokens: %d\n", len);
	for (int i = 0; i < len; i++)
	{
		if (*(args[i]) == PIPE)
			puts("PIPE");
		else if (*(args[i]) == IN)
			puts("IN");
		else if (*(args[i]) == OUT)
			puts("OUT");
		else
			printf("|%s|\n", args[i]);
	}
}

/* Read a line from stdin and return it. */
static char *rash_read_line(void)
{
	char *line = NULL;
	size_t n;
	dgetline(&line, &n, STDIN_FILENO);
	return line;
}

static void rash_loop(void)
{
    char *line;
    char **args;
    int status;
    int args_len;
    int i;

    do
    {
        printf("> ");
        line = rash_read_line();
        args = rash_split_line(line, &args_len);
        status = rash_execute(args);

        free(line);
        for (i = 0; i < args_len; i++)
            free(args[i]);
        free(args);
    } while (status);
}

int main(void)
{
	setenv("SHELL", "rash", 1);
    //signal(SIGINT, SIG_IGN);
    rash_loop();

    return EXIT_SUCCESS;
}
