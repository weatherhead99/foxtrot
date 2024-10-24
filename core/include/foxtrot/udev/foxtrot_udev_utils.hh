#pragma once
#include <concepts>
#include <libudev.h>
#include <pthread.h>
#include <utility>
#include <string_view>
#include <iterator>
#include <optional>
#include <stdexcept>

namespace foxtrot
{

  namespace detail
  {
    using std::invocable;
    using std::is_invocable;

    void noop(auto* ctxt_ptr) {};
    

    template<typename T, auto NewFunc,
	     auto RefFunc = noop<T>,
	     auto UnrefFunc = noop<T>,
	     auto DelFunc = UnrefFunc>
    requires requires
      {
	std::is_invocable_v<decltype(RefFunc), T*>;
	std::is_invocable_v<decltype(UnrefFunc), T*>;
	std::is_invocable_v<decltype(DelFunc), T*>;
      }
    class ExternalRefCountInterface
    {
    public:

      template<typename... Ts>
      ExternalRefCountInterface(Ts&&... args) { _ctxt_ptr = NewFunc(args...);
	if(_ctxt_ptr == nullptr)
	  throw std::bad_alloc();
      };

      ExternalRefCountInterface(T* existing_ptr, bool ref=true) { _ctxt_ptr = existing_ptr;
	if(ref)
	  RefFunc(_ctxt_ptr);
      }

      ExternalRefCountInterface() { _ctxt_ptr = nullptr;};
      
      ~ExternalRefCountInterface() {
	if(_ctxt_ptr != nullptr)
	  {
	    if constexpr(UnrefFunc != DelFunc)
	      UnrefFunc(_ctxt_ptr);
	    DelFunc(_ctxt_ptr);
	  }
      }

      ExternalRefCountInterface(const ExternalRefCountInterface& other)
      {
	_ctxt_ptr = other._ctxt_ptr;
	RefFunc(_ctxt_ptr);
      };

      ExternalRefCountInterface(ExternalRefCountInterface&& other)
      {
	_ctxt_ptr = other._ctxt_ptr;
      }

      ExternalRefCountInterface& operator=(ExternalRefCountInterface& other)
      {
	if(this != &other)
	  {
	    _ctxt_ptr = other._ctxt_ptr;
	    RefFunc(_ctxt_ptr);
	  }
	return *this;
      }

      ExternalRefCountInterface& operator=(ExternalRefCountInterface&& other)
      {
	if(this != &other)
	  {
	    _ctxt_ptr = other._ctxt_ptr;
	    other._ctxt_ptr = nullptr;
	  }
	return *this;
      }

      T* get() { return _ctxt_ptr;};

    private:
      T* _ctxt_ptr;
    };

  }


  using udev_context = detail::ExternalRefCountInterface<udev, udev_new,
							 udev_ref, udev_unref>; 

  using udev_enum_rc = detail::ExternalRefCountInterface<udev_enumerate,
							 udev_enumerate_new, udev_enumerate_ref, udev_enumerate_unref>;

  using std::string;


  using udevListEntry = std::pair<std::string, std::optional<std::string>>;
  
  class udev_list_iterator
  {
    friend class udev_list;
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = udevListEntry;

    udevListEntry operator*();
    udev_list_iterator& operator++();
    udev_list_iterator operator++(int);

    friend bool operator==(const udev_list_iterator& a, const udev_list_iterator& b);
    friend bool operator!=(const udev_list_iterator& a, const udev_list_iterator& b);

    udev_list_iterator(const udev_list_iterator& other);
     

  private:
    udev_list_iterator(udev_list_entry* ptr);
    udev_list_entry* _ptr;
    

  };

  bool operator==(const udev_list_iterator& a, const udev_list_iterator& b);
  bool operator!=(const udev_list_iterator& a, const udev_list_iterator& b);


  class udev_list
  {
    friend class udev_enum;
    friend class udev_device;
    
  public:
    udev_list_iterator begin();
    udev_list_iterator end();
  private:
    udev_list_entry* _start_ptr;
  };

  
  class udev_enum
  {
  public:
    udev_enum(udev_context& ctxt);
    udev_enum& match_subsystem(const string& expr);
    udev_enum& match_property(const string& propname, const string& propvalue);

    udev_list scan_devices();

  private:
    udev_enum_rc _udev_enum_rc;
    void check_throw_udev_return(int ret);

  };


  using udev_device_rc = detail::ExternalRefCountInterface<udev_device, detail::noop<udev_device>,
							   udev_device_ref, udev_device_unref>;



  //forward declare
  namespace detail {
    struct _udev_map_read_interface_proxy;
  }

  
  class udev_device
  {
  public:
    udev_device(udev_context& ctxt, const std::string& syspath);
    udev_list properties();

    detail::_udev_map_read_interface_proxy property();
    

    friend struct detail::_udev_map_read_interface_proxy;

  private:
    udev_device_rc _dev;

  };

  namespace detail {
    using _udev_lookup_funptr = const char* (*) (::udev_device*, const char*);
    
    struct _udev_map_read_interface_proxy
    {
    private:
      _udev_lookup_funptr _lookup_fun = nullptr;
      foxtrot::udev_device& _dev;

    public:
      std::string operator[](const std::string& k)
      {
	return _lookup_fun(_dev._dev.get(), k.c_str());
      }

      _udev_map_read_interface_proxy(auto lookup_fun, foxtrot::udev_device& dev)
	: _lookup_fun(lookup_fun), _dev(dev)
      {
      }


      
    };
  }
    

  
  
}
