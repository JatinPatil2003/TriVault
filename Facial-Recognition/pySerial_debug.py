import serial
import time
import numpy
port = serial.Serial(port = 'COM8', baudrate = 9600, timeout= 100)


# inp = "1"
while True:

    # inpu = input("asasd  : ")
    # print(type(inpu))
    # port.write(inpu.encode())
    # print(inpu.encode())

    inp = "1"
    print(type(inp))
    port.write(inp.encode())
    # port.write(ret.encode())
    print(inp.encode())
    time.sleep(1)
    input = "0"
    print(type(input))
    port.write(input.encode())
    # port.write(ret.encode())
    print(input.encode())
    time.sleep(1)


