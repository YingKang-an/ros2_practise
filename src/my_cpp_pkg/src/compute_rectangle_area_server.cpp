#include <memory>
#include <rclcpp/rclcpp.hpp>
#include "my_robot_interfaces/srv/compute_rectangle_area.hpp"

using ComputeRectangleArea = my_robot_interfaces::srv::ComputeRectangleArea;

class ComputeRectangleAreaService : public rclcpp::Node {
public:
  ComputeRectangleAreaService() : Node("compute_rectangle_area_service") {
    service_ = this->create_service<ComputeRectangleArea>(
        "compute_rectangle_area",
         std::bind(&ComputeRectangleAreaService::handle_compute_area,
            this, std::placeholders::_1, std::placeholders::_2));
    RCLCPP_INFO(this->get_logger(), "ComputeRectangleArea 服务已启动");
  }

private:
  void handle_compute_area(
        const std::shared_ptr<ComputeRectangleArea::Request> request,
        std::shared_ptr<ComputeRectangleArea::Response> response) {
    response->area = request->length * request->width;
    RCLCPP_INFO(this->get_logger(),
        "接收到请求: length=%.2f, width=%.2f, area=%.2f",
        request->length, request->width, response->area);
  }

private:
  rclcpp::Service<ComputeRectangleArea>::SharedPtr service_;
};

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ComputeRectangleAreaService>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
