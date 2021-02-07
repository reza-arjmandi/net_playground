#pragma once

#include "net/Permissions.h"

template<class IOObjectType>
class Session : public std::enable_shared_from_this<Session<IOObjectType>> {

public:

    using RequestType = std::shared_ptr<Request>;
    using ResponseType = std::shared_ptr<Response>;
    using BufferType = std::shared_ptr<std::string>;

    Session(IOObjectType&& io_object, const Permissions& permissions) 
    : _io_object { std::forward<IOObjectType>(io_object) },
      _permissions { permissions } {
    }

    Permissions& get_permissions() {
        return _permissions;
    }

    template<class HandlerType>
    void async_read_request(RequestType request, HandlerType&& handler) {

        auto read_buffer = std::make_shared<std::string>();
        _read_buffers_map[request] = read_buffer;

        boost::asio::async_read_until(
            _io_object, 
            boost::asio::dynamic_buffer(*read_buffer), 
            '\n', 
            [this, read_buffer
            , request, _handler{ std::forward<HandlerType>(handler) } ]
            (const boost::system::error_code& e, std::size_t size) mutable {
                if(e) {
                    _read_buffers_map.erase(request);
                    _handler(false);
                    return;
                }
                std::vector<std::string> splited; 
                boost::split(splited, *read_buffer, boost::is_any_of(" ")); 
                _read_buffers_map.erase(request);
                if(splited.size() < 2) {
                    _handler(false);
                    return;
                }

                if(!_permissions.is_allowed(splited[0], splited[1])) {
                    _handler(false);
                    return;
                }

                if(splited.size() == 2) {
                    request->set_method(splited[0]); 
                    request->set_subject(splited[1]); 
                }
                else if(splited.size() == 3) {
                    request->set_method(splited[0]); 
                    request->set_subject(splited[1]); 
                    request->Data = splited[2];
                }
                _handler(true);
            });

    }

    void async_write_response(ResponseType response) {
        if(!_permissions.is_allowed(response->Subject)) {
            return;
        } 
        auto buffer_write = std::make_shared<std::string>(
            response->to_string());
        _write_buffers_map[response] = buffer_write;
        boost::asio::async_write(
            _io_object, 
            boost::asio::buffer(*buffer_write), 
            [self { this->shared_from_this() }, response]
            (const auto&, auto) mutable {
                self->_write_buffers_map.erase(response);
            });
    }
    
private:

    IOObjectType _io_object;
    Permissions _permissions;
    std::map<RequestType, BufferType> _read_buffers_map; 
    std::map<ResponseType, BufferType> _write_buffers_map; 

};