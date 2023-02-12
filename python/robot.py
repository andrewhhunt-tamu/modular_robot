import multiprocessing
import robot_comms
from time import sleep
from datetime import datetime

# Module addresses
motor_front_left = 1
motor_front_right = 2
motor_rear_left = 3
motor_rear_right = 4
arm_lift = 5
arm_gripper = 6
sensor_left = 7
sensor_right = 8



class robot:
    def __init__(self, comms_queue : multiprocessing.Queue) -> None:
        #motor directions
        self.MOTOR_FORWARD = 1
        self.MOTOR_REVERSE = 2
        self.MOTOR_COAST = 3

        #states
        self.STOPPED = 0
        self.MOVING = 1
        self.ROTATING = 2
        self.FORWARD = 3
        self.REVERSE = 4
        self.CLOCKWISE = 5
        self.COUNTERCLOCKWISE = 6
        
        self.comms = robot_comms.robot_comms("/dev/ttyS0", 115200, 33)
        self.state = self.STOPPED
        self.direction = self.STOPPED
        self.speed = 0
        self.comms_queue = comms_queue
        
        
        

    def uart_txrx():
        pass
    
    
    def send_motor_commands(self, left_dir, right_dir, speed, left_check, right_check):
        self.comms.send_frame(motor_front_left, [left_dir, speed, left_check])
        sleep(0.002)
        self.comms.send_frame(motor_front_right, [right_dir, speed, right_check])
        sleep(0.002)
        self.comms.send_frame(motor_rear_left, [left_dir, speed, left_check])
        sleep(0.002)
        self.comms.send_frame(motor_rear_right, [right_dir, speed, right_check])
        
        # Grab responses
        responses = []
        response_recieved = [0, 0, 0, 0]
        
        for x in range(4):
            responses.append(self.comms.receive_frame(self.comms.MOTOR_FRAME))
            
        
        
        for x in range(4):
            if responses[x][0] == (x + 1):
                response_recieved[x] = 1
            else:
                self.stop_robot()
        
                
        return response_recieved 
        
        for response in responses:
            if response[0] < 0:
                return -3
            elif response[0] == motor_front_left or response[0] == motor_rear_left:
                if response[1] == left_check:
                    if response[2] == left_dir:
                        if response[3] == speed:
                            response_recieved[response[0]-1] = 1
                        else:
                            return -2
                    else:
                        return -1
            elif response[0] == motor_front_right or response[0] == motor_rear_right:
                if response[1] == right_check:
                    if response[2] == right_dir:
                        if response[3] == speed:
                            response_recieved[response[0]-1] = 1
                        else:
                            return -2
                    else:
                        return -1
            else:
                return -3
            
        for response in response_recieved:
            if response != 1:
                return -3
            
        return 1

    def stop_robot(self):
        direction = self.MOTOR_COAST
        speed = 1
        check = direction ^ speed
        
        status = self.send_motor_commands(direction, direction, speed, check, check)
        
        self.state = self.STOPPED
        self.speed = 0
        
        if status == -2:
            return 1
        else:
            return status

        

    def move_robot(self, direction, speed):
        '''
            Returns 1 if all the motors in the correct mode
            -1 if direction value is bad
            -2 if speed value is bad
            -3 if not all the motors responded

            Check the direction and speed that is requested
            if those values are good, send them to the motors
            Check the returned values from the motors to see if they are in the correct mode
            If any are wrong, tell all motors to stop
        '''
        '''
        with open('rblog.txt', 'w') as f:
            
            current_time = datetime.now().strftime("%H:%M:%S")
            f.write('Time: {}'.format(current_time))
            f.close()
        '''
        
        '''
        # Need to flush out comms queue
        while not self.comms_queue.empty():
            self.comms_queue.get()
        '''
        
        
        if direction == self.MOTOR_FORWARD:
            left_direction = self.MOTOR_FORWARD
            right_direction = self.MOTOR_REVERSE
        elif direction == self.MOTOR_REVERSE:
            left_direction = self.MOTOR_REVERSE
            right_direction = self.MOTOR_FORWARD
        else:
            return -1

        if speed > 100 or speed < 0:
            return -2
        
        left_check = left_direction ^ speed
        right_check = right_direction ^ speed
        
        status = self.send_motor_commands(left_direction, right_direction, speed, left_check, right_check)
        
        self.state = self.MOVING
        self.speed = speed
        self.direction = direction
        
        return status
        
        
        # testing return
        if status > 0:
            return 'direction: {}, speed: {}'.format(direction, speed)

        return 1
        
        # need to add in receiving the frames to check
        '''
        # check comms queue
        responses = []
        all_responded = 1
        
        for x in range(4):
            if self.comms_queue.empty():
                all_responded = 0
                break

            responses.append(self.comms_queue.get(block=True, timeout=0.01))
        
        if all_responded:
            for response in responses:
                pass
        else:
            self.stop_robot()
            return -3
        ''' 

        return 1

    def rotate_robot(self, direction, speed):
        '''
            Similar to the move robot function, but in this case spins the motors so that
            the robot will spin instead of move
        '''
        
        if direction == self.MOTOR_FORWARD:
            rotate_direction = self.MOTOR_FORWARD
        elif direction == self.MOTOR_REVERSE:
            rotate_direction = self.MOTOR_REVERSE
        else:
            return -1
        
        if speed > 100 or speed < 0:
            return -2
        
        check = direction ^ speed
        
        return self.send_motor_commands(rotate_direction, rotate_direction, speed, check, check)

    def move_arm(self, angle):
        pass

    def toggle_gripper(self):
        pass

    def read_sensor(self):
        '''
            Requests measured distance from the sensor modules and returns
            a list of the distances
        '''
        
        self.comms.send_frame(sensor_left, [1])
        distance1 = self.comms.receive_frame()[0]
        self.comms.send_frame(sensor_right, [1])
        distance2 = self.comms.receive_frame()[0]

        return [distance1, distance2]