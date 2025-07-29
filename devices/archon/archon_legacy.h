#pragma once
#include "archon.h"

namespace foxtrot
{
  namespace devices
  {
    class archon_legacy : public archon
    {
      //this class is not intended to be c++ compatible with the old archon class
      //it just retains all the declared public methods so that the client side can be
      //the same for a foxtrot server that instantiates archon_legacy. Setup file will still
      //need to be source-changed slightly (e.g. because this new class can only be created
      //as a shared_ptr)
      
      RTTR_ENABLE(archon)
      
    public:
      static std::shared_ptr<archon_legacy> create(std::shared_ptr<simpleTCPBase>&& proto);
      virtual const string getDeviceTypeName() const override;
      ~archon_legacy();

      void update_state();

      int get_rawlines(int buf);
      int get_rawblocks(int buf);
      int get_frameno(int buf);
      int get_width(int buf);
      int get_height(int buf);
      int get_mode(int buf);
      bool get_32bit(int buf);
      int get_pixels(int buf);
      std::string get_tstamp(int buf);
   
      bool isbuffercomplete(int buf);
    protected:
      archon_legacy(std::shared_ptr<simpleTCPBase> proto);
    private:
      ssmap _system;
      ssmap _status;
      ssmap _frame;
    };
  }
}
