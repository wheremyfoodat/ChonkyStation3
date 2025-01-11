#pragma once

#include <common.hpp>
#include <queue>
#include <set>
#include <functional>


// Scheduler class ported from ChonkyStation (rewrite)

static constexpr auto schedulerMaxEntries = 64;

// https://stackoverflow.com/questions/19467485/how-to-remove-element-not-at-top-from-priority-queue
template<typename T>
class pqueue : public std::priority_queue<T, std::vector<T>, std::greater<T>> {
public:
    std::vector<T>& get_container() {
        return this->c;
    }

    bool remove(const T& value) {
        auto it = std::find(this->c.begin(), this->c.end(), value);

        if (it == this->c.end()) {
            return false;
        }

        if (it == this->c.begin()) {
            // Deque the top element
            this->pop();
        }
        else {
            // Remove element and re-heap
            this->c.erase(it);
            std::make_heap(this->c.begin(), this->c.end(), this->comp);
        }

        return true;
    }
};

static u64 next_event_id = 1;

class Scheduler {
public:
    u64 time = 0;
    void tick(u64 cycles);
    u64 tickToNextEvent();

    struct Event {
        std::function<void(void)> func;
        u64 time = 0;
        std::string name = "Default";
        u64 id;

        bool operator>(const Event& other) const {
            return time > other.time;
        }

        bool operator==(const Event& other) const {
            return id == other.id;
        }
    };

    pqueue<Event> events;
    void push(std::function<void(void)> const& func, u64 time, std::string name = "Unnamed event");
    void deleteAllEventsOfName(std::string name);

    static u64 uSecondsToCycles(double us);
};