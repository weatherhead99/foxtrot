#pragma once
#include "archon_modules.h"
#include "archon_legacy.h"
namespace foxtrot
{
  namespace devices
  {
    class archon;
    
    class ArchonModuleLegacy : public ArchonModule
      {
	friend class foxtrot::devices::archon_legacy;
	RTTR_ENABLE(ArchonModule)
	public:
	string getID() const;
	const std::array<char,3>& getVersion() const;
	short unsigned getRev() const;

	double getTemp();
	[[deprecated]] archon& getArchon();
	short unsigned getmodpos();
       
      };

  }
}
