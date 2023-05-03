import serial
import csv
import time

ser = serial.Serial('/dev/ttyAMA1', 115200)


with open('Data1.csv', 'r') as file:
    with open('Data2.csv', 'r') as file2:
        reader = csv.reader(file, delimiter=',')
        reader2 = csv.reader(file2, delimiter=',')
        next(reader) #skip first column
        next(reader2) #skip first column
        for row1, row2 in zip(reader, reader2):
            angle = str(row1[0])
            speed = str(row1[1])
            duration = str(row1[2])
            command_str = "a:" + angle + " s:" + speed + " d:" + duration + "\n"
            print(command_str)
            ser.write(bytes(command_str, 'utf-8'))
            angle = str(row2[0])
            speed = str(row2[1])
            duration = str(row2[2])
            command_str = "a:" + angle + " s:" + speed + " d:" + duration + "\n"
            print(command_str)
            ser.write(bytes(command_str, 'utf-8'))
            time.sleep(int(duration)+1)

ser.close()

