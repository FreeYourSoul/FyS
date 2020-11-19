{ stdenv, callPackage, cmake, flatcc, 
  boost, flatbuffers, zeromq, cppzmq, catch2, fmt, spdlog, nlohmann_json, fil }:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    pname = "fys-dispatcher";

    buildInputs = [ zeromq flatcc cppzmq boost flatbuffers spdlog catch2 fmt nlohmann_json fil ];
    nativeBuildInputs = [ cmake flatcc ];

    cmakeFlags = [
      "-DBUILD_CML=OFF"
      "-DBUILD_SERVICE_WORLD=OFF"
      "-DBUILD_SERVICE_QUEST=OFF"
      "-DBUILD_SERVICE_ARENA=OFF"
      "-DBUILD_SERVICE_INVENTORY=OFF"
      "-DBUILD_DISP_CHAT=OFF"
      "-DBUILD_DISP_GATEWAY=OFF"
      "-DBUILD_TESTING=OFF"
      "-DBUILD_DISPATCHER=ON"
      "-DBUILD_SERVICE_CLI_TMX_MAP_CONVERTER=OFF"
    ];

    enableParallelBuilding = true;
    doCheck = true;

    checkPhase = ''
      ctest -VV
    '';
    
}