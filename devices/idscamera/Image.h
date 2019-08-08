#pragma once

#include <vector>
#include <foxtrot/server/Device.h>
#include <foxtrot/Logging.h>
#include <rttr/registration>



namespace foxtrot{
    namespace devices {
        
        class Image {
            RTTR_ENABLE()
            public:
                //Constructors
                Image (int width, int height, int bitsperpixel);
                Image(const Image& other);
                Image();
                
                //Destructors
                ~Image();
                
                //Methods
                int getWidth() const;
                int getHeight() const;
                
                //Properties
                int width;
                int height;
                int bitsperpixel;
                int datasize;
                int currentbufferid;
                std::vector<unsigned char> rawData;
                foxtrot::Logging _lg;
                
            private:
                //Methods
                void lockMemory();
                void unlockMemory();
                char* getRawDataPointer();
                
                //Properties
                bool lockedMemory;
        };

    }
}
