from __future__ import print_function
import dweepy
import serial
import time

mySerial = serial.Serial("/dev/ttyACM0", 9600, timeout=1)
#print("Start")
while True:
    testStr=mySerial.readline()[:-2]
    #print(testStr)
    if testStr == "MPU6050":
        print(testStr)
        acceler = mySerial.readline()[:-2]
        print(acceler)
        dweepy.dweet_for('egans',{'Acceleration': acceler})
        time.sleep(1)
        gyroscope = mySerial.readline()[:-2]
        print(gyroscope)
        dweepy.dweet_for('egans',{'Gyroscope': gyroscope})
        time.sleep(1)
        print()
    if testStr == "BMP280":
        print(testStr)
        BMPtemperature = mySerial.readline()[:-2]
        print(BMPtemperature + "C")
        dweepy.dweet_for('egans',{'BMPtemperature': BMPtemperature})
        time.sleep(1)
        pressure = mySerial.readline()[:-2]
        print(pressure + "Pa")
        dweepy.dweet_for('egans',{'Pressure': pressure})
        time.sleep(1)
        altitude = mySerial.readline()[:-2]
        print(altitude + "m")
        dweepy.dweet_for('egans',{'Altitude': altitude})
        time.sleep(1)
        print()
    if testStr == "DHT22":
        print("you fucking made it")
        print(testStr)
        humidity = mySerial.readline()[:-2]
        print(humidity + "%")
        dweepy.dweet_for('egans',{'Humidity': humidity})
        time.sleep(1)
        DHTtemperature = mySerial.readline()[:-2]
        print(DHTtemperature + "C")
        dweepy.dweet_for('egans',{'DHTtemperature': DHTtemperature})
        time.sleep(1)
        print()