/**
 * Simple shell interface program.
 *
 * Operating System Concepts
 *
 * Developed by Ivan Chamero de la Rosa
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>

//Preprocessor directives
#define MAX_LINE 80

//Global variables
static int bc = 0;
static int should_run = 1;
static char addr[FILENAME_MAX];

//Global struct, where I store my input from shell
struct inp_s {
  int num_args;
  char *arg[MAX_LINE/2 + 1];
} inp_s;


//Private function
static void separate (char *args);
static void notBuiltIn (void);
static void builtIn (void);
static bool isBuiltIn (void);
static void limpiaStruct (void);

//Function to separate the shell input by spaces
static void separate (char *args) {

  int i = 0, j = 0;
  char aux[10][40]; //Two-dimensional array where I store the inputs separately
  int lenA = strlen(args);

  for (int y = 0; y <= lenA; y++) { //Touring the input

    if (args[y] != '\n') {
      if ((args[y] == '&') && (args[y + 1] == '\n')) bc = 1; //If the input has & and it is on the last position, I do not store it and I put bc to 1
      else {
	aux[i][j] = args[y]; //I start to store all the letter of the input before a space
	j++; //Sum 1 to the next position where I type the next letter of the command
      }
    }

    if (args[y] == ' ') { //If there is an space, it meaning that a part of the command is stored
      aux[i][j - 1] = '\0'; //I put '\0' at the final position to mark the final of the string
      inp_s.arg[i] = aux[i]; //If there is a space it meaning that the first part is stored so I tell the *arg[] to point it
      i++; //Sum 1 to the columns
      j = 0; //Reset the position where I start typing the next part of the input comand
    }

    if ((args[y] == '\n') && (args[y++] == '\n')) { //I look for 2 '\n' because with fgets I get two of them
      //I do the same just in case I get only a 1 command
      aux[i][j] = '\0';
      inp_s.arg[i] = aux[i];
      inp_s.num_args = i + 1; //I store the total of input
    }
  }
}

//Function to do built-in commands
static void BuiltIn (void) {

  if (strcmp(inp_s.arg[0], "ic") == 0) {
    if (inp_s.num_args == 1) printf("%s\n", addr); //I print actual directory cuz I have the adress stored in a global variable
    else printf("Too much operands\n");
  }

  else if (strcmp(inp_s.arg[0], "cd") == 0) {
    if (inp_s.num_args == 1) chdir("/home"); //I go direct to /home
    else if (inp_s.num_args >= 3) printf("Too much operands\n"); //It can take more than 2 operands
    else if (strcmp(inp_s.arg[1], "..") == 0) chdir(".."); //I do cd ..
    else {
      if (chdir(inp_s.arg[1]) == 0) getcwd(addr, sizeof(addr)); //Chdir return 0 if it was successful and I update the new addr where I am
      else printf("Directory does not exists\n");
    }
  }

  else if (strcmp(inp_s.arg[0], "cc") == 0) {
    if (inp_s.num_args == 2){
      if (mkdir(inp_s.arg[1], 0777) == -1) printf("Folder already exists\n"); //Mkdir return -1 if it was unsuccessful neither I create a folder with 0777 permision (owner, group & other users)
    }
    else if (inp_s.num_args >= 3) printf("Too much operands\n"); //It can take more than 2 operands
    else printf("An operand is missing\n");
  }

  else if ((strcmp(inp_s.arg[0], "ec") == 0)) {
    if (inp_s.num_args == 2) {
      if (rmdir(inp_s.arg[1]) == -1) printf("Folder does not exists\n"); //Rmdir return -1 if it was unsuccessful neither I delete the folder
    }
    else if (inp_s.num_args >= 3) printf("Too much operands\n"); //It can take more than 2 operands
    else printf("An operand is missing\n");
  }

  else if (strcmp(inp_s.arg[0], "surt") == 0) should_run = 0; //I turn off the loop
}

//Function to know if command is built-in or not
static bool isBuiltIn (void) {

  if ((strcmp(inp_s.arg[0], "ic") == 0) || (strcmp(inp_s.arg[0], "cd") == 0) || (strcmp(inp_s.arg[0], "cc") == 0) || (strcmp(inp_s.arg[0], "ec") == 0) || (strcmp(inp_s.arg[0], "surt") == 0))
    return true;
  else
    return false;
}

//Function to clan the structure
static void limpiaStruct (void) {

  for (int i = 0; i <= (MAX_LINE/2 + 1); i++) { //Touring all the *arg[] positions
    inp_s.arg[i] = '\0';
  }

  inp_s.num_args = 0;
}

//Function to do not built-in commands
static void notBuiltIn (void) {

  pid_t pid;
  int status;

  pid = fork();

  if (pid == 0) { //Child
    if (execvp(inp_s.arg[0], inp_s.arg) == -1) { //Return -1 if is unsuccessful
      printf("%s: order not found\n", inp_s.arg[0]);
    }
  }

  else if (pid < 0) perror(""); //Fork error

  else { //Parent
    if (bc == 0) waitpid(pid, &status, WUNTRACED); //Wait for the child process normally
    else waitpid(pid, &status, WNOHANG); //Wait for the child process in background mode
  }
}

//Main function
int main (void) {

  char args[MAX_LINE/2 + 1];

  while (should_run) {
    limpiaStruct(); //I clean the struct
    bc = 0; //I set the backgound variable to 0 again

    getcwd(addr, sizeof(addr)); //I get the actual directory
    printf("%s>", addr);

    fgets(args, MAX_LINE, stdin); //I read from shell input
    fflush(stdin); //I clean the input

    separate(args); //I separate the input by spaces

    if (isBuiltIn()) BuiltIn();
    else notBuiltIn();
  }

  return 0;
}
