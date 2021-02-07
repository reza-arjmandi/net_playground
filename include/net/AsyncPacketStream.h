#pragma once

class AsyncPacketStream {

public:

    AsyncPacketStream(std::function<void(std::string&&)> handler)\
        : _handler { handler } {
    }

    void start_stream() {
        if(_started) {
            return;
        }
        
        _started = true;
        ignite_timer();
    }

    void stop_stream() {
        if(!_started) {
            return;
        }
        _started = false;
    }

private:

    void ignite_timer() {
        if(!_started) {
            return;
        }

        _timer.expires_from_now(boost::posix_time::seconds(1));
        _timer.async_wait(
            [this]
            (const boost::system::error_code& error) mutable {
            if(error) {
                return;
            }

            std::string packet = 
                "PACKET " + std::to_string(_packet_number++) + " ";
            _handler(std::move(packet));
            ignite_timer();
        });
    }
    
    std::function<void(std::string&&)> _handler;
    int _packet_number { 0 };
    bool _started { false };
    Active _active;
    boost::asio::deadline_timer _timer {_active.get_executor()};

};