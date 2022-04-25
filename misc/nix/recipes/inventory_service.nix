{ stdenv, callPackage, lua, cmake, pkgconfig, zeromq, cppzmq, catch2, fmt, spdlog
, nlohmann_json }:

stdenv.mkDerivation rec {
  inherit (callPackage ./common.nix { }) version src;

  pname = "fys-inventory-service";

  buildInputs = [ zeromq lua cppzmq spdlog catch2 fmt nlohmann_json ];

  nativeBuildInputs = [ cmake pkgconfig ];

}
