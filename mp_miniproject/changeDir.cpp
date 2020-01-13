#include "changeDir.h"

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

std::string changeDir::curDir;

//constructor, store pwd
changeDir::changeDir() {
  curDir = getenv("PWD");
}

//generate prompt for ffosh
int changeDir::generatePrompt(std::string path) {
  std::cout << "ffosh:" << path << " $";
  return EXIT_SUCCESS;
}

//check to see if we can find matching path
int findDir(char * cmd, char * dName) {
  DIR * dir = opendir(dName);
  int canFind = false;
  struct dirent * entr;
  if (dir) {
    //loop and see if we can find any matching path
    while ((entr = readdir(dir)) != NULL) {
      if (strcmp(entr->d_name, cmd) == 0) {
        canFind = true;
      }
    }
  }
  else {
    //if no path find, then call the error
    perror("Cannot open directory");
    exit(EXIT_FAILURE);
  }
  closedir(dir);
  return canFind;
}

//change chars to string
const char * formatChars(char * str) {
  if (!str) {
    return NULL;
  }
  std::string s(str);
  const char * formatted = s.c_str();
  return strdup(formatted);
}

//concat two chars into one
char * concat(char * first, char * second, char with) {
  std::string f(first);
  std::string s(second);
  std::string res = f + with + s;
  return strdup(res.c_str());
}

//save absolute path to do the cd
void changeDir::absolutePath(char ** argv, envVar var) {
  std::string lst_dir = var.var_map["ECE551PATH"];
  cmdParser path(lst_dir, PATH_MAX);
  char ** envPre = path.parse(":");
  char * cmdPath = argv[0];
  std::string tmp(cmdPath);
  int isRelative = (tmp.find("/") == std::string::npos);
  int isCmdFound = 1;
  //if path is already absolute, we don't need to do anything
  if (!isRelative) {
    return;
  }
  else {
    int i = 0;
    while (envPre[i] != NULL) {
      //check if we get a legal path
      char * cwd = envPre[i];
      isCmdFound = findDir(cmdPath, cwd);
      if (isCmdFound == false) {
        ++i;
        continue;
      }
      else {
        //if we found the path, we concat an absolute path out
        argv[0] = concat(cwd, cmdPath, '/');
        return;
      }
      ++i;
    }
  }

  if (isCmdFound == true) {
    return;
  }
  else {
    std::cerr << "Command " << cmdPath << " not found" << std::endl;
    exit(EXIT_FAILURE);
  }
}

//wrapper for get cwd to return a c++ string
std::string changeDir::getCWD() {
  char buff[PATH_MAX];
  getcwd(buff, PATH_MAX);
  std::string cwd(buff);
  return cwd;
}

//save the current working directory
void changeDir::saveCWD() {
  curDir = getCWD();
}

//cd command implementation
void changeDir::changeD(envVar var, char ** argv) {
  //if more arguement
  if (argv[1] && argv[2]) {
    fprintf(stderr, "We cannot have more than one arguement for cd\n");
  }
  //no input
  else if (!argv[1]) {
    fprintf(stderr, "We need an input for the cd command\n");
  }
  //correct input, we have path in argv[1]
  else if (argv[1] && !argv[2]) {
    //find path
    const char * dest = formatChars(argv[1]);
    if (chdir(dest) == 0) {
      saveCWD();
    }
    else {
      perror("-bash");
    }
  }
}
