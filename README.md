Foxtrot
=======

Foxtrot is a device server (and clients) designed for use in benchtop/small scale experiments. It is intended as a replacement for systems such as [EPICS](https://epics.anl.gov) or [Tango](https://www.tango-controls.org). It does not provide anywhere near as many features as those systems, but it runs with much less infrastructure (for example, does not require a separate database server). As such, it may be useful in situations where some kind of client-server architecture for running experimental hardware is desirable, but where available resources / effort are such that is it not jusitified to maintain a full install of one of those systems.

Foxtrot is built on top of a modern and featureful RPC framework, [gRPC](https://grpc.io). The server is written in c++14 and at present there are reasonably featureful clients available in python and c++. Clients in C and java are planned, as well as clients in MATLAB and LabVIEW based on these.

For (in progress) build instructions, see [doc/building.md](doc/building.md).
