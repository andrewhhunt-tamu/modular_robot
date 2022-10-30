from email.utils import localtime
from tracemalloc import start
import serial

ser = serial.Serial("/dev/ttyS0", 115200)



address = 3
state = int(input('Motor state: '))
speed = int(input('Motor speed: '))
check = state ^ speed
end = 126
print('Sending:\naddress: {}, state: {}, speed: {}'.format(address, state, speed))

ser.write((128 + address).to_bytes(1,'big'))
ser.write(state.to_bytes(1,'big'))
ser.write(speed.to_bytes(1,'big'))
ser.write(check.to_bytes(1,'big'))
ser.write(end.to_bytes(1,'big'))

#rec_state = int.from_bytes(ser.read(), 'big')
#rec_speed = int.from_bytes(ser.read(), 'big')

start_time = localtime()

while(ser.in_waiting == 0):
    if ((localtime - start_time) > 5):
        break

total_time = localtime() - start_time
rec_check = int.from_bytes(ser.read(), 'big')

#print('Received:\nstate: {}, speed {}'.format(rec_state, rec_speed))

print('Needed check: {}, Received check {}'.format(check, rec_check))
print('Took ', total_time, ' seconds')

