#include "ffosh.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

//constructor for ffosh
ffosh::ffosh() : curPath(getenv("PWD")) {
}

//check if it is the function we have to implement:
//e.g. cd, set, export, rev
int isFunc(char * label, std::string func) {
  return (strcmp(label, func.c_str()) == 0);
}

//exit the program with proper status code
void exitProgram(int statusCode) {
  //exit
  if (WIFEXITED(statusCode)) {
    //success
    if (WEXITSTATUS(statusCode) == 0) {
      std::cout << "Program was successful";
    }
    //fail
    else {
      std::cout << "Program failed with code " << WEXITSTATUS(statusCode);
    }
    std::cout << std::endl;
  }
  //signal
  else if (WIFSIGNALED(statusCode)) {
    std::cout << "Terminated by signal " << WTERMSIG(statusCode) << std::endl;
  }
}

//cmd line read
std::string cmdRead() {
  std::string line;
  std::getline(std::cin, line);
  return line;
}

//helper function to change $var to its value
//and check if it succeed
bool varToValue(envVar var, char ** argv, argvManager manager) {
  if (manager.varToVal(var, argv) == 1) {
    fprintf(stderr, "Fail to detect variable");
    return false;
  }
  return true;
}

//fork process and use exeve to call built in functions
void useBuiltInFunc(char ** argv, envVar var, changeDir cdHelper, int statusCode) {
  pid_t cpid = fork();
  if (cpid == 0) {
    //child
    //get path
    cdHelper.absolutePath(argv, var);
    execve(argv[0], argv, environ);
    perror("execve"); /* execve() only returns on error */
    exit(EXIT_FAILURE);
  }
  else {
    //parent
    waitpid(cpid, &statusCode, WUNTRACED | WCONTINUED);
    exitProgram(statusCode);
  }
}

//after each execution of command, we need to use this function to
//maintain the shell as active.
int ffosh::nextExecution(changeDir cdHelper, envVar var, argvManager manager) {
  while (!std::cin.eof()) {
    if (cdHelper.generatePrompt(curPath) != 0) {
      fprintf(stderr, "Fail to generate prompt\n");
    }
    //call cmdread
    std::string line = cmdRead();
    int isEmpty = (line == "");
    int isExit = (line == "exit");
    var.setCmdInp(line);
    if ((!isEmpty) && (!isExit)) {
      //execute
      executeGeneral(line, cdHelper, var, manager);
    }
    else if (isExit) {
      //user type exit
      return EXIT_SUCCESS;
    }
  }
  //eof exit
  std::cout << std::endl;
  return EXIT_SUCCESS;
}

void ffosh::executeGeneral(std::string inputCommand,
                           changeDir cdHelper,
                           envVar var,
                           argvManager manager) {
  //set up parser and argv
  cmdParser parser(inputCommand, PATH_MAX);
  char ** argv = parser.parse(" ");
  if (argv == NULL) {
    return;
  }
  char * label = argv[0];
  //create status code for exiting
  int statusCode = 0;
  //generate variable for conditional check
  int isCd = isFunc(label, "cd");
  int isSet = isFunc(label, "set");
  int isExport = isFunc(label, "export");
  int isRev = isFunc(label, "rev");

  //if var to value fails
  if (varToValue(var, argv, manager) == false) {
    return;
  }

  if (!(isCd || isSet || isExport || isRev)) {
    useBuiltInFunc(argv, var, cdHelper, statusCode);
  }
  else {
    //first do sanity check
    if (var.setKVPair(var.inp_cmd, argv[0]) == false) {
      fprintf(stderr, "input not valid\n");
    }
    else if (manager.isAllValid(var.name) == false) {
      fprintf(stderr, "variable name not valid\n");
    }
    //then run the function we implemented
    else if (isCd) {
      cdHelper.changeD(var, argv);
      curPath = cdHelper.curDir;
    }
    else if (isSet) {
      var.setV(var.name, var.val);
    }
    else if (isExport) {
      var.expV(var.name);
    }
    else if (isRev) {
      var.revV(var.name);
    }
    return;
  }
}

//function to start the ffosh shell
int ffosh::startShell() {
  changeDir cdHelper;
  setenv("ECE551PATH", getenv("PATH"), 1);
  argvManager manager;
  envVar var;
  //need at least have ECE551PATH as our first variable -> from Piazza
  var.var_map["ECE551PATH"] = getenv("ECE551PATH");

  //keep generating next execution
  int exeRes = nextExecution(cdHelper, var, manager);
  return exeRes;
}

//main function to create ffosh and start it
int main() {
  ffosh ffosh;
  ffosh.startShell();
  return EXIT_SUCCESS;
}
