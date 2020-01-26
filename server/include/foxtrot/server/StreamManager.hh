#pragma once
#include <map>
#include <future>
#include <rttr/variant.h>
#include <foxtrot/server/DeviceHarness.h>
#include <string>
#include <foxtrot/server/Device.h>

using future_variant = std::future<rttr::variant>;
using promise_variant = std::promise<rttr::variant>;

using std::string;

namespace foxtrot
{
    struct Stream 
    {
        unsigned threadid;
        string streamcomment;
        unsigned nticks;
        Capability cap;
        std::vector<rttr::variant> capargs;

    private:
        rttr::variant execute(DeviceHarness& harness);
    };


    class StreamThread
    {
    public:
        void add_stream(Stream& strm, unsigned streamid);
        void delete_stream(unsigned streamid);
        future_variant get_stream_result(unsigned streamid, DeviceHarness& harness);
    private:
        void run_tick();
        std::map<unsigned, Stream&> _strms;
        std::map<unsigned, promise_variant> _promises;
    };

    class StreamManager
    {
    public:
        StreamManager(std::shared_ptr<DeviceHarness> harness);
        unsigned create_stream(Stream&& strm);
        void destroy_stream(unsigned streamid);

        unsigned create_stream_thread(unsigned tick_ms, const string& threadcomment);
        void destroy_stream_thread(unsigned threadid);

        future_variant poll_stream(unsigned streamid);

    private:
        std::shared_ptr<DeviceHarness> _harness;
        std::map<unsigned, Stream> _streams_map;
    };

}
