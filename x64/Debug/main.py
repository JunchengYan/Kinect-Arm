# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import serial
import serial.tools.list_ports

def serial_comm():
    #for i in serial.tools.list_ports.comports():
     #   print(i)

    filename = "F:\\KinectControl\\angles.txt"
    fo = open(filename, mode='r')
    line = fo.read(37)

    ser = serial.Serial('COM5', 9600 , timeout = 10)

    #print(ser.port)
    #print(ser.is_open)

    print("Send data: ", line.encode())
    #len = ser.write(line.encode())
    len = ser.write(line.encode())
    print("The length of send data is: ", len)


    #read_data = ser.read(50)
    #print("Read data from bluetooth: ", read_data)
    print()

    '''
    #ser = serial.Serial('com6', 9600)
    #ser.open()


    flag = ser.is_open

    if flag:
        print('Open success\n')
        ser.close()
    else:
        print('Open Error\n')

    ser.write("*#0#0#0#0#0#0#0#2#0".encode('utf-8'))

    print(ser.read(20))
    '''


def jjcc():
    #测试相互调用
    filename = "C:\\Users\\Zhang\\Desktop\\bluetooth\\bluetooth\\angles.txt"
    fo = open(filename, mode='r')
    line = fo.read(10)
    print(line)
    fo.close()

def test():
    print("hello")


#def main():
 #   serial_comm()


#serial_comm()


# See PyCharm help at https://www.jetbrains.com/help/pycharm/
