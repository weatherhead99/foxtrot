#include "aravis_camera.hh"
#include <arv.h>
#include <foxtrot/DeviceError.h>
#include <foxtrot/DeviceHarness.h>
#include <foxtrot/ft_union_helper.hh>
#include <foxtrot/ft_tuple_helper.hh>
#include <stdexcept>
#include <unordered_map>


using namespace foxtrot::devices;


const static std::unordered_map<std::string, GenicamNodeTypes> _nodetypemap =
  {
    {"Integer", GenicamNodeTypes::INT},
    {"Enumeration", GenicamNodeTypes::ENUM},
    {"Float", GenicamNodeTypes::FLOAT},
    {"Boolean", GenicamNodeTypes::BOOL}
  };

GenicamNodeTypes get_feature_type(ArvGcNode* node, foxtrot::Logging& lg)
{
  std::string domtag(arv_dom_element_get_tag_name(&node->parent_instance));
  auto atit = _nodetypemap.find(domtag);

  lg.strm(sl::debug) << "domtag: [" << domtag << "]";

  if(atit == _nodetypemap.end())
    throw std::logic_error("no match for tag type in known map");
  return atit->second;
}


auto GErrDeleter = [] (GError* err){ g_error_free(err);};

struct GErrAdapt
{
  GErrAdapt() {}
  ~GErrAdapt()
  {
    if(_errptr != nullptr)
      g_error_free(_errptr);
  }
    
  GError** pass()  {  return &_errptr;}

  void check()
  {
    if(_errptr != nullptr)
      {
	int code = _errptr->code;
	std::string err = _errptr->message;

	std::ostringstream oss;
	oss << "GError! Code: " << code << ", message: " << err;
	throw foxtrot::DeviceError(oss.str());
      }
  }

private:
  GError* _errptr = nullptr;

};



template<typename T> using ArvPtr = std::unique_ptr<T, _ArvDeleter<T>>;


namespace foxtrot {
  namespace devices {

  class AravisCameraFactoryImpl
  {
  public:
    std::weak_ptr<DeviceHarness> harness;
    ArvPtr<ArvFakeCamera> fakecamera;
    std::unordered_map<int, std::string> _open_devices;
    std::map<std::size_t, std::vector<unsigned char>> _stored_images;
    unsigned max_image_store;
  };

    
class AravisCameraImpl {

  friend class AravisCamera;
  friend class AravisCaemraFactory;
  
public:
  ArvPtr<ArvDevice> _devptr;
  
  std::weak_ptr<DeviceHarness> harness;
  AravisCameraFactory* _factory_backptr = nullptr;
  
  ~AravisCameraImpl() = default;
  ArvCamera* camptr = nullptr;
  bool auto_control = false;

  std::string _device_id;
  
  AravisCameraImpl(const string& device_id)
  {
    GErrAdapt err;

    _devptr = ArvPtr<ArvDevice>(arv_open_device(device_id.c_str(), err.pass()));
    err.check();

    camptr = arv_camera_new_with_device(_devptr.get(), err.pass());
    err.check();

  }

  int id = -1;

  ArvGcNode* get_feature(const string& name)
  {
    ArvGcNode* featnode = arv_device_get_feature(_devptr.get(), name.c_str());
    if(featnode == nullptr)
      throw std::out_of_range("unknown feature");
    return featnode;
  }
  
  
};
  }
}

AravisCameraFactory::AravisCameraFactory(const std::shared_ptr<DeviceHarness>& harness, unsigned max_image_store)
  : Device(nullptr, "AravisCameraFactory"), lg("AravisCameraFactory")
{
  _impl = std::make_unique<AravisCameraFactoryImpl>();
  _impl->harness = harness;
  _impl->max_image_store = max_image_store;
}

AravisCameraFactory::~AravisCameraFactory()
{}    

std::vector<std::tuple<int, string>> AravisCameraFactory::get_open_devices()
{
  std::vector<std::tuple< int, string>> out;
  out.reserve(_impl->_open_devices.size());
  for(auto& item : _impl->_open_devices)
      out.push_back(item);

  return out;
}


std::vector<string> AravisCameraFactory::discover()
{

  arv_update_device_list();

  auto n_dev = arv_get_n_devices();

  std::vector<string> out;
  out.reserve(n_dev);
 
  for(int i=0; i < n_dev; i++)
    {
      out.push_back(arv_get_device_id(i));
    }

  return out;
}

auto get_dev_idx = [] (const string& device_id) {
  auto devs = AravisCameraFactory::discover();
  auto iter = std::find(devs.begin(), devs.end(), device_id);
  if(iter == devs.end())
    throw std::out_of_range("coulsn't find device with identifier: " + device_id);

  return std::distance(devs.begin(), iter);

 };


using foxtrot::devices::AravisCameraDeviceInfo;

AravisCameraDeviceInfo AravisCameraFactory::info(const string& device_id)
{
  AravisCameraDeviceInfo out;

  foxtrot::Logging statlg("AravisCameraFactory_static");
  
  auto idx = get_dev_idx(device_id);
  statlg.strm(sl::debug) << "idx"  << idx ;

  auto idstr = std::string(arv_get_device_id(idx));
  statlg.strm(sl::debug) << "id string check: " << idstr;
  
  out.model = std::string(arv_get_device_model(idx));
  out.address = std::string(arv_get_device_address(idx));
  out.vendor = std::string(arv_get_device_vendor(idx));
  out.serial_number = std::string(arv_get_device_serial_nbr(idx));
  out.protocol = std::string(arv_get_device_protocol(idx));
  
  
  return out;
}


int AravisCameraFactory::open_in_harness(const string& device_id, const string& comment)
{

  auto devinfo = info(device_id);
  std::unique_ptr<AravisCamera> lptr;
  
  if(devinfo.protocol == "GigEVision")
    lptr = AravisGvCamera::create(device_id, _impl->harness, this);
  else
    lptr =  AravisCamera::create(device_id, _impl->harness, this);

  if( auto hptr = _impl->harness.lock())
    {
      auto idx =  hptr->AddDevice(std::move(lptr));

      AravisCamera* dev = static_cast<AravisCamera*>(hptr->GetDevice(idx));
      dev->setHarnessId(idx);
      if(comment.size() > 0)
	dev->setDeviceComment(comment);
      _impl->_open_devices.emplace(idx, device_id);
      
      return idx;
    }
  else {
    throw foxtrot::DeviceError("couldn't get handle to DeviceHarness when creating AravisCamera");  
  }
}

void AravisCameraFactory::set_fake_interface_enable(bool onoff)
{
  if(onoff)
    arv_enable_interface("Fake");
  else
    arv_disable_interface("Fake");
  
}

void AravisCameraFactory::setup_fake_camera(const string& serialno)
{
  _impl->fakecamera = ArvPtr<ArvFakeCamera>( arv_fake_camera_new(serialno.c_str()));
  if(_impl->fakecamera ==nullptr)
    throw std::runtime_error("couldn't setup fake camera!");

}

void AravisCameraFactory::setup_fake_camera(const string& serialno, const string& genicampath)
{
  _impl->fakecamera = ArvPtr<ArvFakeCamera>(arv_fake_camera_new_full(serialno.c_str(), genicampath.c_str()));
  if(_impl->fakecamera == nullptr)
    throw std::runtime_error("couldn't setup fake camera!");
 
}

void AravisCameraFactory::notify_device_removal(int id)
{

  _impl->_open_devices.extract(id);
  
}

const std::vector<unsigned char>& AravisCameraFactory::retrieve_image_data(std::size_t imhandle) const
{
  //will throw out of range if imhandle isn't valid
  const auto& dat = _impl->_stored_images.at(imhandle);
  return dat;
};

void AravisCameraFactory::drop_image_data(std::size_t imhandle)
{
  auto it = _impl->_stored_images.find(imhandle);
  if(it == _impl->_stored_images.end())
    throw std::out_of_range("can't find image handle in data store");

  _impl->_stored_images.erase(it);
}

void AravisCameraFactory::clear_image_data() { _impl->_stored_images.clear(); }

std::size_t AravisCameraFactory::n_stored_images() const
{
  return _impl->_stored_images.size();
}


std::size_t AravisCameraFactory::deposit_image(std::vector<unsigned char>&& data)
{
  if(_impl->_stored_images.size() == _impl->max_image_store)
    throw std::runtime_error("image store is full! Drop some images first!");

  std::size_t next_handle = 0;
  if(_impl->_stored_images.size() > 0)
    next_handle = (_impl->_stored_images.rbegin()->first) + 1;
  
  _impl->_stored_images.emplace(std::make_pair(next_handle, std::move(data)));

  return next_handle;
};




AravisCamera::AravisCamera(const string &device_id,
                           std::weak_ptr<DeviceHarness> harness)
  : Device(nullptr, ""), lg("AravisCamera")
{
  std::ostringstream oss;
  oss << "AravisCamera_" << device_id;
  setDeviceComment(oss.str());

  _impl = std::make_unique<AravisCameraImpl>(device_id);
  
  //do impl setup etc

  _impl->harness = harness;
  _impl->_device_id = device_id;

};

AravisCamera::~AravisCamera() {

	
  //NOTE: do not call this, it causes a cascade of Remove() calls which destroys all devices
  //destroy_self();
}

std::string AravisCamera::download_genicam_xml()
{
  std::size_t sz;
  std::string xml(arv_device_get_genicam_xml(_impl->_devptr.get(), &sz));
  
  return xml;
};


void AravisCamera::destroy_self()
{
  lg.strm(sl::debug) << "destroy_self called";
  
    if(auto hptr = _impl->harness.lock())
    {

      if(_impl->id == -1)
	{
	lg.Info("impl has id -1, already deleted");
	return;
	}

      if(not _impl->_factory_backptr)
	lg.strm(sl::warning) << "camera doesn't have a pointer back to factory that created it!";
      else
	_impl->_factory_backptr->notify_device_removal(_impl->id);

      hptr->RemoveDevice(_impl->id);


      _impl->id = -1;
    }
  else
    {

      lg.Error("couldn't get Harness lock when destructing AravisCamera! Likely a programming error!");
    }

};


template<class>
inline constexpr bool always_false_v = false;


void AravisCamera::set_feature_value(const string& name, GenicamFeatureVariant val)
{

  auto* featnode = _impl->get_feature(name);
  ArvDevice* devptr = _impl->_devptr.get();
  GErrAdapt err;

  std::visit( [&err, devptr, &name](auto&& v) {
    using T = std::decay_t<decltype(v)>;

    if constexpr(std::is_same_v<T, long int>)
      arv_device_set_integer_feature_value(devptr, name.c_str(), v, err.pass());
    else if constexpr(std::is_same_v<T, std::string>)
      arv_device_set_string_feature_value(devptr, name.c_str(), v.c_str(), err.pass());
    else if constexpr(std::is_same_v<T, bool>)
      arv_device_set_boolean_feature_value(devptr, name.c_str(), v, err.pass());
    else if constexpr(std::is_same_v<T, double>)
      arv_device_set_float_feature_value(devptr, name.c_str(), v, err.pass());
    else
      static_assert(always_false_v<T>, "non exhaustive visitor");

    err.check();

  }, val);


}



GenicamFeatureVariant AravisCamera::get_feature_value(const string& name)
{

  auto* featnode = _impl->get_feature(name);
  auto tp = get_feature_type(featnode, lg);
  
  GenicamFeatureVariant out;

  auto* devptr = _impl->_devptr.get();
  GErrAdapt err;
  switch(tp)
    {
    case(GenicamNodeTypes::INT):
      out = arv_device_get_integer_feature_value(devptr, name.c_str(), err.pass());
      break;
    case(GenicamNodeTypes::FLOAT):
      out = arv_device_get_float_feature_value(devptr, name.c_str(), err.pass());
      break;
    case(GenicamNodeTypes::ENUM):
    case(GenicamNodeTypes::STRING):
      out = arv_device_get_string_feature_value(devptr, name.c_str(), err.pass());
      break;
    case(GenicamNodeTypes::BOOL):
      out = arv_device_get_boolean_feature_value(devptr, name.c_str(), err.pass());
      break;
    default:
      lg.strm(sl::error) << "looking at feature name: " << name;
      throw std::logic_error("type was not found, foxtrot programming error");
      
    }

  err.check();
  return out;
}


std::vector<std::string> AravisCamera::get_feature_enumeration_values(const string& name)
{
  auto* featnode = _impl->get_feature(name);
  GErrAdapt err;

  unsigned n;
  auto* enumptr = reinterpret_cast<ArvGcEnumeration*>(featnode);
  
  const char** enumvals = arv_gc_enumeration_dup_available_string_values(enumptr, &n, err.pass());
									 
  err.check();

  std::vector<std::string> out;
  out.reserve(n);

  for(std::size_t i=0; i< n; i++)
    {
      out.push_back(enumvals[i]);
    }

  g_free(enumvals);
  return out;
  
}

std::tuple<int, string> AravisCameraFactory::test_tuple() const
{

  return std::make_tuple(1, "hello");
}


GenicamBoundsVariant AravisCamera::get_feature_bounds(const string& name)
{
  auto* featnode = _impl->get_feature(name);
  auto tp = get_feature_type(featnode, lg);
  ArvDevice* devptr = _impl->_devptr.get();
  GErrAdapt err;

  switch(tp)
    {
    case(GenicamNodeTypes::INT):
      std::array<long int, 3> outi;
      arv_device_get_integer_feature_bounds(devptr, name.c_str(), &outi[0], &outi[1], err.pass());
      err.check();
      outi[2] = arv_device_get_integer_feature_increment(devptr, name.c_str(), err.pass());
      err.check();
      return outi;
    case(GenicamNodeTypes::FLOAT):
      std::array<double, 3> outf;
      arv_device_get_float_feature_bounds(devptr, name.c_str(), &outf[0], &outf[1], err.pass());
      err.check();
      outf[2] = arv_device_get_float_feature_increment(devptr, name.c_str(), err.pass());
      err.check();
      return outf;
    default:
      lg.strm(sl::error) << "feature name:" << name << " called for boundaries but has type:" << static_cast<unsigned short>(tp);
      throw std::logic_error("invalid feature type for getting bounds!");
    }

}


std::unique_ptr<foxtrot::devices::AravisCamera> AravisCamera::create(const string& device_id,						   std::weak_ptr<DeviceHarness> harness, AravisCameraFactory* factory)
{
  auto dev =  std::unique_ptr<AravisCamera>(new AravisCamera(device_id, harness));
  dev->_impl->_factory_backptr = factory;
  return dev;
}



void AravisCamera::setHarnessId(int id) { _impl->id = id; };


ArvPtr<ArvBuffer> AravisCamera::acquire_single(long unsigned timeout_us)
{
  GErrAdapt err;
  if(_impl->camptr == nullptr)
    throw std::logic_error("camptr is null, foxtrot programming error!");
  
  ArvBuffer* buf = arv_camera_acquisition(_impl->camptr,  timeout_us, err.pass());
  err.check();

  if(buf == nullptr)
    throw std::logic_error("GError passed but buffer is nullptr. No idea how this could happen!");

  auto out = ArvPtr<ArvBuffer>(buf);

  auto status = arv_buffer_get_status(out.get());
  if(status != ARV_BUFFER_STATUS_SUCCESS)
    {
      lg.strm(sl::error) << "ARV buffer error, error code is: " << status;
      throw foxtrot::DeviceError("aravis buffer error, check foxtrot logs for error code");
    }

  
  return out;
}

AravisImage AravisCamera::acquire_retrieve_single_blocking(long unsigned timeout_us)
{
  auto buffer = acquire_single(timeout_us);
  AravisImage out;
  fill_image_summary(out, buffer.get());
  
  std::size_t sz;
  auto* datptr = reinterpret_cast<const unsigned char*>(arv_buffer_get_data(buffer.get(), &sz));
  out.data.resize(sz);
  std::copy(datptr, datptr+sz, out.data.begin());

  return out;
};




void AravisCamera::fill_image_summary(AravisImageBase& polyim, ArvBuffer* ptr)
{
  polyim.height = arv_buffer_get_image_height(ptr);
  polyim.width = arv_buffer_get_image_width(ptr);
  polyim.timestamp = arv_buffer_get_timestamp(ptr);
  polyim.system_timestamp = arv_buffer_get_system_timestamp(ptr);
  polyim.frameid = arv_buffer_get_frame_id(ptr);
  polyim.pixel_format = arv_buffer_get_image_pixel_format(ptr);
  arv_buffer_get_image_padding(ptr, &polyim.x_padding, &polyim.y_padding);
  polyim.has_chunks = arv_buffer_has_chunks(ptr);
}


AravisImageSummary AravisCamera::acquire_single_blocking(long unsigned timeout_us)
{
  auto buffer = acquire_single(timeout_us);
  AravisImageSummary out;
  fill_image_summary(out, buffer.get());

  std::size_t sz;
  auto* dataptr = reinterpret_cast<const unsigned char*>(arv_buffer_get_data(buffer.get(), &sz));

  if(_impl->_factory_backptr == nullptr)
    throw std::runtime_error("pointer to Factory is null. This should never happen. Can't store image");

  std::vector<unsigned char> datacpy;
  datacpy.resize(sz);
  std::copy(dataptr, dataptr+sz, datacpy.begin());

  auto imhandle = _impl->_factory_backptr->deposit_image(std::move(datacpy));
  out.handle = imhandle;
  out.is_ready = true;
				 
  return out;
}


std::unique_ptr<foxtrot::devices::AravisCamera> AravisGvCamera::create(const string& device_id, std::weak_ptr<DeviceHarness> harness, AravisCameraFactory* factory)
{
  auto dev = std::unique_ptr<AravisGvCamera>(new AravisGvCamera(device_id, harness));
  dev->_impl->_factory_backptr = factory;
  return dev;

}

AravisGvCamera::AravisGvCamera(const string& device_id, std::weak_ptr<DeviceHarness> harness): AravisCamera(device_id, harness), lg("AravisGvCamera")
{

  
}

bool AravisGvCamera::is_controller()
{
  auto devptr = _impl->_devptr.get();
  ArvGvDevice* gvptr = reinterpret_cast<ArvGvDevice*>(devptr);

  return arv_gv_device_is_controller(gvptr);
}


void AravisGvCamera::enter_control()
{
  auto devptr = _impl->_devptr.get();
  ArvGvDevice* gvptr = reinterpret_cast<ArvGvDevice*>(devptr);

  GErrAdapt err;
  bool worked = arv_gv_device_take_control(gvptr, err.pass());

  if(!worked)
    lg.strm(sl::error) << "take control didn't worked";

  err.check();
  
}


void AravisGvCamera::set_auto_control(bool onoff)
{
  _impl->auto_control = onoff;
}

bool AravisGvCamera::get_auto_control() const
{
  return _impl->auto_control;
}


struct GvControlAdapt
{
  GvControlAdapt(AravisGvCamera& cam) : _camref(cam) {
    initcontrol = _camref.is_controller();

    if(!initcontrol)
      _camref.enter_control();
  }

  ~GvControlAdapt() {
    if(_camref.is_controller() and not initcontrol)
      {
	_camref.exit_control();
      }
  }
  bool initcontrol;
  AravisGvCamera& _camref;
  
};


void AravisGvCamera::exit_control()
{
  auto devptr = _impl->_devptr.get();
  ArvGvDevice* gvptr = reinterpret_cast<ArvGvDevice*>(devptr);

  GErrAdapt err;
  bool worked = arv_gv_device_leave_control(gvptr, err.pass());

  if(!worked)
    lg.strm(sl::error) << "take control didn't worked";

  err.check();
}


void AravisGvCamera::set_feature_value(const string& name, GenicamFeatureVariant val)
{

  GvControlAdapt cntr(*this);
  AravisCamera::set_feature_value(name, val);
}

AravisImage AravisGvCamera::acquire_retrieve_single_blocking(long unsigned timeout_us)
{
  GvControlAdapt cntr(*this);
  return AravisCamera::acquire_retrieve_single_blocking(timeout_us);
}

AravisImageSummary AravisGvCamera::acquire_single_blocking(long unsigned timeout_us)
{
  GvControlAdapt cntr(*this);
  return AravisCamera::acquire_single_blocking(timeout_us);
}




RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::AravisCameraFactory;
  using foxtrot::devices::AravisCamera;
  using foxtrot::devices::AravisCameraDeviceInfo;
  using foxtrot::devices::AravisGvCamera;
  using foxtrot::devices::GenicamBoundsVariant;

  
  foxtrot::register_union<GenicamFeatureVariant>();
  foxtrot::register_union<GenicamBoundsVariant>();
  foxtrot::register_tuple<std::tuple<int, string>>();

  rttr::type::register_converter_func([](const std::string& s, bool& ok) -> GenicamFeatureVariant {  ok = true; return GenicamFeatureVariant(s);  });

  rttr::type::register_converter_func([](long unsigned u, bool& ok) -> GenicamFeatureVariant{ ok = true; return GenicamFeatureVariant(static_cast<long int>(u));});

  registration::enumeration<GenicamNodeTypes>("foxtrot::devices::GenicamNodeTypes")
    (value("BOOL", GenicamNodeTypes::BOOL),
     value("INT", GenicamNodeTypes::INT),
     value("FLOAT", GenicamNodeTypes::FLOAT),
     value("STRING", GenicamNodeTypes::STRING),
     value("ENUM", GenicamNodeTypes::ENUM));
  
  registration::class_<AravisCamera>("foxtrot::devices::AravisCamera")
    .method("set_feature_value", &AravisCamera::set_feature_value)(parameter_names("name", "val"))
    .method("get_feature_value", &AravisCamera::get_feature_value)(parameter_names("name"))
    .method("get_feature_bounds", &AravisCamera::get_feature_bounds)(parameter_names("name"))
    .method("destroy_self", &AravisCamera::destroy_self)
    .method("genicam_xml", &AravisCamera::download_genicam_xml)
    .method("get_feature_enumeration_values", &AravisCamera::get_feature_enumeration_values)
    .method("acquire_single_blocking", &AravisCamera::acquire_single_blocking)
    .method("acquire_retrieve_single_blocking", &AravisCamera::acquire_retrieve_single_blocking)
	    ;



  registration::class_<AravisCameraFactory>("foxtrot::devices::AravisCameraFactory").method("discover", &AravisCameraFactory::discover)
    .method("open_in_harness", &AravisCameraFactory::open_in_harness)(parameter_names("device_id", "comment"))
    .method("info", &AravisCameraFactory::info)(parameter_names("device_id"))
    .method("set_fake_interface_enable",  &AravisCameraFactory::set_fake_interface_enable)
    .property_readonly("open_devices", &AravisCameraFactory::get_open_devices)
    .method("retrieve_image_data", &AravisCameraFactory::retrieve_image_data)
    (parameter_names("imhandle"), metadata("streamdata", true))
    .method("drop_image_data", &AravisCameraFactory::drop_image_data)
    .method("clear_image_data", &AravisCameraFactory::clear_image_data)
    .property_readonly("n_stored_images", &AravisCameraFactory::n_stored_images)

    ;



  registration::class_<AravisCameraDeviceInfo>("foxtrot::devices::AravisCameraDeviceInfo")
    .constructor()(policy::ctor::as_object)
    .property("model", &AravisCameraDeviceInfo::model)
    .property("vendor", &AravisCameraDeviceInfo::vendor)
    .property("address", &AravisCameraDeviceInfo::address)
    .property("serial_number", &AravisCameraDeviceInfo::serial_number)
    .property("protocol", &AravisCameraDeviceInfo::protocol);
    
  

  registration::class_<AravisGvCamera>("foxtrot::devices::AravisGvCamera")
    .property_readonly("is_controller", &AravisGvCamera::is_controller)
    .method("enter_control", &AravisGvCamera::enter_control)
    .method("exit_control", &AravisGvCamera::exit_control)
    .property("auto_control", &AravisGvCamera::get_auto_control,
	      &AravisGvCamera::set_auto_control)(parameter_names("onoff"))

    ;

  registration::class_<AravisImageBase>("foxtrot::Devices::AravisImageBase")
    .constructor()(policy::ctor::as_object)
    .property_readonly("width", &AravisImage::width)
    .property_readonly("height", &AravisImage::height)
    .property_readonly("timestamp", &AravisImage::timestamp)
    .property_readonly("system_timestamp", &AravisImage::system_timestamp)
    .property_readonly("frameid", &AravisImage::frameid)
    .property_readonly("has_chunks", &AravisImage::has_chunks)
    .property_readonly("x_padding", &AravisImage::x_padding)
    .property_readonly("y_padding", &AravisImage::y_padding);

  registration::class_<AravisImage>("foxtrot::Devices::AravisImage")
    .constructor()(policy::ctor::as_object)
    .property_readonly("data", &AravisImage::data);

  registration::class_<AravisImageSummary>("foxtrot::Devices::AravisImageSummary")
    .constructor()(policy::ctor::as_object)
    .property_readonly("handle", &AravisImageSummary::handle)
    .property_readonly("is_ready", &AravisImageSummary::is_ready);

}


