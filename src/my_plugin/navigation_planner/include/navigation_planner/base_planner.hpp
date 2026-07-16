/****
 * 路径规划器插件的共同基类,在该类中写插件所需实现的函数
 * eg:
 *    1.初始化参数
 *    2.生成规划路径
 */

#ifndef NAV_PLANNER_BASE_PLANNER_HPP
#define NAV_PLANNER_BASE_PLANNER_HPP

namespace nav_planner{

class BasePlanner{
protected:
  // 插件基类必须提供无参构造
  BasePlanner() = default;
public:
  // 初始化函数
  virtual void init(double rate) = 0;
  // 路进规划函数
  virtual void path() = 0;
};

} // namespace nav_planner

#endif