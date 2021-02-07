#pragma once

#include "async/Active.h"

/*
Stage class is used to turn a synchronous work into an asynchronous work.
In fact Stage object enables a work to be run concurrently.

Every Stage object has two objects:
1- A work object
2- An active object

Work is a function object which has to get a buffer pointer as an input argument.
It means the signature of function object should be equivalent to the following:
template<class BufferType>
void work(BufferType* buffer)

Active object is an execution context object.
Execution context is a place where a work object will be run.
It can be a thread, thread pool or io_context (kind of queue).
*/
template<
    class WorkType,
    class ActiveType = Active>
    class Stage {

    public:

        /*
        This constructor creates an active object inside the Stage object.
        - num_threads parameter : Determine the number of threads inside the active object.
        */
        Stage(WorkType work, std::size_t num_threads = 1)
            : _work{ work } {
            _active = std::make_unique<ActiveType>(num_threads);
        }

        /*
        This constructor gets an shared active object from outside of the Stage object.
        */
        Stage(WorkType work, std::shared_ptr<ActiveType> shared_active)
            : _work{ work },
            _shared_active{ shared_active } {
        }

        /*
        Run the Stage object with the buffer object.
        In fact this function sends the work object to the active object.
        Buffer object will be passed to the work function object.
        */
        template<class BufferType>
        void process(BufferType&& buffer) {
            if (_shared_active) {
                _shared_active->send(
                    [this, buff{ std::forward<BufferType>(buffer) }]() mutable {
                    _work(std::forward<BufferType>(buff));
                });
                return;
            }
            _active->send([
                this, buff{ std::forward<BufferType>(buffer) }]() mutable {
                    _work(std::forward<BufferType>(buff));
                });
        }

    private:

        WorkType _work;

        std::unique_ptr<ActiveType> _active{ nullptr };
        std::shared_ptr<ActiveType> _shared_active{ nullptr };

};
