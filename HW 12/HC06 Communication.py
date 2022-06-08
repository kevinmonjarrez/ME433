import board
import busio # for the uart functions
import digitalio

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

uart = busio.UART(tx=board.GP0, rx=board.GP1, baudrate=115200,  timeout= 1) # open the uart

i = 0 # a counter 

while True:
    s = "hello " + str(i) + "\r\n" # make a string to print
    i = i + 1
    uart.write(bytearray(s)) # uart prints are of type byte
    data = uart.readline() # read until newline or timeout
    print(data) # print the data received
    
    if data is not None:
        print(str(data,'utf-8')) # print the data as a string
        if str(data,'utf-8').strip() == "on": # strip() removes the hidden \r
            led.value = 1
        else:
            led.value = 0
