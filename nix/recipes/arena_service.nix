{ stdenv, callPackage, cmake, flatcc,
  boost, flatbuffers, zeromq, cppzmq, catch2, fmt, spdlog, nlohmann_json, fil, chaiscript,
  python37, python37Packages, fseam, zlib}:

stdenv.mkDerivation rec {
    inherit (callPackage ./common.nix { }) version src;

    pname = "arena_service";

    buildInputs = [ zeromq flatcc cppzmq flatbuffers spdlog fmt nlohmann_json boost
                    fil chaiscript fseam zlib ];

    nativeBuildInputs = [ cmake catch2 flatcc python37 python37Packages.ply ];

    cmakeFlags = [
      "-DBUILD_SERVICE_WORLD=OFF"
      "-DBUILD_SERVICE_QUEST=OFF"
      "-DBUILD_SERVICE_ARENA=ON"
      "-DBUILD_SERVICE_INVENTORY=OFF"

      "-DBUILD_DISP_CHAT=OFF"
      "-DBUILD_DISP_GATEWAY=OFF"
      "-DBUILD_DISPATCHER=OFF"

      "-DBUILD_CML=ON"
      "-DBUILD_SERVICE_CLI_TMX_MAP_CONVERTER=OFF"

      "-DBUILD_TESTING=ON"
    ];

    enableParallelBuilding = true;

    doCheck = true;

    checkPhase = ''
      ctest -VV
    '';

}