#include <chrono>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include "my_robot_interfaces/srv/compute_rectangle_area.hpp"

using namespace std::chrono_literals;
using ComputeRectangleArea = my_robot_interfaces::srv::ComputeRectangleArea;

class ComputeRectangleAreaClient : public rclcpp::Node {
public:
  ComputeRectangleAreaClient() : Node("compute_rectangle_area_client") {
    client_ = this->create_client<ComputeRectangleArea>(
        "compute_rectangle_area");
    timer_ = this->create_wall_timer(
        500ms, std::bind(&ComputeRectangleAreaClient::send_request, this));
  }

private:
  void send_request() {
    if (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "节点已关闭，退出");
        return;
      }
      RCLCPP_WARN(this->get_logger(), "服务未就绪，正在等待...");
      return;
    }

    auto request = std::make_shared<ComputeRectangleArea::Request>();
    request->length = 3.5;
    request->width = 2.0;

    auto result_future = client_->async_send_request(request,
      std::bind(&ComputeRectangleAreaClient::handle_response,
          this, std::placeholders::_1));

    timer_->cancel();
  }

  void handle_response(
      rclcpp::Client<ComputeRectangleArea>::SharedFuture future) {
    auto response = future.get();
    RCLCPP_INFO(this->get_logger(),
      "请求结果: area = %.2f",
      response->area);
  }
  
private:
  rclcpp::Client<ComputeRectangleArea>::SharedPtr client_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ComputeRectangleAreaClient>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
