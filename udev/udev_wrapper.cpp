#include "udev_wrapper.hh"

#include <libudev.h>

foxtrot::UdevContext::UdevContext()
{
    ctxt_ = udev_new();
}

foxtrot::UdevContext::~UdevContext()
{
    udev_unref(ctxt_);
}

foxtrot::UdevContext::UdevContext(foxtrot::UdevContext& other)
{
    ctxt_ = other.ctxt_;
    udev_ref(ctxt_);
    
}


foxtrot::UdevList::UdevList(foxtrot::UdevEnumerate& enumerate)
: enumerate_(enumerate)
{
    first_entry_ = udev_enumerate_get_list_entry(enumerate.enum_);
    list_entry_ = first_entry_;
}



foxtrot::UdevList::iterator::iterator(foxtrot::UdevList& list)
: list_(list)
{
    
}

std::pair<std::string, foxtrot::UdevDevice> foxtrot::UdevList::iterator::operator*()
{
    std::string path(udev_list_entry_get_name(list_.list_entry_));
 
    UdevDevice dev(list_.enumerate_.ctxt_,path);
    
    return std::make_pair(path,dev);
}

foxtrot::UdevDevice::UdevDevice(foxtrot::UdevContext& ctxt, 
                                const std::string& path)
{
    dev_ = udev_device_new_from_syspath(ctxt.ctxt_,path.c_str());
}

foxtrot::UdevDevice::UdevDevice(const foxtrot::UdevDevice& other)
{
    dev_ = other.dev_;
    udev_device_ref(dev_);
}


foxtrot::UdevDevice::~UdevDevice()
{
    udev_device_unref(dev_);
}



foxtrot::UdevEnumerate::UdevEnumerate(foxtrot::UdevContext& ctxt)
: ctxt_(ctxt)
{
    udev_enumerate_new(ctxt_.ctxt_);    
}

foxtrot::UdevEnumerate::~UdevEnumerate()
{
    udev_enumerate_unref(enum_);
}

foxtrot::UdevEnumerate::UdevEnumerate(foxtrot::UdevEnumerate& other)
{
    ctxt_ = other.ctxt_;
    enum_ = other.enum_;
    udev_enumerate_ref(enum_);
    
}







