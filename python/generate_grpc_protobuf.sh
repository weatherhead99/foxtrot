#!/bin/bash
env python3 -m grpc_tools.protoc -I ../core/proto --python_out=./pyfoxtrot/ --grpc_python_out=./pyfoxtrot/ ../core/proto/foxtrot.proto 

env python3 -m grpc_tools.protoc -I ../core/proto --python_out=./pyfoxtrot/ ../core/proto/ft_types.proto ../core/proto/ft_error.proto ../core/proto/ft_auth.proto ../core/proto/ft_capability.proto ../core/proto/ft_streams.proto

sed -i -E 's/^import.*_pb2/from . \0/' ./pyfoxtrot/*.py
