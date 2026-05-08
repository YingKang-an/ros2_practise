#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;

static constexpr auto TimerPeriod = 1000ms;
static constexpr auto CallbackDelay = 2000ms;

static std::string get_current_thread_id() {
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  return ss.str();
}

class Node1 : public rclcpp::Node {
public:
  explicit Node1() : Node("node1") {
    timer1_ = create_wall_timer(TimerPeriod, std::bind(&Node1::callbackTimer1, this));
    timer2_ = create_wall_timer(TimerPeriod, std::bind(&Node1::callbackTimer2, this));
    timer3_ = create_wall_timer(TimerPeriod, std::bind(&Node1::callbackTimer3, this));
  }

private:
  void callbackTimer1() {
    std::this_thread::sleep_for(CallbackDelay);
    counter_++;
    RCLCPP_INFO(get_logger(), "callback_1 count=%zu thread_id=%s", counter_,
        get_current_thread_id().c_str());
  }

  void callbackTimer2() {
    std::this_thread::sleep_for(CallbackDelay);
    counter_++;
    RCLCPP_INFO(get_logger(), "callback_2 count=%zu thread_id=%s", counter_,
        get_current_thread_id().c_str());
  }

  void callbackTimer3() {
    std::this_thread::sleep_for(CallbackDelay);
    counter_++;
    RCLCPP_INFO(get_logger(), "callback_3 count=%zu thread_id=%s",
        counter_, get_current_thread_id().c_str());
  }

  rclcpp::TimerBase::SharedPtr timer1_;
  rclcpp::TimerBase::SharedPtr timer2_;
  rclcpp::TimerBase::SharedPtr timer3_;
  size_t counter_ = 0;
};

class Node2 : public rclcpp::Node {
public:
  explicit Node2() : Node("node2") {
    timer4_ = create_wall_timer(TimerPeriod, std::bind(&Node2::callbackTimer4, this));
    timer5_ = create_wall_timer(TimerPeriod, std::bind(&Node2::callbackTimer5, this));
  }

private:
  void callbackTimer4() {
    std::this_thread::sleep_for(CallbackDelay);
    counter_++;
    RCLCPP_INFO(get_logger(), "callback_4 count=%zu thread_id=%s",
        counter_, get_current_thread_id().c_str());
  }

  void callbackTimer5() {
    std::this_thread::sleep_for(CallbackDelay);
    counter_++;
    RCLCPP_INFO(get_logger(), "callback_5 count=%zu thread_id=%s", 
        counter_, get_current_thread_id().c_str());
  }

  rclcpp::TimerBase::SharedPtr timer4_;
  rclcpp::TimerBase::SharedPtr timer5_;
  size_t counter_ = 0;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node1 = std::make_shared<Node1>();
  auto node2 = std::make_shared<Node2>();
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node1);
  executor.add_node(node2);
  executor.spin();
  rclcpp::shutdown();
  return 0;
}
