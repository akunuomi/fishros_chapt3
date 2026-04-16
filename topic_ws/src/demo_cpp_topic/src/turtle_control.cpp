#include"rclcpp/rclcpp.hpp"
#include"geometry_msgs/msg/twist.hpp"
#include <chrono>
#include<turtlesim/msg/pose.hpp>

using namespace std::chrono_literals;

class TurtleContolNode: public rclcpp::Node
{
private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;//发布者对象的智能指针
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber_;//订阅者对象的智能指针
    double target_x_ = 1.0; // 目标位置的x坐标
    double target_y_ = 1.0; // 目标位置的y坐标
    double k_ =1.0; // 控制增益
    double max_linear_speed_ = 3.0; // 最大线速度

public:
    explicit TurtleContolNode(const std::string & node_name):Node(node_name)
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);//创建发布者对象
        subscriber_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, 
            std::bind(&TurtleContolNode::pose_callback, this, std::placeholders::_1));
        // timer_ = this->create_wall_timer(1000ms, std::bind(&TurtleContolNode::timer_callback, this));//创建定时器对象

    }
    void pose_callback(const turtlesim::msg::Pose::SharedPtr pose)
    {
        auto msg = geometry_msgs::msg::Twist();
        // 处理接收到的pose消息
        //1.获取当前位姿
        double current_x = pose->x;
        double current_y = pose->y;
        RCLCPP_INFO(this->get_logger(), "当前: x=%f, y=%f", current_x, current_y);
        //2.计算当前海龟位置和目标位置之间的误差和角度差
        auto distance =std ::sqrt(
            (target_x_ - current_x) * (target_x_ - current_x) +
            (target_y_ - current_y) * (target_y_ - current_y)
        );
        RCLCPP_INFO(this->get_logger(), "距离: %f", distance);
        auto angle = std::atan2(target_y_ - current_y, target_x_ - current_x) - pose->theta;
        RCLCPP_INFO(this->get_logger(), "角度差: %f", angle);
        //3.控制策略
        if(distance > 0.1)
        {
            if(fabs(angle) > 0.1)
            {
                msg.angular.z = fabs(angle);
            }
            else{
                msg.linear.x =k_ * distance;
            }
        }
        //4.限制线速度
        if(msg.linear.x > max_linear_speed_)
        {            msg.linear.x = max_linear_speed_;
        }
        publisher_->publish(msg);//发布消息

    }


};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);//初始化ROS2
    auto node = std::make_shared<TurtleContolNode>("turtle_control_node");//创建节点对象
    rclcpp::spin(node);//让节点开始工作
    rclcpp::shutdown();//关闭ROS2
    return 0;
}