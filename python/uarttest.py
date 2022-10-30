import robot_comms

comms = robot_comms.robot_comms("/dev/ttyS0", 115200)

address = 3
state = int(input('Motor state: '))
speed = int(input('Motor speed: '))
check = state ^ speed

comms.send_frame(address, [state, speed, check])


#end = 126
print('Sending:\naddress: {}, state: {}, speed: {}'.format(address, state, speed))

rec_check = comms.receive_frame()[0]
print('Needed check: {}, Received check {}'.format(check, rec_check))

