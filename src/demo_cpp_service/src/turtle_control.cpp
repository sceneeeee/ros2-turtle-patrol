#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>
#include "turtlesim/msg/pose.hpp"
#include "chapt4_turtle_interfaces/srv/patrol.hpp"

using Patrol = chapt4_turtle_interfaces::srv::Patrol;
using namespace std::chrono_literals;

class TurtleControlNode: public rclcpp::Node
{
private:
    rclcpp::Service<Patrol>::SharedPtr patrol_service_;
    // rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber_;
    double target_x_{1.0};
    double target_y_{1.0};
    double k_{1.0}; // 比例系数
    double max_speed_{3.0};

public:
    explicit TurtleControlNode(const std::string& node_name):Node(node_name)
    {
        patrol_service_ = this->create_service<Patrol>("patrol",
            [&](const Patrol::Request::SharedPtr request,Patrol::Response::SharedPtr response) -> void {
                if ((request->target_x && request->target_x < 12.0f)&&
                    (request->target_y && request->target_y < 12.0f)
                    )
                {
                    this->target_x_ = request->target_x;
                    this->target_y_ = request->target_y;
                    response->result = Patrol::Response::SUCESS;
                }else{
                    response->result = Patrol::Response::FAIL;
                }
        });

        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10);
        subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(&TurtleControlNode::on_pose_received, this, std::placeholders::_1)
        );
        // timer_ = this->create_wall_timer(1000ms, std::bind(&TurtleControlNode::timer_callback, this));
    }
    
    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {
        // get the position
        auto current_x = pose->x;
        auto current_y = pose->y;
        RCLCPP_INFO(get_logger(),"the current x is %f, y is %f", current_x, current_y);
        
        // calculate the 距离差和角度茶
        auto distance = std::sqrt(
            (target_x_-current_x)*(target_x_-current_x)+
            (target_y_-current_y)*(target_y_-current_y)
        );
        auto angle = std::atan2((target_y_-current_y),(target_x_-current_x)) - pose->theta;
        
        // control strategy
        auto msg = geometry_msgs::msg::Twist();
        if(distance > 0.1){
            if(fabs(angle) > 0.2){
                msg.angular.z = fabs(angle);
            }else{
                msg.linear.x = k_*distance;
            }
        }

        // limit the max_speed
        if(msg.linear.x > max_speed_){
            msg.linear.x = max_speed_;
        }

        publisher_->publish(msg);
    }
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControlNode>("turtle_control");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}