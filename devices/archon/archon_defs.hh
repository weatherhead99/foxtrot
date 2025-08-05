#pragma once
#include <string>
#include <optional>
#include <variant>


namespace foxtrot
{
  namespace devices
  {
    struct archon_driverprop
   {
     auto operator<=>(const archon_driverprop& other) const = default;
     bool operator<(const archon_driverprop& other) const = default;

     unsigned chan;
     std::optional<std::string> label = std::nullopt;
     double slew_slow;
     double slew_fast;
     unsigned source;
     bool enable;
   };

   struct archon_biasprop {
     auto operator<=>(const archon_biasprop& other) const = default;
     bool operator<(const archon_biasprop& other) const = default;
     double Vset;
     std::optional<std::string> label = std::nullopt;
     std::string name;
     std::optional<double> Iset = std::nullopt;
     std::optional<bool> enable = std::nullopt;
     double Imeas;
     double Vmeas;
     unsigned order;
   };
    
    enum class archon_gpio_source : short unsigned
      {
	low = 0,
	high = 1,
	clocked = 2,
	VCPU = 3,
      };


    
    struct archon_gpioprop
    {
	archon_gpio_source source;
      std::optional<std::string> label = std::nullopt;
	std::string name;
	bool direction;
    };
    

    using ArchonModuleProp = std::variant<archon_biasprop,archon_driverprop>;

    
  }
}
