#include <iostream>
#include <random>
#include <chrono>  // to measure elapsed timecd build
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight(){
    _currentPhase = TrafficLightPhase::red;
}


void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases(){
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(4,6); // set range 4 to 6 seconds

    // Record start time, initialize finish time
    auto start = std::chrono::high_resolution_clock::now();
    auto finish = std::chrono::high_resolution_clock::now();
    int cycle_duration = 5; // set first cycle to be 5 sec

    while(true){
        if (std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() > cycle_duration){

            // flip light: if red make it green, if green make it red
            int new_phase = abs(TrafficLight::getCurrentPhase() - 1);
            // static_cast operator explicitly converts an int value to an enum type
            _currentPhase = static_cast<TrafficLightPhase>(new_phase);

            // TO-DO: Send update message to the class `MessageQueue`
            // to be implemented as part of assignment FP.3 and FP.4b

            // generate next cycle duration (range was set 4 to 6 seconds)
            int cycle_duration = distribution(generator)*1000;

            // update start time
            start = std::chrono::high_resolution_clock::now();
        }

       std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 100 milliseconds = 0.1 second
       // update finish time
       finish = std::chrono::high_resolution_clock::now();
    }
}