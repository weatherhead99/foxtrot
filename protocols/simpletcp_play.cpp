#include "simpleTCP.h"

int main(int argc, char** argv)
{
  parameterset params{ {"port", 10000u} ,
    {"addr", "10.0.0.1"}
  };
  
  simpleTCP proto(&params);
  
  
  proto.Init(nullptr);
  
};