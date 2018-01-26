#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define Max_Length 1024
#define Max_Word 10


int input_flag = 0;
int output_flag = 0;
int background_flag = 0;
int multi_redirect=0;
int pipe_and_redirect =0;
int pipe_flag = 0;
int in_flag_count;
int out_flag_count;
char* input_file = NULL;
char* output_file = NULL;
char DELI[]= {' ','^','*'};


void error_handler(char* temp[])
{
	int i=0;
	in_flag_count=0;
	out_flag_count=0;

	if (temp[i] == NULL)
		printf("ERROR: There is no command!"); // ERROR


	while (temp[i]!= NULL){
	if (strcmp (temp[i],"<")==0)
		in_flag_count++;
	if (strcmp (temp[i],">")==0)
		out_flag_count++;
	if (in_flag_count ==1 && out_flag_count==1)
		multi_redirect=1;
	i++;
	}


	if (in_flag_count > 1 || out_flag_count > 1)
	{
		temp[0]=NULL;
		printf("ERROR: Only redirection is allowed!");
	}
	if ((in_flag_count == 1 && (input_file ==NULL || strcmp(input_file,">")==0)) || (out_flag_count == 1 && (output_file ==NULL || strcmp(output_file,"<")==0)))
		{
		printf("ERROR: File is missed!");
		temp[0]=NULL;
	}


}


int readline(char Line[],char Line1[])
{
	while(1)
	{
	printf("\nsish:>");
	fgets(Line, Max_Length, stdin);
	int i=0;
	while(Line[i]!='\n')
        {
        Line1[i]=Line[i];
        i++;
        }

	Line[i]='\0';
	Line1[i]='\0';
	return 1;
	}

}


int parseline(char Line[],char *temp[])
{

	int i=0;
	if (strcmp(Line, "exit")==0)
		exit(0);


	temp [i] = strtok(Line,DELI);

	while (temp[i] != NULL)
	{
		i++;
		temp[i] = strtok(NULL,DELI);
	}
	return 1;

}

int redirect_check (char* temp[])
{
	int i=0;

	while (temp[i] != NULL)
	{
		if (strcmp(temp[i],"<")==0)
		{
			input_flag=1;
			input_file = temp [i + 1];

		}
		if (strcmp(temp[i],">")==0)
		{
			output_flag=1;
			output_file = temp [i+ 1];
		}
		if (strcmp(temp[i],"|")==0)
			pipe_flag=1;

		i++;
		}
		if ((input_flag==1 || output_flag ==1) && pipe_flag ==1)
			pipe_and_redirect =1;


		if (temp[i] == NULL)
		{
			if (strcmp(temp[i-1],"&")==0)
			{
			background_flag=1;
			temp[i-1]=NULL;
			}
		}
	return 1;
}

int redirect_position (char* temp[])
{
	int i=0;
	while (temp[i]!= NULL)
	{
		if (in_flag_count ==1 || out_flag_count ==1) // OR is enough for AND
		{
			 if ((strcmp(temp[i],"<")==0) || (strcmp(temp[i],">")==0)) //The first char
				return i;
		}

		i++;
	}
	return i;
}


void distributed_arrays(char* temp[], char* arg[])
{
	int pos = redirect_position(temp);
	int i=0;

	while (i < pos )
	{
		arg[i] = temp[i];
		i++;
	}
	arg[i] = NULL;

	return;
}

 void pipe_handler(char Line1[])  //Pipe
{
	int status2;
    int pid;
    int fd[2];
	int fd_in;
    int i;
	int j;
	char *arr1[10][10];
	char *cmd[10];


	i=0;
    	cmd[i] = strtok(Line1,"|");
	while (cmd[i] != NULL)
	{
		i++;
		cmd[i] = strtok(NULL,"|");
	}

	i=0;
	fd_in=0;

	while(cmd[i]!=NULL)
	{	j=0;
		arr1[i][j]= strtok(cmd[i],DELI);
		while(arr1[i][j]!=NULL)
		{

			j++;
			arr1[i][j]=strtok(NULL,DELI);

		}

		pipe(fd);
		pid = fork();

		if (pid == 0)
		{
			if (input_flag ==1 && pipe_and_redirect==1)
			{	j=0;
				while (arr1[i][j]!=NULL)
				{
					if (strcmp(arr1[i][j],"<")==0)
					{
						while (arr1[i][j]!=NULL)
						{
							arr1[i][j]=NULL;
							j++;
						}
						input_flag=0;
						break;

					}
					j++;
				}
				dup2(open(input_file, O_RDWR| O_CREAT, 0777),0);
			}
			else
				dup2(fd_in,0);
			if (cmd[i+1]!= NULL)
				dup2(fd[1],1);
			if (output_flag ==1 && pipe_and_redirect==1 && cmd[i+1]== NULL)
			{	j=0;
				while (arr1[i][j]!=NULL)
				{
					if (strcmp(arr1[i][j],">")==0)
					{
						while (arr1[i][j]!=NULL)
						{
							arr1[i][j]=NULL;
							j++;
						}
						output_flag=0;
						break;

					}
					j++;
				}

				dup2(open(output_file, O_RDWR| O_CREAT, 0777),1);
			}
			close(fd[0]);
			execvp(arr1[i][0],arr1[i]);
			exit(0);
		}
		else if (pid>0)
		{
			wait(NULL);
			close(fd[1]);
			fd_in=fd[0];
			i++;
		}
		else {
		exit(0);
		}

    	}


}



void execute (char *arg[], char* temp[],char Line1[])
{
	int status;
	int pid = fork();

	if (pid < 0)
		printf("ERROR");
	if (pid == 0)
	{

		if ((input_flag == 1 && input_file!= NULL && pipe_and_redirect !=1)|| multi_redirect==1)
			dup2(open(input_file, O_RDWR| O_CREAT, 0777),0);
		if((output_flag == 1 && output_file != NULL && pipe_and_redirect !=1)|| multi_redirect==1)
			dup2(open(output_file, O_RDWR| O_CREAT, 0777),1);


		if (pipe_flag !=1)
	        	execvp(arg[0],arg);
		else
		{
			pipe_handler(Line1);
			exit(0);
		}


	}
	if (pid > 0)
	{
		if (background_flag ==0)
			wait(&status);
		input_flag = 0;
        output_flag = 0;
		pipe_flag =0;
        input_file = NULL;
        output_file = NULL;
		background_flag=0;

	}
}

int runner (char Line[],char Line1[], char* arg[], char* temp[])
{	readline(Line,Line1);
	parseline(Line,temp);
	redirect_check (temp);
	error_handler(temp);
	distributed_arrays(temp,arg);

	return 1;
}

int main(int argc, char*argv[])
{

    char *arg[Max_Word];
    char *temp[Max_Word];
    char Line[Max_Length];
    char Line1[Max_Length];


    while (runner(Line,Line1,arg,temp))
	{
            execute(arg, temp,Line1);
        }


    return 0;
}

