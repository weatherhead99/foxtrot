#pragma once
#include "archon.h"
#include "archon_modules.h"
#include <format>

namespace foxtrot
{
 namespace devices
 {
     class archon;


   //ugly hack because RTTR doesn't handle NTTI stuff apparently
   namespace detail
   {
     using eight_t = std::integral_constant<unsigned short, 8>;
     using twelve_t = std::integral_constant<unsigned short, 12>;
   }
   
  template<typename N>
  class ArchonDriverBase : public ArchonModule, public ArchonChannelBoundsChecker<N::value>
  {
    RTTR_ENABLE(ArchonModule)
    
  public:
      virtual const string getTypeName() const override;
      
      void setLabel(int channel, const std::string& val);
      std::string getLabel(int channel);

      void setSlowSlewRate(int channel, double val);
      double getSlowSlewRate(int channel);
      
      void setFastSlewRate(int channel, double val);
      double getFastSlewRate(int channel);
      
      void setEnable(int channel, bool onoff);
      bool getEnable(int channel);
      
  protected:
    ArchonDriverBase(std::weak_ptr<archon>& arch, const archon_module_info& modpos);      
      
  };


   
   class ArchonDriver : public ArchonDriverBase<detail::eight_t> {
     RTTR_ENABLE(ArchonDriverBase<detail::eight_t>)
   public:
     ArchonDriver(std::weak_ptr<archon>& arch, const archon_module_info& modpos);
     ~ArchonDriver();
     const string getTypeName() const override;
   };
   
   class ArchonDriverX : public ArchonDriverBase<detail::twelve_t> {
     RTTR_ENABLE(ArchonDriverBase<detail::twelve_t>)
   public:
     ~ArchonDriverX();
     ArchonDriverX(std::weak_ptr<archon>& arch, const archon_module_info&  modinfo);
     const string getTypeName() const override;

   };


   template<typename NChannels>
   ArchonDriverBase<NChannels>::ArchonDriverBase(std::weak_ptr<archon>& arch, const archon_module_info& modinfo)
     : ArchonModule(arch, modinfo)
   {
   }

   template<typename NChannels>
   double ArchonDriverBase<NChannels>::getFastSlewRate(int channel)
   {
     this->checkChannel(channel);
     return readConfigKey<double>(std::format("FASTSLEWRATE{}",channel));
   }

   template<typename NChannels>
   std::string ArchonDriverBase<NChannels>::getLabel(int channel)
   {
     this->checkChannel(channel);
     return readConfigKey(std::format("LABEL{}",channel));
   }


   template<typename NChannels>
   double ArchonDriverBase<NChannels>::getSlowSlewRate(int channel)
   {
     this->checkChannel(channel);
     return readConfigKey<double>(std::format("SLOWSLEWRATE{}", channel));
   }

   template<typename NChannels>
   void ArchonDriverBase<NChannels>::setFastSlewRate(int channel, double val)
   {
     this->checkChannel(channel);
     if(val < 0.001 or val > 1000)
       throw std::out_of_range("invalid slew rate value");
     writeConfigKey(std::format("FASTSLEWRATE{}",channel), val);
   }

   template<typename NChannels>
   void ArchonDriverBase<NChannels>::setLabel(int channel, const std::string& val)
   {
     this->checkChannel(channel);
     writeConfigKey(std::format("LABEL{}", channel), val);
   }

   template<typename NChannels>
   void ArchonDriverBase<NChannels>::setSlowSlewRate(int channel, double val)
   {
     this->checkChannel(channel);
     writeConfigKey(std::format("SLOWSLEWRATE{}",channel), val);
   }

   template<typename  NChannels>
   void ArchonDriverBase<NChannels>::setEnable(int channel, bool onoff)
   {
     this->checkChannel(channel);
     writeConfigKey(std::format("ENABLE{}",channel), onoff);
   }

   template<typename NChannels>
   bool ArchonDriverBase<NChannels>::getEnable(int channel)
   {
     this->checkChannel(channel);
     return readConfigKey<int>(std::format("ENABLE{}", channel));
   }

 }

}


