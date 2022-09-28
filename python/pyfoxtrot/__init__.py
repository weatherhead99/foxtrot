import sys
import os

#NOTE: hack to make protobuf imports work properly
FILE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(FILE_DIR, "protos"))
