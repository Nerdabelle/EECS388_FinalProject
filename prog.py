import serial
import csv
import time

ser = serial.Serial('/dev/ttyAMA1', 115200)


with open('milestone2.csv', 'r') as csv_file:
    with open('milestone2.csv', 'r') as csv_file2:
        csv_reader = csv.reader(csv_file, delimiter=',')
        csv_reader2 = csv.reader(csv_file2, delimiter=',')
        for row in csv_reader:
            for row2 in csv_reader2:
                angle = str(row[0])
                speed = str(row[1])
                duration = str(row[2])
                command_str = "a:" + angle + " s:" + speed + " d:" + duration + "\n"
                ser.write(bytes(command_str, 'utf-8'))
                angle = str(row2[0])
                speed = str(row2[1])
                duration = str(row2[2])
                command_str = "a:" + angle + " s:" + speed + " d:" + duration + "\n"
                ser.write(bytes(command_str, 'utf-8'))
                time.sleep(int(duration) + 1)

ser.close()
