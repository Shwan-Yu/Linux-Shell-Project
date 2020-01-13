#include "envVar.h"

#include <linux/limits.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
std::map<std::string, std::string> envVar::var_map;
//constructor
envVar::envVar() {
}

//set variable
void envVar::setV(std::string v_name, std::string v_val) {
  if (v_name == "") {
    return;
  }
  var_map[v_name] = v_val;
}

//export variable
void envVar::expV(std::string v_name) {
  std::string val = var_map[v_name];
  int exp_res = setenv(v_name.c_str(), val.c_str(), 1);
  //success
  if (exp_res == 0) {
    std::cout << "Variable exported" << std::endl;
  }
  else {
    perror("Fail to export this variable");
  }
}

//reverse helper
void reverseStr(std::string & str) {
  int n = str.length();

  // Swap character starting from two
  // corners
  for (int i = 0; i < n / 2; i++)
    std::swap(str[i], str[n - i - 1]);
}

//reverse variable
void envVar::revV(std::string v_name) {
  std::string val = var_map[v_name];
  reverseStr(val);
  var_map[v_name] = val;
  return;
}

//set key value pair value from input command
bool envVar::setKVPair(std::string inp_cmd, char * method) {
  cmdParser inp(inp_cmd, PATH_MAX);
  //check to see what the method we are using
  bool isSet = (strcmp(method, "set") == 0);
  bool isExp = (strcmp(method, "export") == 0);
  bool isRev = (strcmp(method, "rev") == 0);
  //parse input
  char ** kv = inp.parse(" ");
  size_t c = 0;
  name = "";
  val = "";
  //check the number of input command
  while (kv[c]) {
    ++c;
  }
  //more than 3, invalid
  if (c > 3) {
    fprintf(stderr, "Too many arguements\n");
    return false;
  }
  //set need two arguement
  if (isSet) {
    if (c == 3) {
      name = kv[1];
      val = kv[2];
      return true;
    }
    else if (c == 2) {
      name = kv[1];
      fprintf(stderr, "Need a second arguement\n");
    }
    else if (c <= 1) {
      fprintf(stderr, "Need two arguement\n");
    }
    return false;
  }
  //others method need only one arguement
  else if (isExp || isRev) {
    if (c == 2) {
      name = kv[1];
      return true;
    }
    else if (c == 3) {
      fprintf(stderr, "only need one arguement\n");
    }
    else if (c <= 1) {
      fprintf(stderr, "Need one arguement\n");
    }
    return false;
  }
  return true;
}
