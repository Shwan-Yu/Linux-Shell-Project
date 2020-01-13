#include <map>
#include <string>

#include "cmdParser.h"
class envVar {
 public:
  //comments in .cpp
  static std::map<std::string, std::string> var_map;
  std::string inp_cmd;
  std::string name;
  std::string val;
  envVar();
  void setV(std::string v_name, std::string v_val);
  void expV(std::string v_name);
  void revV(std::string v_name);
  void setCmdInp(std::string inp) { inp_cmd = inp; }
  bool setKVPair(std::string inp_cmd, char * method);
};
