{ stdenv, callPackage, cmake, flatcc,
  boost, flatbuffers, zeromq, cppzmq, catch2, fmt, spdlog, nlohmann_json, fil, lua, tmxlite, sol3,
  python37, python37Packages, fseam }:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix {
        description = "Server service : Handling player/characters movement in the section of the world map";
    }) version src meta;

    pname = "world_service";

    buildInputs = [ zeromq flatcc cppzmq flatbuffers spdlog fmt nlohmann_json boost
                    fil lua tmxlite fseam sol3 ];

    nativeBuildInputs = [ cmake flatcc python37 python37Packages.ply ];

    cmakeFlags = [
      "-DBUILD_SERVICE_WORLD=ON"
      "-DBUILD_SERVICE_QUEST=OFF"
      "-DBUILD_SERVICE_ARENA=OFF"
      "-DBUILD_SERVICE_INVENTORY=OFF"

      "-DBUILD_DISP_CHAT=OFF"
      "-DBUILD_DISP_GATEWAY=OFF"
      "-DBUILD_DISPATCHER=OFF"

      "-DBUILD_CML=ON"
      "-DBUILD_SERVICE_CLI_TMX_MAP_CONVERTER=ON"

      "-DBUILD_TESTING=ON"
    ];

    enableParallelBuilding = true;

    checkInputs = [ catch2 ];
    doCheck = true;

    checkPhase = ''
      ctest -VV
    '';

}