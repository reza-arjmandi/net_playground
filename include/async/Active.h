#pragma once

/*
Active is an execution context.
It has two objects:
1- A thread or thread pool
2- A work queue

In fact Active object runs the works which
was sended to it one by one if it has just
one thread, and it dispatches the works on
thread pool if it has a thread pool.
*/
class Active {

public:

    /*
    - active_at_first parameter: Determines that thread would be
        whether started at creation time or not, if it set to
        false thread doesn't start to run at creation time.
        The thread will be started when the run function is called.
    */
    Active(bool active_at_first = true, std::size_t num_threads = 1)
        : _active_at_first{ active_at_first }
        , _num_threads{ num_threads } {

        if (!_active_at_first) {
            _mq = std::make_unique<boost::asio::io_context>(_num_threads);
            return;
        }

        _thd_pool = std::make_unique<boost::asio::thread_pool>(num_threads);
    }

    ~Active() {
        if (_thd_pool) {
            _thd_pool->join();
        }
    }

    /*
    This function is used when Active object is constructed
    with false active_at_first argument.
    This function fires the thread to run.
    */
    void run() {
        if (_active_at_first) {
            return;
        }

        _thd_pool = std::make_unique<boost::asio::thread_pool>(_num_threads);
        boost::asio::post(*_thd_pool, [=]() {_mq->run(); });
    }

    /*
    This function is used to send work to work queue.
    */
    template<class MessageType>
    void send(MessageType&& m) {
        if (_active_at_first) {
            boost::asio::post(*_thd_pool, std::forward<MessageType>(m));
            return;
        }
        boost::asio::post(*_mq, std::forward<MessageType>(m));
    }

    boost::asio::any_io_executor get_executor() {
        if (_active_at_first) {
            return _thd_pool->get_executor();
        }
        return _mq->get_executor();
    }

private:

    bool _active_at_first{ true };
    std::size_t _num_threads{ 1 };
    std::unique_ptr<boost::asio::thread_pool> _thd_pool;
    std::unique_ptr<boost::asio::io_context> _mq;

};