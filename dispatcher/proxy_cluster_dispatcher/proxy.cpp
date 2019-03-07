#include <tclap/ValueArg.h>
#include <tclap/CmdLine.h>
#include "zhelpers.hpp"

struct Args {
    int threadNumbers = 1;
    ushort bindingPort_fe = 5559;
    ushort bindingPort_be = 5560;
    std::string protocol = "tcp://";
};

Args parseArguments(int argc, char *argv[]) {
    TCLAP::CmdLine cli("FyS::Dispatcher", ' ', VERSION_DISPATCHER);
    TCLAP::ValueArg<std::string> protocol("p", "protocol", "protocol to use (has to be prefixed with '://'", false, "tcp://", "string");
    TCLAP::ValueArg<ushort> changePortFe("f", "frontend", "Listening Port FrontEnd", false, 5559, "integer");
    TCLAP::ValueArg<ushort> changePortBe("b", "backend", "Listening Port BackEnd", false, 5560, "integer");
    TCLAP::ValueArg<int> threadNb("t", "thread", "Number of thread managed by zmq context", false, 1, "integer");

    cli.add(protocol);
    cli.add(changePortBe);
    cli.add(changePortFe);
    cli.add(threadNb);
    cli.parse(ac, av);
    return {threadNb.getValue(), };
}

int main (int argc, char *argv[])
{
    Args args = parseArguments(argv, argv);
    zmq::context_t context(args.threadNumbers);
    zmq::socket_t frontend (context, ZMQ_XSUB);
    zmq::socket_t backend (context, ZMQ_XPUB);

    frontend.bind(args.protocol + "*:" + args.bindingPort_fe);
    backend.bind(args.protocol + "*:" + args.bindingPort_be);

    //  Start the proxy
    zmq::proxy(frontend, backend, nullptr);

    return 0;
}
