#!/bin/bash
env python3 -m grpc_tools.protoc -I ../core/proto --python_out=./pyfoxtrot/ --grpc_python_out=./pyfoxtrot/ ../core/proto/foxtrot.proto 

env python3 -m grpc_tools.protoc -I ../core/proto --python_out=./pyfoxtrot/ ../core/proto/ft_types.proto ../core/proto/ft_error.proto ../core/proto/ft_auth.proto

sed -i -e 's/import foxtrot_pb2 as foxtrot__pb2/import pyfoxtrot.foxtrot_pb2 as foxtrot__pb2/g' ./pyfoxtrot/foxtrot_pb2_grpc.py
