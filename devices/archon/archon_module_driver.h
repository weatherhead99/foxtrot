#pragma once
#include "archon.h"
#include "archon_modules.h"
#include "archon_defs.hh"
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

      std::vector<archon_driverprop> clocks(const ssmap& statusmap) const;
    std::vector<archon_driverprop> clocks();

    virtual std::vector<ArchonModuleProp> props(const ssmap& statusmap) const override;
    
      void setLabel(int channel, const std::string& val);
      std::string getLabel(int channel);
    
      consteval unsigned n_clocks() const { return N::value;};
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

   template<typename NChannels>
   std::vector<foxtrot::devices::archon_driverprop>
   ArchonDriverBase<NChannels>::clocks(const ssmap& statusmap) const
   {
     std::vector<archon_driverprop> out;
     out.reserve(n_clocks());

     auto modpos = this->info().position;
     for(unsigned i=0; i <n_clocks(); i++)
       {
	 archon_driverprop thisprop;
	 auto findstr = std::format("MOD{}/ENABLE{}",modpos, i);
	 thisprop.enable = std::stoul(statusmap.at(findstr));
	 findstr = std::format("MOD{}/LABEL{}", modpos, i);
	 thisprop.enable = std::stoul(statusmap.at(findstr));

	 findstr = std::format("MOD{}/SLOWSLEWRATE{}", modpos, i);
	 thisprop.slew_slow = std::stod(statusmap.at(findstr));

	 findstr = std::format("MOD{}/FASTSLEWRATE{}", modpos, i);
	 thisprop.slew_fast = std::stod(statusmap.at(findstr));

	 thisprop.chan = i;

	 findstr = std::format("MOD{}/SOURCE{}", modpos, i);
	 thisprop.source = std::stoul(statusmap.at(findstr));
	 
	 out.push_back(thisprop);  
       }
     return out;
   }
   
   template<typename NChannels>
   std::vector<ArchonModuleProp>
   ArchonDriverBase<NChannels>::props(const ssmap& statusmap) const
   {
     std::vector<ArchonModuleProp> out;
     auto clockvec = this->clocks(statusmap);
     out.reserve(clockvec.size());
     out.insert(out.end(), clockvec.begin(), clockvec.end());
     return out;
   }
   
 }

}


