let pkgs = import <nixpkgs> { };
in with pkgs; rec {

  stdenv = pkgs.overrideCC pkgs.stdenv pkgs.gcc10;

  # External Dependencies (personal)
  fil = (callPackage (builtins.fetchurl
    "https://raw.githubusercontent.com/FreeYourSoul/FiL/9a38ffe32a7cd709efe56bf7f05255259acb95a0/fil-recipee.nix") {
      rev = "903f138ab473c6249b58e1571191c71b61e912ab";
    });

  fseam = (callPackage (builtins.fetchurl
    "https://raw.githubusercontent.com/FreeYourSoul/FSeam/9f0ca84beab1b4c6152238820029450d589f319d/fseam-recipee.nix") {
      rev = "9f0ca84beab1b4c6152238820029450d589f319d";
    });

  # External Dependencies (not default nixpkg)
  nlohmann_json = (callPackage ./nix/dependency/nlohmann_json.nix) { };
  chaiscript = (callPackage ./nix/dependency/chaiscript.nix) { };
  tmxlite = (callPackage ./nix/dependency/tmxlite.nix) { };
  cppzmq = (callPackage ./nix/dependency/cppzmq.nix) { };
  sol3 = (callPackage ./nix/dependency/sol3.nix) { };
  spdlog = (callPackage ./nix/dependency/spdlog.nix) { };

  # FyS Dependencies
  dispatcher = (callPackage ./nix/recipes/dispatcher.nix) {
    inherit stdenv nlohmann_json fil spdlog;
  };
  world_service = (callPackage ./nix/recipes/world_service.nix) {
    inherit stdenv fil nlohmann_json tmxlite fseam sol3 cppzmq spdlog;
  };
  arena_service = (callPackage ./nix/recipes/arena_service.nix) {
    inherit stdenv fil nlohmann_json fseam chaiscript cppzmq spdlog;
  };
}
