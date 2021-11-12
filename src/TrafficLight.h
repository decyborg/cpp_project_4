#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive.
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type.
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as private members.

template <class T>
class MessageQueue
{
public:

private:

};

// Enum class to represent the phases of the TrafficLight
enum class TrafficLightPhase : bool
{
  red,      //< The traffic light is in its red state
  green,    //< The traffic light is in its green state
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“.
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value.

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight() = default;
    ~TrafficLight() = default;

    // getters / setters

    // Gets the current phase of the traffic light
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods

    // Wait For Green
    void waitForGreen();

    // Simulate implementation
    void simulate() override;

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase{TrafficLightPhase::red};
};

#endif
