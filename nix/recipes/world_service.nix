{stdenv, callPackage, cmake, zmqpp, catch2, spdlog}:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    name = "fys-world-service-${version}";

    buildInputs = [ cmake zmq ];

}