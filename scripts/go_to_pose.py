#!/usr/bin/env python3

# Package imports
import rospy
from geometry_msgs.msg import Twist, Pose2D
from phasespace_msgs.msg import Markers
from math import pow, atan2, sqrt

class Tortoisebot:

    def __init__(self, ):
        # Creates a node with name 'tortoisebot_controller' and make sure it is a
        # unique node (using anonymous=True).
        rospy.init_node('tortoisebot_controller', anonymous=True)

        # Publisher which will publish to the topic '/cmd_vel'.
        self.velocity_publisher = rospy.Publisher('/cmd_vel', Twist, queue_size=10)

        # A subscriber to the topic '/turtle1/pose'. self.update_pose is called
        # when a message of type Pose is received.
        self.pose_subscriber = rospy.Subscriber('/phasespace/markers',
                                                Markers, self.update_pose)

        self.pose = Pose2D()
        self.rate = rospy.Rate(10)

    def update_pose(self, data):
        """Callback function which is called when a new message of type Pose is
        received by the subscriber."""
        # Put the led number as per the position on the robot.
        # dyn-> led on the dynamics point of the robot
        # ori-> led on the center of the robot
        marker_dyn = data.markers[led_dyn]
        marker_ori = data.markers[led_ori]

        self.pose.x = round(marker_dyn.x, 4)
        self.pose.y = round(marker_dyn.y, 4)
        
        dy = marker_dyn.y - marker_ori.y
        dx = marker_dyn.x - marker_ori.x
        
        self.pose.theta = round(atan2(dy, dx), 4)

    def euclidean_distance(self, goal_x, goal_y):
        """ Description: Euclidean distance between current pose and the goal.
            Input: goal_x, goal_y
            Returns: distance between current pose and the goal
        """
        # Your code goes here

        
        # Your code ends here

    def linear_vel(self, goal_x, goal_y, constant=1.5):
        """ Description: Conmputes the linear velocity for the robot based on the current pose and goal.
            Input: goal_x, goal_y, contant(Kp) value
            Returns: linear velocity 
        """
        # Your code goes here

        
        # Your code ends here 

    def steering_angle(self, goal_x, goal_y):
        """ Description: Steering angle between current pose and the goal.
            Input: goal_x, goal_y
            Returns: angle between current pose and the goal
        """
        # Your code goes here

        
        # Your code ends here

    def angular_vel(self, goal_x, goal_y, constant=6):
        """ Description: Conmputes the angular velocity for the robot based on the current pose and goal.
            Input: goal_x, goal_y, contant(Kp) value
            Returns: angular velocity 
        """
        # Your code goes here

        
        # Your code ends here

    def move2goal(self, goal_x, goal_y, distance_tolerance):
        """Description: Moves the Tortoise bot to the goal.
            Input: goal_x, goal_y, distance_tolerance
        """
        vel_msg = Twist()

        while ():
            # Your Code goes here

            
            
            # Your Code ends here

            # Publishing our vel_msg
            self.velocity_publisher.publish(vel_msg)
            # Publish at the desired rate.
            self.rate.sleep()

        # Stopping our robot after the movement is over.
        vel_msg.linear.x = 0
        vel_msg.angular.z = 0
        self.velocity_publisher.publish(vel_msg)

        # If we press control + C, the node will stop.
        rospy.spin()

if __name__ == '__main__':
    try:
        # Call the Tortoisebot class
        bot = Tortoisebot()

        # Get the input from the user.
        goal_x = float(input("Set your x goal: "))
        goal_y = float(input("Set your y goal: "))

        # Please, insert a number slightly greater than 0 (e.g. 0.01).
        distance_tolerance = input("Set your tolerance: ")

        bot.move2goal(goal_x, goal_y, distance_tolerance)

    except rospy.ROSInterruptException:
        pass