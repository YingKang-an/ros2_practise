#ifndef PLANNER_SAMPLE_1__VISIBILITY_CONTROL_H_
#define PLANNER_SAMPLE_1__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define PLANNER_SAMPLE_1_EXPORT __attribute__ ((dllexport))
    #define PLANNER_SAMPLE_1_IMPORT __attribute__ ((dllimport))
  #else
    #define PLANNER_SAMPLE_1_EXPORT __declspec(dllexport)
    #define PLANNER_SAMPLE_1_IMPORT __declspec(dllimport)
  #endif
  #ifdef PLANNER_SAMPLE_1_BUILDING_LIBRARY
    #define PLANNER_SAMPLE_1_PUBLIC PLANNER_SAMPLE_1_EXPORT
  #else
    #define PLANNER_SAMPLE_1_PUBLIC PLANNER_SAMPLE_1_IMPORT
  #endif
  #define PLANNER_SAMPLE_1_PUBLIC_TYPE PLANNER_SAMPLE_1_PUBLIC
  #define PLANNER_SAMPLE_1_LOCAL
#else
  #define PLANNER_SAMPLE_1_EXPORT __attribute__ ((visibility("default")))
  #define PLANNER_SAMPLE_1_IMPORT
  #if __GNUC__ >= 4
    #define PLANNER_SAMPLE_1_PUBLIC __attribute__ ((visibility("default")))
    #define PLANNER_SAMPLE_1_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define PLANNER_SAMPLE_1_PUBLIC
    #define PLANNER_SAMPLE_1_LOCAL
  #endif
  #define PLANNER_SAMPLE_1_PUBLIC_TYPE
#endif

#endif  // PLANNER_SAMPLE_1__VISIBILITY_CONTROL_H_
