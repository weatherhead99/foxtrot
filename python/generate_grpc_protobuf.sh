#!/bin/bash

python -m grpc_tools.protoc -I ../server/ --python_out=./foxtrot/ --grpc_python_out=./foxtrot/ ../server/foxtrot.proto