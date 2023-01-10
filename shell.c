#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <stdbool.h>
#define BUFF_SIZE (1024)
#define Max_Nr_Args (10) // o comanda poate avea maxim 10 cuvinte

//file system implementation->512, you can read from a disk maximum 512 bytes
static char prompt[512];
//1024=512*2 for 2 commands
static char cwd[1024];
static char path[1024];
char *input_buf;
int number_of_histories;
int running = 1;


void signal_ctrlC()
 
{
/*
backward-kill-line (C-x Rubout)
Kill backward from the cursor to the beginning of the current line. 
With a negative numeric argument, kill forward from the cursor 
to the end of the current line.
*/
rl_backward_kill_line(1,0);
printf("\n");
rl_redraw_prompt_last_line();
 
}

//function to print the whole history with a given number
//treats commands like: history 5
//
void print_history(int cnt)
{
 
/*used
http://www.math.utah.edu/docs/info/hist_2.html#SEC13
*/
 
	HIST_ENTRY **the_history_list;
	/*
	history_list()
	Return a NULL terminated array of HIST_ENTRY which is the current input history. 
	Element 0 of this list is the beginning of time. If there is no history, return NULL. */
 
	if(cnt > number_of_histories)
	{
		cnt = number_of_histories;
	}
 
	the_history_list = history_list();
	//int n=sizeof(the_history_list)/sizeof(the_history_list[0]);
	int i=0;
 
	printf("\n");
	while(i<cnt)
	{
 
		printf("%s\n",the_history_list[i]->line);
		i++;
 
	}
 
 
}

void create_prompt()
{
	char *username = getenv("USER"); //get the username
	getcwd(cwd,sizeof(cwd)); //get the current directory
	strcpy(prompt,username);
	strcat(prompt,":~");
	strcat(prompt,cwd);
	strcat(prompt,"$\0 ");
 
}


int splitCommand(char *command, char **command_split)
{
    char *copy_command;
    char *token;
    int i=0;

    copy_command=strdup(command);

    token=strtok(copy_command," ");
    while(token!=NULL){
        command_split[i]=token;
        ++i;
        token=strtok(NULL," ");
    }

    command_split[i]=NULL;

    return i;

    
}

bool Logic_Expression_1(char *command, char **logic_command){
    char *find_logic;
    char *token;
    char* copy_command;
    const char ch[3]="&&";
    int i;
 
    find_logic=strchr(command,'&');
    copy_command=strdup(command);
 
    char *aux=find_logic;
 if(find_logic){
    //for && ----------------------------
    if(strncmp(&aux[1],"&",1)==0){ //imi aux imi scoate &&....restul textului -> verific daca e & al doilea caracter
    //daca este primul caracter din tot cuvantul pe care mi-l scoate
        token=strtok(copy_command, "&&");
        i=0;
        while (token != NULL) {
            logic_command[i]=token;
            ++i;
            token = strtok(NULL, "&&");
        }
        logic_command[i]=NULL;
        
        
        return true;
 
    }
    }
    
    return false;
 
 
 
}
//verify ||
bool Logic_Expression_2(char *command, char **logic_command){
 
    char *find_logic;
    char *token;
    char* copy_command;
    const char ch[3]="||";
    int i;
 
    find_logic=strchr(command,'|');

    if(find_logic){
        copy_command=strdup(command);
 
    char *aux=find_logic;
 //for || ----------------------------------------
    
    if(strncmp(&aux[1],"|",1)==0){
       
       //if(&aux[1]=="|"){
	    token=strtok(copy_command, "||");
		i=0;
		while (token != NULL) {
		    logic_command[i]=token;
		    ++i;
		    token = strtok(NULL, "||");
		}
         logic_command[i]=NULL;
		return true;
 
	    }
    }
    
	    
	return false;
 
 
}

bool Logic_Expression_3(char *command, char **logic_command){
    char *find_logic;
    char *token;
    char* copy_command;
    const char ch[2]=";";
 
 	
 
    find_logic=strchr(command,';');
    copy_command=strdup(command);
    char *aux=find_logic;
 	
    
    if(find_logic){ //imi aux imi scoate &&....restul textului -> verific daca e & al doilea caracter
        token=strtok(copy_command, ";");
        int i=0;
        while (token != NULL) {
            logic_command[i]=token;
            ++i;
            token = strtok(NULL, ";");
        }
        logic_command[i]=NULL;
        return true;
 
    }
    
    return false;
    	
}

int execute(char *input_buf) 
{
	char *s[2];
	int i;
	char *command = input_buf;
	char *command_split[11];
	int size = splitCommand(command, command_split);
    
	pid_t pid = fork(); 
	if (pid<0) {	return errno; }
	else if (pid==0)  
	{
		//printf("in procesul copil:\n");
		int status_code = execvp(command_split[0], command_split);
		/*if (!strcmp("cd", args[0])) {
			return 1;
		}*/
		if(status_code < 0) {
			perror("Execvp ");
			//printf("eroare in execvp -> comanda gresita\n"); 
		}
		else printf("else execvp\n");
	}
	else {
		wait(NULL); // asteapta incheierea proc fiu
		//wait(NULL); // asteapta si proc curent
		//printf("fork incheiat\n");
	}
		 
}
//this function return true if pipe is found in command
//otherwise return false
//if pipe is found also splits the command in a array of pipe commands
bool hasPipe(char *command, char **pipe_command){
    char *find_pipe;
    char *token;
    char* copy_command;
    const char ch[2]="|";

    find_pipe=strchr(command,'|'); 
    copy_command=strdup(command);

    if(find_pipe){
        token=strtok(copy_command, "|");
        int i=0;
        while (token != NULL) {
            pipe_command[i]=token;
            ++i;
            token = strtok(NULL, "|");
        }

        pipe_command[i]=NULL;
        return true;
        
    }

       
    return false;
}

int executeCommand(char * command){
    char *pipe_split[2];
    int size1,size2;
    int j;
    
    if(Logic_Expression_1(command,pipe_split)){
        int i;
        int size[2];
        char *command_split[2][10];
        for(i=0;i<2;++i){
            size[i]=splitCommand(pipe_split[i],command_split[i]);
               
        }         
                         
           
            for(i=0;i<2;++i){
                pid_t childPid1=fork();

            if(childPid1<0)
                return errno;
            else if(childPid1==0){
                
                
                
                if(execvp(command_split[i][0],command_split[i])<0)
                    return 1;
                //not to make other children
                return 1;                
                perror(NULL);
            }
            else{
                wait(NULL);
                wait(NULL);
                   
                 }
            }
            
            for(i=0;i<2;++i){
                for(j=0;j<size[i]-1;++j)
                    if(command_split[i][j]!=NULL)
                        free(command_split[i][j]);
                
            }  
    }
     else if(Logic_Expression_2(command,pipe_split)) // logic expression ||
    {
 
    	int i;
        int size[2];
        char *command_split[2][10];
 
        for(i=0;i<2;++i){
            size[i]=splitCommand(pipe_split[i],command_split[i]);
 
          }         
 
 
            for(i=0;i<2;++i){
                pid_t childPid1=fork();
 
            if(childPid1<0)
                return errno;
            else if(childPid1==0){
 
 
                
                //if first command is right executed 
                if(execvp(command_split[i][0],command_split[i])>=0 && i==0)
                	return 1;
                else if(execvp(command_split[i][0],command_split[i])<0 && i==1)
                    return 1;
                return 1;
                perror(NULL);
            }
            else{
                wait(NULL);
                wait(NULL);
 
 
 
            }
            }
 
            for(i=0;i<2;++i){
                for(j=0;j<size[i]-1;++j)
                    if(command_split[i][j]!=NULL)
                        free(command_split[i][j]);
 
            }  
 
    }
    else if(Logic_Expression_3(command,pipe_split)){
        int i;
        int size[2];
        char *command_split[2][10];
 
        for(i=0;i<2;++i){
            size[i]=splitCommand(pipe_split[i],command_split[i]);
 
        }         
 
 
            for(i=0;i<2;++i){
                pid_t childPid1=fork();
 
            if(childPid1<0)
                return errno;
            else if(childPid1==0){
 
 
                
                execvp(command_split[i][0],command_split[i]);
                perror(NULL);
            }
            else{
                wait(NULL);
                wait(NULL);
 
 
 
            }
            }
 
            for(i=0;i<2;++i){
                for(j=0;j<size[i]-1;++j)
                    if(command_split[i][j]!=NULL)
                        free(command_split[i][j]);
 
            }  
    }
    else if(hasPipe(command,pipe_split)){
        
        char *command_split[2][10];
        pid_t childPid;
        //
        int pipefd[2];
        int size[2];
        int i;
        for(i=0;i<2;++i){
            size[i]=splitCommand(pipe_split[i],command_split[i]);
            }
        pipe(pipefd);

        childPid=fork();

        if(childPid<0)
            return errno;
        else  if(childPid==0){

            //pipefd[0]->for reading
            close(pipefd[0]);

            //pipefd[1] for writing
            //copy what is written to the fd to standard output
            dup2(pipefd[1],1);


            close(pipefd[1]);
            
            execvp(command_split[0][0],command_split[0]);

        }
        else{
            wait(NULL);
            wait(NULL);
            //close writing because the second command needs teh result from the first one
            close(pipefd[1]);

            //copy what is read to the standard input
            dup2(pipefd[0],0);

            
            close(pipefd[0]);
            
            execvp(command_split[1][0],command_split[1]);
            
        }
        
    }
    else 
        execute(command);
     
   
    //return 0;
}


int main(){
    
     /*
 SIGINT is one of the predefined signals that’s associated with 
 the terminal interrupt character (commonly Ctrl+C). 
 It causes the shell to stop the current process and return to its main loop, displaying a new command prompt to the user.
 */
 	signal(SIGINT,signal_ctrlC);


    using_history();
	int times_works=1;
	//int cnt=0;
 
	while(times_works)
 
	{
		create_prompt(); //create: user/path/
		input_buf = readline(prompt);
 
		if(strlen(input_buf)>0)
		{	
			number_of_histories+=1;
			add_history(input_buf);
			//call function(input_buf)
 
		}
		//strncmp returns 0 if the first n bytes are the same
		if(strncmp(input_buf,"exit",4)==0 || strncmp(input_buf,"quit",4)==0)
		{
 
		printf("Finished with exit/quit");
		//print_history(number_of_histories);
 
		break;
 
 
		rl_clear_history();
 
		}
 
 
		//times_works-=1;
		prompt[0] ='\0';


        char *s[2];
    int i;
    char *command=input_buf;

    if(strcmp(command,"history")==0)
		print_history(number_of_histories);
 
	else
        executeCommand(command);
 
    
    char *command_split[10];

	}
    return 0;
}
