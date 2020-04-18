# Linux_shell-C

This repo is for CS170: Spring 2016 project1 - OS course

The overall job of the shell program is to take input from the user, parse it, and perform the program or commands specified by the user. In addition to executing programs the user specifies, the shell also provides support for functionality such as input/output redirection, pipelining output from one program into another, and putting processes into the background.

The basic structure of the shell is essentially a long loop that parses user input. When running normally, the shell should always present the user with the prompt "sish:>" before each command is executed. In cases where STDIN is not a TTY (input redirection during invocation
of your shell), no prompt should be displayed.

### Parsing each line of input, supports the following tokens/functionality:
- Basic commands
- Command line arguments 
- Input/output redirection using '<' and '>' characters 
- Pipelining using '|' character 
- Background processes using '&' character 
- Exiting the shell 
- White space characters 
- Error handling
