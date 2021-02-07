#pragma once

template<
int Port = 1234,
class IOContextType = boost::asio::io_context,
class EndpointType = boost::asio::ip::tcp::endpoint,
class AcceptorType = boost::asio::ip::tcp::acceptor>
class TCPServer {

public:

    TCPServer(std::function<void(void)> request_new_executor):
      _request_new_executor {request_new_executor} {
    }

    template<class ExecutorType>
    decltype(auto) operator()(ExecutorType&& executor) {
        _request_new_executor();
        return _acceptor.accept(std::forward<ExecutorType>(executor));
    }

private:

    IOContextType _io_context;
    EndpointType _endpoint { boost::asio::ip::tcp::v4(), Port};
    AcceptorType _acceptor {_io_context, _endpoint};
    std::function<void(void)> _request_new_executor;

};