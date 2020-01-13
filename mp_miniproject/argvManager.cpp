#include "argvManager.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

//empty constructor
argvManager::argvManager() {
}

//check if is not one of the valid option
bool isNotValidChar(int ch) {
  bool isNum = (ch >= 48) && (ch <= 57);
  bool isUpp = (ch >= 65) && (ch <= 90);
  bool isLow = (ch >= 97) && (ch <= 122);
  bool isUnderSc = (ch == 95);
  return !(isNum || isUpp || isLow || isUnderSc);
}

//check if no invalid char found
bool argvManager::isAllValid(std::string str) {
  std::string::iterator it = std::find_if(str.begin(), str.end(), isNotValidChar);
  return it == str.end();
}

//change string to char *
char * strToChars(std::string s) {
  const char * c = s.c_str();
  return strdup(c);
}

//see if we match all $ sign
int ifMatchAll(size_t c_doll, size_t c_rep, char ** argv, int i, std::string cmdPiece) {
  if (c_doll != c_rep) {
    fprintf(stderr, "Fail to match the $ sign in the command at index %d\n", i);
    return 1;
  }
  else {
    argv[i] = strToChars(cmdPiece);
  }
  return 0;
}

//change all variable in argv to its value
int argvManager::varToVal(envVar var, char ** argv) {
  int i = 0;
  while (argv[i]) {
    size_t invalid = std::string::npos;
    //build up each piece
    std::string cmdPiece(argv[i]);
    size_t l = cmdPiece.length();
    size_t dollar_ind = cmdPiece.find("$");
    //if no $ found at all in this command
    if (dollar_ind == invalid) {
      ++i;
      continue;
    }
    //if found any $
    else {
      int right_bound = 1;
      int countDollar = 1;
      int countReplace = 0;
      //right_boundeep finding when we don't reach the end
      while (dollar_ind != invalid) {
        size_t start_left = dollar_ind + 1;
        size_t largest_right = dollar_ind + right_bound;
        if (largest_right > l) {
          break;
        }
        //slice a possible substring
        std::string possible = cmdPiece.substr(start_left, largest_right);
        //if we found it in the map, replace the value

        std::map<std::string, std::string>::iterator it = var.var_map.find(possible);
        std::map<std::string, std::string>::iterator e = var.var_map.end();
        bool notFound = (it == e);
        //if not found, ++right_bound to get another possible substring
        if (notFound) {
          ++right_bound;
        }
        //if found, replace it
        else {
          size_t slice_right = right_bound + 1;
          cmdPiece.replace(dollar_ind, slice_right, it->second);
          ++countReplace;

          dollar_ind = cmdPiece.find("$");
          //if another valid dollar found, we need one more place to match
          if (dollar_ind != invalid) {
            ++countDollar;
          }
          //reset right_bound
          right_bound = 1;
        }
      }
      //if the number of the two didn't match, means fail to match the $ sign here
      if (ifMatchAll(countDollar, countReplace, argv, i, cmdPiece) == 1) {
        return 1;
      };
    }

    ++i;
  }

  return 0;
}
