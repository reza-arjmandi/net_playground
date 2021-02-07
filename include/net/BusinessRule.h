#pragma once

#include "net/Session.h"
#include "net/Request.h"
#include "net/Session.h"
#include "net/AsyncPacketStream.h"
#include "net/Pipeline.h"

#include "async/Stage.h"
#include "async/Active.h"

class BusinessRule {

public:
    
    using SessionType = std::shared_ptr<Session<boost::asio::ip::tcp::socket>>;
    using RequestType = std::shared_ptr<Request>;
    using ResponseType = std::shared_ptr<Response>;
    using PipelineType = std::shared_ptr<Pipeline>;

    void operator()(SessionType session) {
        auto pipeline = std::make_shared<Pipeline>(session);
        _session_pipeline_map[session] = pipeline;
        auto req = std::make_shared<Request>();
        _session_req_map[session] = req;
        read_request(req, session);
    }   


private:

    void read_request(RequestType req, SessionType session) {
        session->async_read_request(req, 
            [this, req, session]
            (bool is_success) {
                if(!is_success) {
                    read_request(req, session);
                    return;
                }

                if(!(req->Method == Methods::POST || req->Method == Methods::DELETE2)) {
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "invalid method for business rule");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }

                if(req->Data == "DECORATE\n" && req->Method == Methods::POST) {
                    _session_pipeline_map[session]->enable_decorate();
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "DECORATE stage is created");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }

                if(req->Data == "DECORATE\n" && req->Method == Methods::DELETE2) {
                    _session_pipeline_map[session]->disable_decorate();
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "DECORATE stage is deleted");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }

                if(req->Data == "ENCRYPT\n" && req->Method == Methods::POST) {
                    _session_pipeline_map[session]->enable_encrypt();
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "ENCRYPT stage is created");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }

                if(req->Data == "ENCRYPT\n" && req->Method == Methods::DELETE2) {
                    _session_pipeline_map[session]->disable_encrypt();
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "ENCRYPT stage is deleted");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }


                if(req->Data == "COMPRESS\n" && req->Method == Methods::POST) {
                    _session_pipeline_map[session]->enable_compress();
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "COMPRESS stage is created");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }

                if(req->Data == "COMPRESS\n" && req->Method == Methods::DELETE2) {
                    _session_pipeline_map[session]->disable_compress();
                    auto response = std::make_shared<Response>(
                        "BUSINESS_RULE", "COMPRESS stage is deleted");
                    session->async_write_response(response);
                    read_request(req, session);
                    return;
                }

            });
    }

    std::map<SessionType, RequestType> _session_req_map;
    std::map<SessionType, PipelineType> _session_pipeline_map;



};