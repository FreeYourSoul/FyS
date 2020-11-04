{stdenv, callPackage, cmake, flatbuffers, zeromq, cppzmq, catch2, fmt, spdlog, nlohmann_json, fil}:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    name = "fys-inventory-service-${version}";

    cmakeFlags = "
    -DBUILD_SERVICE_WORLD=OFF
    -DBUILD_SERVICE_QUEST=OFF
    -DBUILD_SERVICE_ARENA=OFF
    -DBUILD_SERVICE_INVENTORY=OFF
    -DBUILD_DISP_CHAT=OFF
    -DBUILD_DISP_GATEWAY=OFF
    -DBUILD_DISPATCHER=ON
    ";

    buildInputs = [ cmake zeromq cppzmq flatbuffers spdlog catch2 fmt nlohmann_json fil ];

}