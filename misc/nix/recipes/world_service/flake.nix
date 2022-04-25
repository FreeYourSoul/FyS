{
  description =
    "Server service : Handling player/characters movement in the section of the world map";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";

    fil.url = "github:FreeYourSoul/FiL/62084104ab569f7663ea6b71c6e799e69dea4a1f";
    fseam.url = "github:FreeYourSoul/FSeam";

    tmxlite.url = "path:./../../dependency/tmxlite";
    chaiscript.url = "path:./../../dependency/chaiscript";
    sol3.url = "path:./../../dependency/sol3";
  };

  outputs = { self, nixpkgs, flake-utils, tmxlite, sol3, chaiscript, fil, fseam }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in rec {

        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "world_service";
          version = "1.0.0-DEV";
          src = pkgs.nix-gitignore.gitignoreSource [ ".git" ] ./../../../..;

          buildInputs = with pkgs; [
            zeromq
            flatcc
            cppzmq
            flatbuffers
            spdlog
            fmt
            nlohmann_json
            boost
            fil
            lua
            tmxlite
            fseam
            sol3
          ];

          nativeBuildInputs = with pkgs; [ cmake flatcc python37 python37Packages.ply pkgconfig ];

  shellHooks  = ''
    export CMAKE_PREFIX_PATH="share/cmake/"
  '';


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

          checkInputs = [ pkgs.catch2 ];
          doCheck = true;

          checkPhase = ''
            ctest -VV
          '';
        };

        world_service = defaultPackage;
	shell = defaultPackage;

      });
}
