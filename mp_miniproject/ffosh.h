#include <string>

#include "argvManager.h"

class ffosh {
 public:
  //store pwd
  std::string curPath;
  ffosh();
  //function description in .cpp
  int startShell();
  int nextExecution(changeDir cdHelper, envVar var, argvManager manager);
  void executeGeneral(std::string inputCommand,
                      changeDir cdHelper,
                      envVar var,
                      argvManager manager);
};
