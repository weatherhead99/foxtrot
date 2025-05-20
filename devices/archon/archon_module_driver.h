#pragma once
#include "archon.h"
#include "archon_modules.h"
#include <format>

namespace foxtrot
{
 namespace devices
 {
     class archon;

  template<int NChannels>
  class ArchonDriverBase : public ArchonModule 
  {
    RTTR_ENABLE(ArchonModule)
    
  public:
      virtual const string getTypeName() const override;
      virtual void update_variables() override;
      
      void setLabel(int channel, const std::string& val);
      std::string getLabel(int channel);

      void setSlowSlewRate(int channel, double val);
      double getSlowSlewRate(int channel);
      
      void setFastSlewRate(int channel, double val);
      double getFastSlewRate(int channel);
      
      void setEnable(int channel, bool onoff);
      bool getEnable(int channel);
      
  private:
      ArchonDriverBase(archon& arch, short unsigned int modpos);
      std::ostringstream _oss;

      inline static constexpr void checkChannel(int channel)
      {
	if(channel < 1 || channel > NChannels)
          {
              throw std::out_of_range("invalid Driver channel number");
          };
      };
      
      
  };


   class ArchonDriver : public ArchonDriverBase<8> {
   public:
     RTTR_ENABLE();
     ArchonDriver(archon& arch, short unsigned int modpos);

   };

   class ArchonDriverX : public ArchonDriverBase<12> {
   public:
     RTTR_ENABLE();
     ArchonDriverX(archon& arch, short unsigned int modpos);


   };


   template<int NChannels>
   ArchonDriverBase<NChannels>::ArchonDriverBase(foxtrot::devices::archon& arch, unsigned short modpos)
     : ArchonModule(arch,modpos)
   {
     
   }

   template<int NChannels>
   double ArchonDriverBase<NChannels>::getFastSlewRate(int channel)
   {
     checkChannel(channel);
     return readConfigKey<double>(std::format("FASTSLEWRATE{}",channel));
   }

   template<int NChannels>
   std::string ArchonDriverBase<NChannels>::getLabel(int channel)
   {
     checkChannel(channel);
     return readConfigKey(std::format("LABEL{}",channel));
   }


   template<int NChannels>
   double ArchonDriverBase<NChannels>::getSlowSlewRate(int channel)
   {
     checkChannel(channel);
     return readConfigKey<double>(std::format("SLOWSLEWRATE{}", channel));
   }

   template<int NChannels>
   void ArchonDriverBase<NChannels>::setFastSlewRate(int channel, double val)
   {
     checkChannel(channel);
     if(val < 0.001 or val > 1000)
       throw std::out_of_range("invalid slew rate value");
     writeConfigKey(std::format("FASTSLEWRATE{}",channel), val);
   }

   template<int NChannels>
   void ArchonDriverBase<NChannels>::setLabel(int channel, const std::string& val)
   {
     checkChannel(channel);
     writeConfigKey(std::format("LABEL{}", channel), std::to_string(val));
   }

   template<int NChannels>
   void ArchonDriverBase<NChannels>::setSlowSlewRate(int channel, double val)
   {
     checkChannel(channel);
     writeConfigKey(std::format("SLOWSLEWRATE{}",channel), std::to_string(val));
   }

   
   
     
 }
    
    
    
}


