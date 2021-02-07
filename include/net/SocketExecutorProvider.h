#pragma once

class SocketExecutorProvider {

public:

    void request_new_executor() {
        provide_executor();
    }

    template<class StageType>
    void set_tcp_server_stage(StageType* stage) {
        _create_executor = [&]() mutable {
            if(!stage) {
                return;
            }

            stage->process(_active.get_executor());
        };
    }

    void provide_executor() {
        if(!_create_executor) {
            return;
        }
        _create_executor();
    }

private:

    std::function<void(void)> _create_executor;
    boost::asio::io_context _all_sockets_io_context;
    Active _active;

};