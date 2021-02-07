#include "pch.h"

#include "async/Stage.h"
#include "async/Active.h"

#include "net/SessionFactory.h"
#include "net/TCPServer.h"
#include "net/SocketExecutorProvider.h"
#include "net/AsyncAuthentication.h"
#include "net/BusinessRule.h"

int main(int argc, char** argv)
{
    Stage business_rule([run_business_rules{ BusinessRule() }]
    (auto session) mutable {
        run_business_rules(session);
    });

    Stage authenticator([&, auth{ AsyncAuthentication() }]
    (auto session) mutable {
        auth(session, [&, session]
        (bool is_authenticed) mutable {
                if (!is_authenticed) {
                    return;
                }
                business_rule.process(session);
            });
    });

    Stage session_factory([&, session_creator{ SessionFactory() }]
    (auto&& io_object) mutable {
        auto session = session_creator(
            std::forward<decltype(io_object)>(io_object));
        authenticator.process(std::move(session));
    });

    SocketExecutorProvider socket_executor_provider;
    TCPServer tcp_serving(
        [&]() {socket_executor_provider.request_new_executor(); });

    Stage tcp_server([&](auto&& executor) mutable {
        auto socket = tcp_serving(std::forward<decltype(executor)>(executor));
        session_factory.process(std::move(socket));
        });

    socket_executor_provider.set_tcp_server_stage(&tcp_server);
    socket_executor_provider.request_new_executor();

    return 0;
}