#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jan 14 00:24:12 2017

@author: danw
"""

#TODO: machine based config
folder = '/home/dweatherill/teststation_logs'

ATMOS_HPA = 1010

import pandas as pd
import os
import matplotlib.pyplot as plt
import matplotlib.dates as mdat
import matplotlib.ticker as mtck
from dateutil import parser
from math import modf
from datetime import datetime, timedelta

import numpy as np

def datetime_from_frac_ordinal(ordinal):
    frac_ord, greg_ord = modf(ordinal)
    dt = datetime.fromordinal(int(greg_ord))
    delta = timedelta(days=frac_ord)
    return dt + delta
    
def datetime_to_frac_ordinal(dt):
    ordinal = dt.toordinal()
    dtmidnight = datetime.fromordinal(ordinal)
    from_midnight = dt - dtmidnight
    frac_ordinal = from_midnight.total_seconds() / timedelta(days=1).total_seconds()
    return float(ordinal)  + frac_ordinal


def find_nearest_index(val,series):
    arg_nearest = np.argmin(np.abs(val - np.array(series)))
    return arg_nearest

def label_nearest_point(x,xs,ys,ax,fmtstr,col):
    arg_nearest = np.argmin(np.abs(xs - x))
    xnearest = xs[arg_nearest]
    ynearest = ys[arg_nearest]
    
    ax.annotate(fmtstr % ynearest, xy=(xnearest,ynearest),ha='left',color=col,
                xytext = (2,2), textcoords='offset points')


    print('nearest: %2.2f, %2.2f' % (xnearest,ynearest))

fls = [_ for _ in os.listdir(folder)]

#f = folder + '/' + 'holdtime2017-Jan-27.txt'
f = folder + '/' + 'pumpdown2017-Feb-06.txt'

#df = pd.read_csv(os.path.abspath(folder + '/' + fls[0]),header=0)
df = pd.read_csv(os.path.abspath(f),header=0)

datetimes = [parser.parse(_) for _ in df['date/time']]
dtfracords = np.array([datetime_to_frac_ordinal(_) for _ in datetimes])

plt.close('all')
fig,ax = plt.subplots(figsize=(14,5))


#START = 0

START_DATE = datetime(2017,2,7,11,00)
START =  find_nearest_index(START_DATE,datetimes)
#START=0

END = -1
PLOT_EVERY=1

slc = slice(START,END,PLOT_EVERY)

#normalize maximum pressure
df['pressure_pump(hPa)'][df['pressure_pump(hPa)'] > ATMOS_HPA] = ATMOS_HPA
df['pressure_cryostat(hPa)'][df['pressure_cryostat(hPa)'] > ATMOS_HPA] = ATMOS_HPA



ax.plot(datetimes[slc],df['pressure_pump(hPa)'][slc],"--",c='blue',label='Pressure at Pump')
ax.set_ylabel('Pressure (hPa)')

ax.plot(datetimes[slc],df['pressure_cryostat(hPa)'][slc],"-",c='blue',label='Pressure at Cryostat')
#
ax2 = ax.twinx()
ax2.plot(datetimes[slc],df['temperature_tank(C)'][slc],"-",c='red',label='Tank Temperature')
ax2.plot(datetimes[slc],df['temperature_stage(C)'][slc],"--",c='red',label='Stage Temperature')
#
ax2.set_ylabel('Temperature ($^\circ$C)')


#pressure formatting
fmtpres = mtck.FormatStrFormatter('%2.1e')
#ax.semilogy()       
ax.yaxis.set_major_formatter(fmtpres)
ax.yaxis.set_major_locator(mtck.LogLocator(base=10.,subs=[1.,2,5.]))

#date formatting
ax.xaxis.set_major_locator(mdat.MinuteLocator(interval=400))
fmt = mdat.DateFormatter('%d-%b-%H:%M')
ax.xaxis.set_major_formatter(fmt)


fig.autofmt_xdate()

ax.grid(axis='x',linestyle='-')

plt.minorticks_on()
plt.tight_layout()              

lns,lbs = ax.get_legend_handles_labels()
#lns2,lbs2 = ax2.get_legend_handles_labels()

#ax.legend(lns + lns2, lbs+lbs2,loc='center right')


#label major tick points
for x in ax.xaxis.majorTicks:
    loc = x.get_loc()
    
    
    label_nearest_point(loc,dtfracords,df['pressure_cryostat(hPa)'],ax,"%2.2e","blue")
    label_nearest_point(loc,dtfracords,df['pressure_pump(hPa)'],ax,"%2.2e","blue")
    label_nearest_point(loc,dtfracords,df['temperature_tank(C)'],ax2,"%2.0f","red")
    label_nearest_point(loc,dtfracords,df['temperature_stage(C)'],ax2,"%2.0f","red")
#    label_nearest_point(loc,dtfracords,df['temperature(C)'],ax2,"%2.2f","red")

