#include <iostream>
#include <random>
#include <chrono>  // to measure elapsed time
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // This method waits for new messages and then pulls and returns the associated object from the queue.

    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(_mtx);
    // wait to be notified and then awake the thread, pass unique lock to condition variable
    _cnd.wait(uLock, [this] { return !_queue.empty(); });

    // access the last element from queue and assign ownership of it to new variable 'msg'
    T msg = std::move(_queue.back());
    // remove the last element from queue
    _queue.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // This method adds an object to the queue and then notifys the client to unblock its thread.

    // get a lock to perform queue modification under the lock
    std::lock_guard<std::mutex> uLock(_mtx);

    // add object to the queue transfering its ownership
    _queue.push_back(std::move(msg));
    // TO-DO: protect cout with shared mutex
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
    // Run an infinite loop to repeatedly call the `receive` function on the message queue.
    // Once it receives `TrafficLightPhase::green`, the method returns.

    while(true){
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        TrafficLightPhase current_phase = _msgQ.receive();
        if(current_phase == TrafficLightPhase::green){
            return;
        }
    }
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
    std::uniform_int_distribution<int> distribution(4,6); // range 4 to 6 sec, includes endpoints

    // Record start time, initialize finish time
    auto start = std::chrono::high_resolution_clock::now();
    auto finish = std::chrono::high_resolution_clock::now();
    int cycle_duration = 5; // set first cycle to be 5 sec

    while(true){
        if (std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() > cycle_duration){

            // flip light: if red make it green, if green make it red
            int new_phase = abs(TrafficLight::getCurrentPhase() - 1);
            // update value of _currentPhase variable, static_cast explicitly converts int value to enum type
            _currentPhase = static_cast<TrafficLightPhase>(new_phase);
            // push each new TrafficLightPhase into _msgQ calling its send method in conjunction with move semantics
            _msgQ.send(std::move(_currentPhase));

            // generate next cycle duration (range was set 4 to 6 seconds)
            cycle_duration = distribution(generator);

            // update start time
            start = std::chrono::high_resolution_clock::now();
        }
       // sleep at every iteration to reduce CPU usage
       std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 100 milliseconds = 0.1 second
       // update finish time
       finish = std::chrono::high_resolution_clock::now();
    }
}