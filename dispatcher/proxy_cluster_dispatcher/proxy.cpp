#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include <zmq.hpp>
#include <iostream>

struct Args {
    int threadNumbers = 1;
    ushort bindingPort_fe = 5559;
    ushort bindingPort_be = 5560;
    std::string protocol = "tcp://";
};

int main (int argc, char *argv[])
{
    const Args args = [](int ac, char *av[]) -> Args {
        TCLAP::CmdLine cli("FyS::Dispatcher", ' ', "1.0");
        TCLAP::ValueArg<std::string> protocol("p", "protocol", "protocol to use (has to be prefixed with '://'", false, "tcp://", "string");
        TCLAP::ValueArg<ushort> changePortFe("f", "frontend", "Listening Port FrontEnd", false, 5559, "integer");
        TCLAP::ValueArg<ushort> changePortBe("b", "backend", "Listening Port BackEnd", false, 5560, "integer");
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

    frontend.bind(args.protocol + "*:" + std::to_string(args.bindingPort_fe));
    backend.bind(args.protocol + "*:" + std::to_string(args.bindingPort_be));

    //  Start the proxy
    std::cout << "Dispatcher Proxy has been started with " << std::to_string(args.threadNumbers) << "thread(s) using protocol " << args.protocol
        << "\n\n--------\nSUBSCRIBER socket Front end with binding port " << std::to_string(args.bindingPort_fe)
        << "\nPUBLISHER socket Back end with binding port " << std::to_string(args.bindingPort_be)
        << "\n";

    zmq::proxy(frontend, backend, nullptr);

    return 0;
}
