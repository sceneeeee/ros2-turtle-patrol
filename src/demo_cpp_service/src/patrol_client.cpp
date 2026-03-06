#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include "chapt4_turtle_interfaces/srv/patrol.hpp"
#include <ctime>

using Patrol = chapt4_turtle_interfaces::srv::Patrol;
using namespace std::chrono_literals;

class PatrolClient: public rclcpp::Node
{
private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<Patrol>::SharedPtr patrol_client_;

public:
    PatrolClient() : Node("turtle_patrol_client")
    {
        patrol_client_ = this->create_client<Patrol>("patrol");
        timer_ = this->create_wall_timer(10s, [&]()->void{
            // 检测服务是否上线
            while (!this->patrol_client_->wait_for_service(1s)) {
                if(!rclcpp::ok()){
                    RCLCPP_ERROR(this->get_logger(), "rclcpp was interrupted while waiting for the service. Exiting.");
                    return;
                }
                RCLCPP_INFO(this->get_logger(), "service not available, waiting again...");
            }
            // 创建请求
            auto request = std::make_shared<Patrol::Request>();
            srand((unsigned)time(NULL));
            request->target_x = static_cast<float>(rand() % 15);
            request->target_y = static_cast<float>(rand() % 15);
            RCLCPP_INFO(this->get_logger(), "sending patrol request: target_x=%f, target_y=%f", request->target_x, request->target_y);
            // 发送请求并等待响应
            this->patrol_client_->async_send_request(request,
                [&](rclcpp::Client<Patrol>::SharedFuture result_future)->void{
                    auto response = result_future.get();
                    if(response->result == Patrol::Response::SUCESS){
                        RCLCPP_INFO(this->get_logger(), "patrol request succeeded!");
                    }else{
                        RCLCPP_WARN(this->get_logger(), "patrol request failed!");
                    }
                });
        });
    }
    
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PatrolClient>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}