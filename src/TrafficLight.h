#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;
enum TrafficLightPhase {red,green};

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::deque<TrafficLightPhase> _queue;
    std::condition_variable _cnd;
    std::mutex _mtx;
};

// Sub class TrafficLight inheriting from TrafficObject Class (Parent)
class TrafficLight : protected TrafficObject
{
public:
    // constructor / destructor
    TrafficLight();

    // typical behaviour methods
    void waitForGreen();

    void simulate();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _msgQ;
};

#endif