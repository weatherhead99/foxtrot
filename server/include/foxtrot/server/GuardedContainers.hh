#pragma once
#include <memory>
#include <shared_mutex>
#include <algorithm>


namespace foxtrot
{
    
    template<typename T, typename Idx>
    class GuardedContainer : public std::enable_shared_from_this<GuardedContainer<T,Idx>>
    {
    public:
        using value_type = typename T::value_type;
        template<typename F>
        void for_each_readonly(F apply)
        {
           std::shared_lock lck(_mut);
           std::for_each(_var.cbegin(), _var.cend(), apply);
        };
        
        template<typename F>
        void for_each_readwrite(F apply)
        {
            std::lock_guard lck(_mut);
            std::for_each(_var.begin(), _var.end(), apply);
        }
        
        
        
        void set(const Idx& key, const value_type& val)
        {
            std::lock_guard lck(_mut);
            _var[key] = val;
        };

    protected:
        T _var;
        std::shared_mutex _mut;            
        
    };
    
    
    template<typename M> 
    class GuardedMap : public GuardedContainer<M, typename M::key_type>
    {
    public:
        using key_type = typename M::key_type;
        using value_type = typename M::value_type;
        
        void drop(const key_type& key)
        {
            std::lock_guard lck(this->_mut);
            auto it = this->_var.find(key);
            if(it == this->_var.end())
                throw std::out_of_range("no such key");
            this->_var.erase(it);
        }
        
        value_type get(const key_type& key)
        {
            std::shared_lock lck(this->_mut);
            auto it = this->_var.find(key);
            if(it == this->_var.end())
                throw std::out_of_range("no such key");
            return this->_var.at(key);
        };
        
        
    };
    
    template<class V>
    class GuardedSequential : public GuardedContainer<V, typename V::size_type>
    {
        using index_type = typename V::size_type;
        using value_type = typename V::value_type;
        
        value_type get(const index_type& idx)
        {
            std::shared_lock lck(this->_mut);
            return this->_var[idx];
        };
        
    };
    
    
}
