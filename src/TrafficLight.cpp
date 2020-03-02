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
    // This method adds an object to the queue and then notifys the client to unblock its thread.

    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mtx);

    // add object to the queue transfering its ownership
    _queue.push_back(std::move(msg));
    std::cout << " Message " << msg << " has been sent to the queue" << std::endl;
    // notify client to unblock a thread
    _cnd.notify_one();
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

            // push each new TrafficLightPhase into _msgQ calling its send method in conjunction with move semantics
            // static_cast operator explicitly converts int value to enum type
            _msgQ.send(static_cast<TrafficLightPhase>(new_phase));

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