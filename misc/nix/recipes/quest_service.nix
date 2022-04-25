{ stdenv, callPackage, cmake, pkgconfig, zmqpp, catch2, spdlog }:

stdenv.mkDerivation rec {
  inherit (callPackage ./common.nix { }) version src;

  pname = "fys-quest-service";

  buildInputs = [ zmq ];
  nativeBuildInputs = [ make pkgconfig ];

}
