{stdenv, callPackage, cmake, zmqpp, catch2, spdlog}:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    pname = "fys-world-service";

    buildInputs = [ cmake zmq ];

}