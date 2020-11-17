#pragma once
#include <map>
#include <future>
#include <rttr/variant.h>
#include <foxtrot/server/DeviceHarness.h>
#include <string>
#include <foxtrot/server/Device.h>
#include <chrono>

using future_variant = std::future<rttr::variant>;
using promise_variant = std::promise<rttr::variant>;
using duration = std::chrono::system_clock::duration;

using std::string;

namespace foxtrot
{
    struct Stream 
    {
        unsigned threadid;
        string streamcomment;
        unsigned nticks;
        Capability cap;
        unsigned devid;
        std::vector<rttr::variant> capargs;

    private:
        rttr::variant execute(DeviceHarness& harness, const duration& timeout);
    };


    class StreamThread : public std::enable_shared_from_this<StreamThread>
    {
    public:
        void add_stream(Stream& strm);
        void remove_stream(std::size_t streamid);
        std::vector<future_variant> get_stream_result(unsigned streamid, DeviceHarness& harness);
        const std::vector<Stream>& get_streams() const;
    private:
        void start();
        std::vector<promise_variant> run_tick();
        std::vector<Stream&> _strms;
        std::thread _thrd;
        std::condition_variable _threadrun;
        std::mutex _threadmut;
    };

    class StreamManager
    {
    public:
        StreamManager(std::shared_ptr<DeviceHarness> harness);
        
        std::shared_ptr<StreamThread> create_stream_thread(unsigned tick_ms, const string& threadcomment);
        void destroy_stream_thread(unsigned threadid);
        std::weak_ptr<StreamThread> get_stream_thread(unsigned threadid);

    private:
        std::shared_ptr<DeviceHarness> _harness;
        std::map<unsigned, StreamThread> _threads_map;
    };

}
