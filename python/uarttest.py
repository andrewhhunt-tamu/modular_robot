import serial
#import serial.rs485

ser = serial.Serial("/dev/ttyS0", 115200)
#ser.rs485_mode = serial.rs485.RS485Settings()

#ser = serial.rs485.RS485(port='/dev/ttyS0', baudrate=115200)
#ser.write('testing'.encode('utf-8'))

send = 'A'.encode('utf-8')
print('Sending: ' + str(send))
ser.write(send)
x = ser.read()

#while(x == b'\x00'):
#    x = ser.read()

print('Received: ' + str(x))
