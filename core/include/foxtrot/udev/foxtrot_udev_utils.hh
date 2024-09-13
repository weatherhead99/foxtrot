#pragma once
#include <concepts>
#include <libudev.h>
#include <utility>
#include <string_view>
#include <iterator>

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
      ExternalRefCountInterface(Ts&&... args) { _ctxt_ptr = NewFunc(args...);};
      ~ExternalRefCountInterface() {
	UnrefFunc(_ctxt_ptr);
	DelFunc(_ctxt_ptr);
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


      T* const get() { return _ctxt_ptr;};

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
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = udevListEntry;

    udevListEntry operator*();
    udev_list_iterator& operator++();
    udev_list_iterator& operator++(int);

    friend bool operator==(const udev_list_iterator& a, const udev_list_iterator& b);
    friend bool operator!=(const udev_list_iterator& a, const udev_list_iterator& b);

  private:
    udevListEntry(udev_list_entry* ptr);
    

  };

  bool operator==(const udev_list_iterator& a, const udev_list_iterator& b);
  bool operator!=(const udev_list_iterator& a, const udev_list_iterator& b);

  
  class udev_enum
  {
  public:
    udev_enum(udev_context& ctxt);
    udev_enum& match_subsystem(const string& expr);

    udev_device_list scan_devices();


  private:
    udev_enum_rc _udev_enum_rc;

  };
  
  
  
}
