add_library(DLP_IOR4 OBJECT DLP_IOR4.cpp)
target_sources(DLP_IOR4 PUBLIC FILE_SET HEADERS FILES DLP_IOR4.hh)
ft_device_lib_configure(DLP_IOR4)
