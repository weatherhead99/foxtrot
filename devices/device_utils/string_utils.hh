#pragma once
#include <string>
#include <locale>

namespace foxtrot
{
  namespace devices
  {
    inline void ltrim(std::string& s)
    {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(),
				      [] (auto ch)
				      { return !std::isspace(ch);}));
    }
    inline void rtrim(std::string& s)
    {
      s.erase(std::find_if(s.rbegin(), s.rend(), [] (auto ch)
      {return !std::isspace(ch);}).base(), s.end());
    }

    inline void trim(std::string& s)
    {
      ltrim(s);
      rtrim(s);
    }
  }
  } // namespace foxtrot
  
