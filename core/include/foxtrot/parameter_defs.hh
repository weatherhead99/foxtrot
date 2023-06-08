#include "ExperimentalSetup.h"
#include <string>
#include <map>
#include <variant>
#include <boost/mpl/vector.hpp>

namespace foxtrot
{

  using parameter_types = boost::mpl::vector<unsigned,char, int , std::string>;
  using parameter_types_minimal = boost::mpl::vector<int,std::string>;
  using parameter = std::variant<int, unsigned, std::string>;
  using parameterset = std::map<std::string, parameter>;
  using mapofparametersets = std::map<std::string, parameterset>;
  } // namespace foxtrot
  
