#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jan 14 00:24:12 2017

@author: danw
"""

folder = 'teststand_logs'
import pandas as pd
import os
import matplotlib.pyplot as plt
import matplotlib.dates as mdat
import matplotlib.ticker as mtck
from dateutil import parser

fls = [_ for _ in os.listdir(folder)]

df = pd.read_csv(os.path.abspath(folder + '/' + fls[0]),header=0)

datetimes = [parser.parse(_) for _ in df['date/time']]

plt.close('all')
fig,ax = plt.subplots(figsize=(14,5))


ax.plot(datetimes,df['pressure(hPa)'],".-",c='blue',label='Pressure at Pump')
ax.set_ylabel('Pressure (hPa)')

ax2 = ax.twinx()
ax2.plot(datetimes,df['temperature(C)'],".-",c='red',label='Stage Temperature')

ax2.set_ylabel('Temperature ($^\circ$C)')


#pressure formatting
fmtpres = mtck.FormatStrFormatter('%2.1e')
ax.semilogy()       
ax.yaxis.set_major_formatter(fmtpres)
ax.yaxis.set_major_locator(mtck.LogLocator(base=10.,subs=[1.,2.5,5.]))

#date formatting
ax.xaxis.set_major_locator(mdat.MinuteLocator(interval=30))
fmt = mdat.DateFormatter('%d-%b-%H:%M')
ax.xaxis.set_major_formatter(fmt)


fig.autofmt_xdate()

ax.grid(axis='x',linestyle='-')

plt.minorticks_on()
plt.tight_layout()              

lns,lbs = ax.get_legend_handles_labels()
lns2,lbs2 = ax2.get_legend_handles_labels()

ax.legend(lns + lns2, lbs+lbs2)