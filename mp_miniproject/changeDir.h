#include <string>

#include "envVar.h"
class changeDir {
 public:
  static std::string curDir;

  changeDir();
  int generatePrompt(std::string);
  void changeD(envVar var, char ** args);
  void absolutePath(char ** argv, envVar myVar);
  void saveCWD();
  std::string getCWD();
};
