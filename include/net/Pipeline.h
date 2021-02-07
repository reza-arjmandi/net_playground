#pragma once


#include "net/Session.h"
#include "net/Request.h"
#include "net/Session.h"
#include "net/AsyncPacketStream.h"

#include "async/Stage.h"
#include "async/Active.h"

std::string decorate(std::string&& buffer) {
    return buffer + "-->decorated";
}

std::string encrypt(std::string&& buffer) {
    return buffer + "-->encrypted";
}

std::string compress(std::string&& buffer) {
    return buffer + "-->compressed";
}

void stream(
    std::shared_ptr<Session<boost::asio::ip::tcp::socket>> session, 
    std::string&& buffer) {
    auto response = std::make_shared<Response>("BUSINESS_RULE", buffer);
    session->async_write_response(response);
}

class Pipeline {

public:

    using SessionType = std::shared_ptr<Session<boost::asio::ip::tcp::socket>>;
    using StageType = Stage<std::function<void(std::string&&)>>;
    
    Pipeline(SessionType session)
    : _session {session} {
        packet_stream.start_stream();
    }

    void enable_decorate() {
        _enable_decorate = true;
    }

    void disable_decorate() {
        _enable_decorate = false;
    }

    void enable_encrypt() {
        _enable_encrypt = true;
    }

    void disable_encrypt() {
        _enable_encrypt = false;
    }

    void enable_compress() {
        _enable_compress = true;
    }

    void disable_compress() {
        _enable_compress = false;
    }

private:

    bool _enable_decorate {false};
    bool _enable_encrypt {false};
    bool _enable_compress {false};

    SessionType _session;

    StageType streamer { [&](auto&& buffer) mutable {
        stream(_session, std::forward<decltype(buffer)>(buffer));
    } };

    StageType compressor { [&](auto&& buffer) mutable {
        if(!_enable_compress) {
            streamer.process(std::forward<decltype(buffer)>(buffer));
            return;
        }

        auto result = compress(std::forward<decltype(buffer)>(buffer));
        streamer.process(std::move(result));
    } };

    StageType encryptor{ [&](auto&& buffer) mutable {
        if(!_enable_encrypt) {
            compressor.process(std::forward<decltype(buffer)>(buffer));
            return;
        }

        auto result = encrypt(std::forward<decltype(buffer)>(buffer));
        compressor.process(std::move(result));
    } };

    StageType decorator{ [&](auto&& buffer) mutable {
        if(!_enable_decorate) {
            encryptor.process(std::forward<decltype(buffer)>(buffer));
            return;
        }

        auto result = decorate(std::forward<decltype(buffer)>(buffer));
        encryptor.process(std::move(result));
    } };

    AsyncPacketStream packet_stream {[&](std::string&& buffer){
        decorator.process(std::move(buffer));
    }};

};