#pragma once
#include <string>

struct udev;
struct udev_enumerate;
struct udev_list_entry;
struct udev_device;

namespace foxtrot {
    
    
    class UdevDevice;
    class UdevEnumerate;
 
    class UdevContext
    {
    friend class UdevEnumerate;
    friend class UdevDevice;
    friend class UdevList;
    public:
        UdevContext();
        ~UdevContext();
        
        //delete copy const for now
        UdevContext(UdevContext& other);
        
        
    private:
        struct udev* ctxt_;
        
    };
    
        class UdevEnumerate
    {
        friend class UdevList;
    public:
        UdevEnumerate(UdevContext& ctxt);
        ~UdevEnumerate();
        
        UdevEnumerate(UdevEnumerate& other);
        
    private:
        UdevContext ctxt_;
        struct udev_enumerate* enum_;
    };
    
    class UdevList
    {
    public:
        class iterator
        {
            friend class UdevList;
        public:
            
            iterator(const iterator& other);
            iterator& operator=(const iterator& other);
            std::pair<std::string,UdevDevice> operator*();
            
            
            iterator& operator++();
            iterator operator++(int i);
            
            bool operator==(const iterator& other);
            bool operator!=(const iterator& other);
            
        private:
            explicit iterator(UdevList& list);
            UdevList& list_;
            
        };
        
    protected:
        
        struct udev_list_entry* list_entry_;
        struct udev_list_entry* first_entry_;
        
        UdevList(UdevEnumerate& enumerate);
        UdevEnumerate enumerate_;
                
    };
    
    class UdevDevice 
    {
        friend class UdevList::iterator;
    public:
        ~UdevDevice();
        UdevDevice(const UdevDevice& other);
    private:
        UdevDevice(UdevContext& ctxt, const std::string& path);
        udev_device* dev_;
        
    };
    

    
    

    

    
}
