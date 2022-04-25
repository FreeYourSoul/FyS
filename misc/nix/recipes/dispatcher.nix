{ stdenv, callPackage, cmake, pkgconfig, flatcc, boost, flatbuffers, zeromq, cppzmq, catch2
, fmt, spdlog, nlohmann_json, fil }:

stdenv.mkDerivation rec {
  inherit (callPackage ./common.nix {
    description =
      "Service Dispatcher : proxy server to route messages from clients to connected service servers";
  })
    version src meta;

  pname = "fys-dispatcher";

  buildInputs = [
    zeromq
    flatcc
    cppzmq
    boost
    flatbuffers
    spdlog
    catch2
    fmt
    nlohmann_json
    fil
  ];
  nativeBuildInputs = [ cmake pkgconfig flatcc ];

  cmakeFlags = [
    "-DBUILD_SERVICE_WORLD=OFF"
    "-DBUILD_SERVICE_QUEST=OFF"
    "-DBUILD_SERVICE_ARENA=OFF"
    "-DBUILD_SERVICE_INVENTORY=OFF"

    "-DBUILD_DISP_CHAT=OFF"
    "-DBUILD_DISP_GATEWAY=OFF"
    "-DBUILD_DISPATCHER=ON"

    "-DBUILD_CML=OFF"
    "-DBUILD_SERVICE_CLI_TMX_MAP_CONVERTER=OFF"

    "-DBUILD_TESTING=OFF"
  ];

  enableParallelBuilding = true;
  doCheck = true;

  checkPhase = ''
    ctest -VV
  '';

}
