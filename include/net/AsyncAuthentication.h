#pragma once

#include "net/Request.h"
#include "net/Session.h"

class AsyncAuthentication {

public:

    using SessionType = std::shared_ptr<Session<boost::asio::ip::tcp::socket>>;
    using RequestType = std::shared_ptr<Request>;
    using ResponseType = std::shared_ptr<Response>;

    template<class HandlerType>
    void operator()(SessionType session, HandlerType&& handler) {
        
        auto req = std::make_shared<Request>();
        _session_req_map[session] = req;
        session->async_read_request(req, 
            [this, req, session, _handler { std::forward<HandlerType>(handler) }]
            (bool is_success) mutable {
                if(!is_success) {
                    _handler(false);
                    _session_req_map.erase(session);
                    return;
                }

                if(req->Method != Methods::POST) {
                    auto response = std::make_shared<Response>(
                        "AUTH_CHALLENGE", "invalid method for auth");
                    session->async_write_response(response);
                    _handler(false);
                    _session_req_map.erase(session);
                    return;
                }

                if(req->Subject != Subjects::AUTH_CHALLENGE) {
                    auto response = std::make_shared<Response>(
                        "AUTH_CHALLENGE", "invalid subject for auth");
                    session->async_write_response(response);
                    _handler(false);
                    _session_req_map.erase(session);
                    return;
                }

                if(req->Data != "HelloWorld\n") {
                    auto response = std::make_shared<Response>(
                        "AUTH_CHALLENGE", "password is wrong!");
                    session->async_write_response(response);
                    _handler(false);
                    _session_req_map.erase(session);
                    return;
                }

                auto response = std::make_shared<Response>(
                        "AUTH_CHALLENGE", "authentication is successful");
                session->get_permissions().give_permission(Subjects::BUSINESS_RULE);
                session->get_permissions().give_permission(Methods::DELETE2);
                session->async_write_response(response);
                _handler(true);
                _session_req_map.erase(session);
            });
    }

private:

    std::map<SessionType, RequestType> _session_req_map;

};