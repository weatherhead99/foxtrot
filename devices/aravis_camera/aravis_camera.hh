#pragma once
#include <string>
#include <memory>
#include <vector>
#include <variant>

#include <foxtrot/Device.h>
#include <rttr/registration>
#include <foxtrot/Logging.h>
#include <arv.h>



using std::string;
using std::unique_ptr;

template <typename T>
struct _ArvDeleter
{
  void operator()(T* ptr)
  { g_object_unref(ptr);
  }

};

template<typename T> using ArvPtr = std::unique_ptr<T, _ArvDeleter<T>>;



namespace foxtrot
{
  class DeviceHarness;

  namespace devices
  {

    class AravisCameraImpl;
    class AravisCamera;
    class AravisCameraFactoryImpl;

    using GenicamFeatureVariant = std::variant<std::string, bool, double, long int>;
    using GenicamBoundsVariant = std::variant<
      std::array<long int, 3>,
      std::array<double, 3>>;

    
    enum class GenicamNodeTypes :  unsigned short
      {
	BOOL = 0,
	  INT = 1,
	  FLOAT = 2,
	  STRING = 3,
	  ENUM = 4
	  };

    struct AravisCameraDeviceInfo
    {
      std::string model;
      std::string vendor;
      std::string address;
      std::string serial_number;
      std::string protocol;
    };
    
    class AravisCameraFactory : public Device
    {
      RTTR_ENABLE(Device)
    public:
      AravisCameraFactory(const std::shared_ptr<DeviceHarness>& harness = nullptr,
			  unsigned max_image_store = 50);
      ~AravisCameraFactory();

      static std::vector<string> discover();
      static AravisCameraDeviceInfo info(const string& device_id);

      std::vector<std::tuple<int, string>> get_open_devices();

      
      int open_in_harness(const string& device_id, const string& comment="");


      void set_fake_interface_enable(bool onoff);

      void setup_fake_camera(const string& serialno);
      void setup_fake_camera(const string& serialno, const string& genicampath);
      
      unique_ptr<AravisCameraFactoryImpl> _impl;
      friend class AravisCamera;

      std::tuple<int, string> test_tuple() const;

      const std::vector<unsigned char>& retrieve_image_data(std::size_t imhandle) const;
      void drop_image_data(std::size_t imhandle);
      void clear_image_data();
      std::size_t n_stored_images() const;

    protected:
      void notify_device_removal(int id);
      std::size_t deposit_image(std::vector<unsigned char>&& data);
      

    private:
      foxtrot::Logging lg;
    };


    class AravisImageBase
    {
      RTTR_ENABLE()
      public:
      unsigned short width;
      unsigned short height;
      long unsigned timestamp;
      long unsigned system_timestamp;
      long unsigned frameid;
      bool has_chunks;
      int x_padding;
      int y_padding;
      unsigned pixel_format;
    };

    class AravisImage : public AravisImageBase
    {
      RTTR_ENABLE(AravisImageBase)
      public:
      std::vector<unsigned char> data;
    };

    class AravisImageSummary : public AravisImageBase
    {
      RTTR_ENABLE(AravisImageBase)
    public:
      std::size_t handle;
      bool is_ready;
    };
    
    
    class AravisCamera : public Device
    {
      RTTR_ENABLE(Device)
      
    public:
      friend class AravisCameraFactory;

      ~AravisCamera() override;

      void destroy_self();

      GenicamFeatureVariant get_feature_value(const string& name);
      virtual void set_feature_value(const string& name, GenicamFeatureVariant val);
      GenicamBoundsVariant get_feature_bounds(const string& name);
      std::vector<std::string> get_feature_enumeration_values(const string& name);

      
      static std::unique_ptr<AravisCamera> create(const string& device_id, std::weak_ptr<DeviceHarness> harness, AravisCameraFactory* factory);

      std::string download_genicam_xml();
      ArvPtr<ArvBuffer> acquire_single(long unsigned timeout_us);

      virtual AravisImage acquire_retrieve_single_blocking(long unsigned timeout_us);
      virtual AravisImageSummary acquire_single_blocking(long unsigned timeout_us);
      

    protected:
      void setHarnessId(int id);
      AravisCamera(const string& device_id, std::weak_ptr<DeviceHarness> harness);      unique_ptr<AravisCameraImpl> _impl;

    private:
      void fill_image_summary(AravisImageBase& polyim, ArvBuffer* ptr);

      foxtrot::Logging lg;

    };

    class AravisGvCamera: public AravisCamera
    {
      RTTR_ENABLE(AravisCamera)
      public:
      static std::unique_ptr<AravisCamera> create(const string& device_id, std::weak_ptr<DeviceHarness> harness, AravisCameraFactory* factory=nullptr);

      void set_feature_value(const string& name, GenicamFeatureVariant val) override;
      AravisImage acquire_retrieve_single_blocking(long unsigned timeout_us) override;
      AravisImageSummary acquire_single_blocking(long unsigned timeout_us) override;
      
      bool is_controller();
      void enter_control();
      void exit_control();

      void set_auto_control(bool onoff);
      bool get_auto_control() const;
	
      
      
      private:
      AravisGvCamera(const string& device_id, std::weak_ptr<DeviceHarness> harness);
      foxtrot::Logging lg;
      
    };
    

  }


}
