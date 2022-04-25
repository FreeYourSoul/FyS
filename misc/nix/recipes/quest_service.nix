{ stdenv, callPackage, cmake, zmqpp, catch2, spdlog, python3 }:

stdenv.mkDerivation rec {
  inherit (callPackage ./common.nix { }) version src;

  pname = "fys-quest-service";

  buildInputs = [ zmq ];
  nativeBuildInputs = [ make python3 ];

}
