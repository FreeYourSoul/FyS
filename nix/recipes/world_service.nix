{ stdenv, callPackage, cmake, flatcc,
  boost, flatbuffers, zeromq, cppzmq, catch2, fmt, spdlog, nlohmann_json, fil, lua, tmxlite, sol3,
  python37, python37Packages, fseam }:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    pname = "world_service";

    buildInputs = [ zeromq flatcc cppzmq flatbuffers spdlog fmt nlohmann_json boost
                    fil lua tmxlite fseam sol3 ];

    nativeBuildInputs = [ cmake flatcc python37 python37Packages.ply ];

    cmakeFlags = [
      "-DBUILD_CML=ON"
      "-DBUILD_SERVICE_WORLD=ON"
      "-DBUILD_SERVICE_QUEST=OFF"
      "-DBUILD_SERVICE_ARENA=OFF"
      "-DBUILD_SERVICE_INVENTORY=OFF"
      "-DBUILD_DISP_CHAT=OFF"
      "-DBUILD_DISP_GATEWAY=OFF"
      "-DBUILD_TESTING=ON"
      "-DBUILD_DISPATCHER=OFF"
    ];

    enableParallelBuilding = true;

    checkInputs = [ catch2 ];
    doCheck = true;

    checkPhase = ''
      ctest -VV
    '';
}