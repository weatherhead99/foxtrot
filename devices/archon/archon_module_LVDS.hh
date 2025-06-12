#pragma once

#include "archon_modules.h"
#include "archon_GPIO.h"

namespace foxtrot
{
  namespace devices
  {
    class archon;

    class ArchonLVDS : public ArchonModule, archonGPIO, ArchonChannelBoundsChecker<16>
    {
      friend class ArchonModule;
      RTTR_ENABLE(ArchonModule)

      public:
      virtual const string getTypeName() const override;

      void setLVDSLabel(int channel, const string& val);
      string getLVDSLabel(int channel);
      ~ArchonLVDS();

    protected:
      ArchonLVDS(std::weak_ptr<archon>& arch, const archon_module_info& modinf);
      

    };

  }

}
