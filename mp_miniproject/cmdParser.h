#include <string.h>

#include <string>
#include <vector>
class cmdParser {
 private:
  std::string inputCmd;
  //allocated char **
  char ** argv;
  size_t size;

 public:
  //function description in .cpp
  cmdParser();
  cmdParser(std::string cmd, size_t initSize);
  //rule of three
  cmdParser(const cmdParser & rhs);
  ~cmdParser();
  cmdParser & operator=(const cmdParser & rhs);
  char ** parse(const char * parseBy);
  void escape(std::string from, std::string to);
  void handleQuotes();
  void assignToArgv(std::vector<std::string> v);
};
