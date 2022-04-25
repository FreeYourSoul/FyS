let pkgs = import <nixpkgs> { };
in with pkgs; rec {

  stdenv = pkgs.overrideCC pkgs.stdenv pkgs.gcc10;

  # External Dependencies (personal)
  fil = (callPackage (builtins.fetchurl
    "https://raw.githubusercontent.com/FreeYourSoul/FiL/9a38ffe32a7cd709efe56bf7f05255259acb95a0/fil-recipee.nix") {
      rev = "60af488da2b23962b0c085bc0c6a8444745c7471";
    });

  fseam = (callPackage (builtins.fetchurl
    "https://raw.githubusercontent.com/FreeYourSoul/FSeam/9f0ca84beab1b4c6152238820029450d589f319d/fseam-recipee.nix") {
      rev = "9f0ca84beab1b4c6152238820029450d589f319d";
    });

  # External Dependencies (not default nixpkg)
  chaiscript = (callPackage ./misc/nix/dependency/chaiscript.nix) { };
  tmxlite = (callPackage ./misc/nix/dependency/tmxlite.nix) { };
  cppzmq = (callPackage ./misc/nix/dependency/cppzmq.nix) { };
  sol3 = (callPackage ./misc/nix/dependency/sol3.nix) { };

  # FyS Dependencies
  dispatcher = (callPackage ./misc/nix/recipes/dispatcher.nix) {
    inherit stdenv nlohmann_json fil;
  };
  world_service = (callPackage ./misc/nix/recipes/world_service.nix) {
    inherit stdenv fil nlohmann_json tmxlite fseam sol3 cppzmq;
  };
  arena_service = (callPackage ./misc/nix/recipes/arena_service.nix) {
    inherit stdenv fil nlohmann_json fseam chaiscript cppzmq;
  };
}
