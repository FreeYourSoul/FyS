#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include <thread>
#include <zmq.hpp>
#include <iostream>

struct Args {
    int threadNumbers = 1;
    ushort bindingPort_fe = 4002;
    ushort bindingPort_be = 4001;
    std::string protocol = "tcp://";
};

int main (int argc, char *argv[])
{
    const Args args = [](int ac, char *av[]) -> Args {
        TCLAP::CmdLine cli("FyS::Dispatcher", ' ', "1.0");
        TCLAP::ValueArg<std::string> protocol("p", "protocol", "protocol to use (has to be prefixed with '://'", false, "tcp://", "string");
        TCLAP::ValueArg<ushort> changePortFe("f", "frontend", "Listening Port FrontEnd", false, 4002, "integer");
        TCLAP::ValueArg<ushort> changePortBe("b", "backend", "Listening Port BackEnd", false, 4001, "integer");
        TCLAP::ValueArg<int> threadNb("t", "thread", "Number of thread managed by zmq context", false, 1, "integer");

        cli.add(protocol);
        cli.add(changePortBe);
        cli.add(changePortFe);
        cli.add(threadNb);
        cli.parse(ac, av);
        return { threadNb.getValue(), changePortFe.getValue(), changePortBe.getValue(), protocol.getValue() };
    } (argc, argv);

    zmq::context_t context(args.threadNumbers);
    zmq::socket_t frontend (context, zmq::socket_type::xsub);
    zmq::socket_t backend (context, zmq::socket_type::xpub);
    zmq::socket_t listener (context, zmq::socket_type::pair);

    frontend.bind(args.protocol + "*:" + std::to_string(args.bindingPort_fe));
    backend.bind(args.protocol + "*:" + std::to_string(args.bindingPort_be));
    listener.bind("inproc://in");

    //  Start the proxy
    std::cout << "Dispatcher Proxy has been started with " << std::to_string(args.threadNumbers) << "thread(s) using protocol " << args.protocol
        << "\n\n--------\nSUBSCRIBER socket Front end with binding port " << std::to_string(args.bindingPort_fe)
        << "\nPUBLISHER socket Back end with binding port " << std::to_string(args.bindingPort_be)
        << "\n";

    std::thread t([&listener](){
        zmq::pollitem_t items [] = {
                { listener, 0, ZMQ_POLLIN, 0 }
        };
        while (true) {
            zmq::message_t message;
            zmq::poll(&items [0], 1, -1);
            if (items [0].revents & ZMQ_POLLIN) {
                listener.recv(&message);
                std::cout << message.data() << "\n";
            }
        }
    });

    zmq::proxy(frontend, backend, listener);
    t.join();
    return 0;
}
