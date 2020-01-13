#include "cmdParser.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

//empty constructor
cmdParser::cmdParser() {
}

//constructor with command line and size parametor
cmdParser::cmdParser(std::string cmd, size_t initSize) :
    inputCmd(cmd),
    argv(new char *[initSize]),
    size(initSize) {
}
//since we have allocated variable, we need rule of three
//destructor
cmdParser::~cmdParser() {
  delete[] argv;
}

//copy constructor
cmdParser::cmdParser(const cmdParser & rhs) :
    inputCmd(rhs.inputCmd),
    argv(new char *[rhs.size]),
    size(rhs.size) {
  //copy
  for (size_t i = 0; i < size; i++) {
    argv[i] = rhs.argv[i];
  }
}

//assignment operator
cmdParser & cmdParser::cmdParser::operator=(const cmdParser & rhs) {
  if (this != &rhs) {
    char ** tmp = new char *[rhs.size];
    //copy
    for (size_t i = 0; i < size; i++) {
      tmp[i] = rhs.argv[i];
    }
    delete[] argv;
    //assignment
    inputCmd = rhs.inputCmd;
    size = rhs.size;
    argv = tmp;
  }
  return *this;
}

//escaping backslash
void cmdParser::escape(std::string from, std::string to) {
  size_t find = inputCmd.find(from);
  while (find != std::string::npos) {
    inputCmd.replace(find, 2, to);
    find = inputCmd.find(from, find + 1);
  }
}

//parse by space
char ** cmdParser::parse(const char * parseBy) {
  escape("\\\\", "\\");
  escape("\\\"", "\"");
  size_t find = inputCmd.find("\"");
  if (find == std::string::npos) {
    char * tmp = strdup(inputCmd.c_str());
    //split into argv
    argv[0] = strtok(tmp, parseBy);

    int i = 1;

    while (argv[i - 1] != NULL) {
      argv[i] = strtok(NULL, parseBy);
      ++i;
    }
  }
  else {
    //if there's no \" left
    handleQuotes();
  }

  return argv;
}

//see if we have non-complete quotes
bool checkCompleteQuote(bool qot, int countQ) {
  if ((qot) || (countQ % 2 == 1)) {
    fprintf(stderr, "One of the quotes is open\n");
    std::cout << "Program failed with code 1" << std::endl;
    return false;
  }
  return true;
}

//assign parts in vector to argv
void cmdParser::assignToArgv(std::vector<std::string> v) {
  int ci = 0;
  std::cout << "(To see how it works, we also print it by lines first:" << std::endl;
  for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
    if (*it == "") {
      continue;
    }
    argv[ci] = (char *)it->c_str();
    std::cout << argv[ci] << std::endl;
    ++ci;
  }
  std::cout << ")" << std::endl << std::endl;
  std::cout << "The true execute result:" << std::endl;
}

//handle all kind of quotes
void cmdParser::handleQuotes() {
  std::vector<std::string> v;
  int len = inputCmd.length();
  bool hasQot = false;
  int arglen;
  int countQ = 0;
  for (int i = 0; i < len; i++) {
    int start = i;
    if (inputCmd[i] == '\"') {
      hasQot = true;
    }

    if (hasQot) {
      i++;
      start++;
      //split parts
      while (i < len && inputCmd[i] != '\"') {
        i++;
      }
      if (i < len) {
        hasQot = false;
      }
      arglen = i - start;
      i++;
    }
    else {
      //move to next
      while (i < len && inputCmd[i] != ' ')
        i++;
      arglen = i - start;
    }
    //store value
    v.push_back(inputCmd.substr(start, arglen));
  }
  //for checking single quote
  for (int i = 0; i < len; i++) {
    if (inputCmd[i] == '"') {
      ++countQ;
    }
  }
  //final check and assignment
  if (checkCompleteQuote(hasQot, countQ) == false) {
    argv = NULL;
  }
  else {
    assignToArgv(v);
  }
}
