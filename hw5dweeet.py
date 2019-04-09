from __future__ import print_function
import dweepy
import serial
import time

mySerial = serial.Serial("/dev/ttyACM0", 9600)
while True:
    testStr=mySerial.readline()[:-2]
    if testStr == "MPU6050":
        acceler = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'Acceleration': acceler})
        time.sleep(1)
        gyroscope = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'Gyroscope': gyroscope})
        time.sleep(1)
    elif testStr == "BMP280":
        BMPtemperature = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'BMPtemperature': BMPtemperature})
        time.sleep(1)
        pressure = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'Pressure': pressure})
        time.sleep(1)
        altitude = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'Altitude': altitude})
        time.sleep(1)
    else: 
        humidity = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'Humidity': humidity})
        time.sleep(1)
        DHTtemperature = mySerial.readline()[:-2]
        dweepy.dweet_for('egans',{'DHTtemperature': DHTtemperature})
        time.sleep(1)