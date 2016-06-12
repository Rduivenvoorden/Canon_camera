### send_Canon_trigger.py ###
#
# - starts serial connection with Arduino over USB
# - sends a desired pulse width to the Arduino for it to send to the Canon CHDK
# - records the time at which the trigger was sent for geotagging purposes post-flight
# 
# Rikky Duivenvoorden June 12, 2016
# UTAT UAV
#

import serial
import csv
from time import sleep
from datetime import datetime

# open serial connection
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=10)

# --- DEFINE CAPTURE PARAMETERS --- #
delay = 240 # initial delay in seconds, between program start and image triggering
num_pictures = 250 # number of pictures to take, if less than 1, infinite loop
time_between_frames = 0.8 # time in seconds at which a new trigger signal is sent to arduino 
pulse_width = 50 # time of the pulse applied in the signal in milliseconds, (1 byte: 0 - 255 only for fast serial communication)

# --- TIME LOGGING --- #
enable_time_logging = 1 # records the trigger times in a csv file, for later geotagging
# the directory and filename where the trigger event times are stored
csv_filename = '/home/odroid/Canon_camera/Canon_images.csv'

# wait with triggering for the time period delay (for pre-flight and takoff)
sleep(delay)

if num_pictures < 1:
  # then take pictures in an infinite loop
  while 1:
    # write the desired pulswidth in milliseconds to the Arduino
    arduino.write(chr(pulse_width))
    
    # record at which date and time the trigger is sent
    x = datetime.strftime(datetime.now(), '%Y-%m-%d_%H-%M-%S-%f')

    # append the date and time of the trigger event 
    with open(csv_filename,'ab') as f:
      writer = csv.writer(f)
      # write date and time up to the millisecond (full string length includes microseconds)
      writer.writerow([x[:-3]])
    
    # wait until next frame
    sleep(time_between_frames)
  
else:
  # take num_pictures pictures
  for picture_num in range(0,num_pictures):
    # write the desired pulswidth in milliseconds to the Arduino
    arduino.write(chr(pulse_width))
    
    # record at which date and time the trigger is sent
    x = datetime.strftime(datetime.now(), '%Y-%m-%d_%H-%M-%S-%f')

    # append the date and time of the trigger event 
    with open(csv_filename,'ab') as f:
      writer = csv.writer(f)
      # write date and time up to the millisecond (full string length includes microseconds)
      writer.writerow([x[:-3]])
    
    # wait until next frame
    sleep(time_between_frames)
  
  # close serial connection
  arduino.close()
