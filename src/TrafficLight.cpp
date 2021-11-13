#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait()
    // to wait for and receive new messages and pull them from the queue using move semantics.
    // The received object should then be returned by the receive function.
    std::unique_lock<std::mutex> u_lck(mutex_);
    cond_var_.wait(u_lck, [this](){return !queue_.empty();});
    T msg = std::move(queue_.front());
    queue_.pop_front();
    queue_.clear();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex>
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(mutex_);
    queue_.emplace_back(msg);
    data_ready_ = true;
    cond_var_.notify_one();
}

/* Implementation of class "TrafficLight" */

/*TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}*/

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        auto msg = message_queue_.receive();
        if(msg == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    std::lock_guard<std::mutex> lck(_mutex);
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    // launch cycleThroughPhase in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));

}

constexpr std::chrono::milliseconds task_sleep_ms{1U};

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles
    // and toggles the current phase of the traffic light between red and green and sends an update method
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds.
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    using namespace std::chrono;
    time_point<system_clock> start{system_clock::now()};
    auto cycle_gen = [](){return seconds(rand() % 6 + 4);};
    seconds cycle = cycle_gen();

    for(;;)
    {
        time_point<system_clock> now{system_clock::now()};

        // Check if cycle has elapsed
        if((now - start) >= cycle)
        {
            // Toggle current phase
            switch(_currentPhase)
            {
                case TrafficLightPhase::red:
                    { // Limit scope of the lockguard
                        std::lock_guard<std::mutex> lck(_mutex);
                        _currentPhase = TrafficLightPhase::green;
                    }
                    message_queue_.send(TrafficLightPhase::green);
                    break;
                case TrafficLightPhase::green:
                    { // Limit scope of the lockguard
                        std::lock_guard<std::mutex> lck(_mutex);
                        _currentPhase = TrafficLightPhase::red;
                    }
                    message_queue_.send(TrafficLightPhase::red);
                    break;
            }

            cycle = cycle_gen();
            start = system_clock::now();
        }
        // Thread should sleep
        std::this_thread::sleep_for(task_sleep_ms);
    }
}
