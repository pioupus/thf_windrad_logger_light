#!/usr/bin/env python
import csv
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import threshold
from datetime import datetime
import matplotlib.dates as mdates
import os

def convertdigits_to_ampere(digits):
    ampere = digits

    ampere *= 0.04132
    ampere += 0.3
    ampere = threshold(ampere, 0.6)
    return ampere

calibData_digit =   [0, 0.5,    0.6,    0.7,    0.8,    0.9,    1.0,    5.5,    5.6]
calibData_curr =    [7, 7,      7,     10,     12,     14,     17,    126,    128  ]



for root, dirs, files in os.walk("daten/"):
    data_current_A = []
    data_date_time = []
    for name in files:
        filename = os.path.join(root, name)
        print (filename)
        
        with open(filename, 'rb') as csvfile:
            winddaten = csv.reader(csvfile, delimiter=';', quotechar='|')

            for row in winddaten:
                datetime_object = datetime.strptime(row[0], '%Y.%m.%d %H:%M:%S')
                data_date_time.append(datetime_object)
                data_current_A.append(float(row[1]))

    np_data_current_A = np.array(data_current_A)
    np_data_current_A = convertdigits_to_ampere(np_data_current_A)
    
    avg_val = np.average(np_data_current_A)
    max_val = np.max(np_data_current_A)
    
    wind_data_start_datetime = min(data_date_time)
    wind_data_stop_datetime = max(data_date_time)
    
    logging_interaval = wind_data_stop_datetime-wind_data_start_datetime
    
    print("start: "+str(wind_data_start_datetime))
    print("stop: "+str(wind_data_stop_datetime))
    print("logging dauer: "+str(logging_interaval))
    dauer_hour = logging_interaval.total_seconds()/3600.0
    
    ladung_ah = dauer_hour*avg_val
    
    txt_avg = "avg_val[A]:" +str(round(avg_val,2))
    txt_max = "max_val[A]:" +str(round(max_val,2))
    txt_charge = "Ladung[Ah]:" +str(round(ladung_ah,2))
    print(txt_avg)
    print(txt_max)
    print(txt_charge)
    
    entrie_count = len(data_date_time)
    
    
    f = plt.figure()
    
    #plt.subplot(211)
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%m.%d.%Y %H:%M'))
    plt.gca().xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.plot(data_date_time,np_data_current_A,'o')
    plt.gcf().autofmt_xdate()

    plt.ylabel('current[A]')
    plt.xlabel('Zeit')
    plt.title('Strom ueber Zeit')
    plt.text(data_date_time[int(entrie_count/10)], 4*max_val/15, txt_avg)
    plt.text(data_date_time[int(entrie_count/10)], 3*max_val/15, txt_max)
    plt.text(data_date_time[int(entrie_count/10)], 2*max_val/15, txt_charge)
    
    #plt.subplot(212)
    #plt.plot(calibData_curr,calibData_digit)
    #plt.xlabel('digit')
    #plt.ylabel('current[A]')
    #plt.title('Strom vs Messwert(Kalibrierung)')
    f.savefig("stromplot"+wind_data_stop_datetime.strftime('%Y_%m_%d')+".pdf", dpi=3000)
    plt.show()
    