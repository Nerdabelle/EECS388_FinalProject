import serial
import csv
import time

ser = serial.Serial('/dev/ttyAMA1', 115200)

with open('milestone2.csv', 'r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    for row in csv_reader:
        angle = str(row[0])
        speed = str(row[1])
        duration = str(row[2])
        command_str = "angle: " + angle + "speed: " + speed + "duration: " + duration
        ser.write(bytes(command_str, 'utf-8'))
        time.sleep(2)

ser.close()
