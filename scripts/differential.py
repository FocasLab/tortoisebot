#!/usr/bin/env python3

import rospy
from geometry_msgs.msg import Twist
from tortoisebot_mini.msg import Diff
import time
from math import pi

diff_msg = Diff()

motor_rpm = 90             # Max rpm of motor on full voltage 
wheel_diameter = 0.065     # In meters
wheel_separation = 0.12    # In meters

wheel_radius = wheel_diameter / 2
circumference_of_wheel = 2 * pi * wheel_radius
max_speed = (circumference_of_wheel * motor_rpm) / 60   # m/sec
max_angle = (max_speed * 2) / wheel_separation          # rad/sec

def stop():
    global diff_pub, diff_msg

    diff_msg.lrpm.data = 0
    diff_msg.rrpm.data = 0
    diff_msg.ldir.data = 1
    diff_msg.rdir.data = 1
    
    diff_pub.publish(diff_msg)
    rospy.loginfo(diff_msg)


def wheel_vel_executer(left_speed, right_speed):
    global diff_pub, diff_msg, motor_rpm, circumference_of_wheel
    
    # Calculate target RPM for each wheel
    left_rpm = min(max((abs(left_speed) / circumference_of_wheel) * 60, 0), motor_rpm)
    right_rpm = min(max((abs(right_speed) / circumference_of_wheel) * 60, 0), motor_rpm)

    diff_msg.lrpm.data = int(left_rpm)
    diff_msg.rrpm.data = int(right_rpm)
    
    if left_speed >= 0 :
        diff_msg.ldir.data = 0
    else :
        diff_msg.ldir.data = 1
        
    if right_speed >= 0 :
        diff_msg.rdir.data = 1
    else :
        diff_msg.rdir.data = 0
    diff_pub.publish(diff_msg)
    rospy.loginfo(diff_msg)

    
def callback(data):
    global wheel_radius, wheel_separation
    
    linear_vel = data.linear.x
    angular_vel = data.angular.z
    
    VrplusVl = 2 * linear_vel
    VrminusVl = angular_vel * wheel_separation
    
    right_vel = (VrplusVl + VrminusVl) / 2
    left_vel = VrplusVl - right_vel
    
    if left_vel == 0.0 and right_vel == 0.0:
        stop()
    else:
     wheel_vel_executer(left_vel, right_vel)
        
def listener():
    global diff_pub
    
    rospy.init_node('cmdvel_listener', anonymous=False)
    rospy.Subscriber("/cmd_vel", Twist, callback)
    diff_pub = rospy.Publisher('diff', Diff, queue_size=10)
    rospy.spin()

if __name__ == '__main__':
    print('Tortoisebot Differential Drive Initialized with following Params-')
    print(f'Motor Max RPM: {motor_rpm} RPM')
    print(f'Wheel Diameter: {wheel_diameter} m')
    print(f'Wheel Separation: {wheel_separation} m')
    print(f'Robot Max Speed: {max_speed} m/sec')
    print(f'Max Angular Speed: {max_angle} rad/sec')
    listener()
