#include <string.h>

#include "changeDir.h"
class argvManager {
 public:
  //comments in .cpp
  argvManager();
  int varToVal(envVar var, char ** argv);
  bool isAllValid(std::string str);
};
