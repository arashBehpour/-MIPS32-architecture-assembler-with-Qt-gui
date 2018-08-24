#ifndef THREAD_SAFE_MESSAGE_QUEUE
#define THREAD_SAFE_MESSAGE_QUEUE

#include <iostream>
#include <string>
#include <queue>
#include <condition_variable>
#include <thread>
#include <mutex>


template<typename T>
class ThreadSafeMessageQueue {
private:
	std::queue<T> the_queue;
	mutable std::mutex the_mutex;
	/* The condition_variable class is a synchronization primitive that can be used to block a thread, or multiple threads at the same time, until
	   another thread both modifies a shared variable (the condition), and notifies the condition_variable. */
	std::condition_variable the_condition_variable; 

public:

	bool empty() const {
		std::lock_guard<std::mutex> lock(the_mutex); // This locks the thread until the end of scope of the function call
		return the_queue.empty();
	};

	void push(const T& value) {
		std::unique_lock<std::mutex> lock(the_mutex); // OR I CAN DO A std::lock_guard<std::mutex> lock(the_mutex) AND NOT BOTHER WITH UNLOCKING
		the_queue.push(value);
		lock.unlock();
		the_condition_variable.notify_one(); // notifies or allows only one thread to push into the queue
	};

	bool try_pop(T& poppedValue) { // For this project I will be using the wait_pop() function instead because when I need to pop something it needs to be done
		std::lock_guard<std::mutex> lock(the_mutex);
		if (the_queue.empty()) {
			return false;
		}

		poppedValue = the_queue.front(); // I want to know what value I popped so I send in a default variable into this function
		the_queue.pop();
		return true;
	};

	void wait_and_pop(T& poppedValue) {
		std::unique_lock<std::mutex> lock(the_mutex);
		while (the_queue.empty()) {
			/* wait causes the current thread to block until the condition variable is notified or 
			   a spurious wakeup occurs, optionally looping until some predicate is satisfied. */
			the_condition_variable.wait(lock); // The thread will be locked until the_queue is not empty, then once it exists the while loop will lock.unlock() b/c of the wait() function 
		}

		poppedValue = the_queue.front();
		the_queue.pop();
	};
};

#endif // !THREAD_SAFE_MESSAGE_QUEUE
