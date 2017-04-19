#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 19 17:49:43 2017

@author: weatherill
"""

import os
os.environ['LD_LIBRARY_PATH'] = '/local/home/weatherill/lib/'


from foxtrot.TelemetryCatcher import TelemetryCatcher

tc = TelemetryCatcher("tcp://127.0.0.1:50052","test_telem")
tc.subscribe("counter", lambda x : 0)

