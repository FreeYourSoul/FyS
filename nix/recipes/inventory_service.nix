{stdenv, callPackage, lua, cmake, zeromq, cppzmq, catch2, fmt, spdlog, nlohmann_json}:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    name = "fys-inventory-service-${version}";

    buildInputs = [ cmake zeromq lua cppzmq spdlog catch2 fmt nlohmann_json];

}