import serial

ser = serial.Serial("/dev/ttyS0", 115200)



address = 3
state = int(input('Motor state: '))
speed = int(input('Motor speed: '))
print('Sending:\naddress: {}, state: {}, speed: {}'.format(address, state, speed))

ser.write((128 + address).to_bytes(1,'big'))
ser.write(state.to_bytes(1,'big'))
ser.write(speed.to_bytes(1,'big'))

rec_state = int.from_bytes(ser.read(), 'big')
rec_speed = int.from_bytes(ser.read(), 'big')

print('Received:\nstate: {}, speed {}'.format(rec_state, rec_speed))

