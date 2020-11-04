let
  pkgs = import <nixpkgs> {};
  
in with pkgs; rec {

  # External Depedencies

  fil = (import (builtins.fetchurl "https://raw.githubusercontent.com/FreeYourSoul/FiL/master/fil-recipee.nix") 
    { inherit stdenv cmake rocksdb catch2 fmt; }
  );

  # FyS Depedencies

  dispatcher = import ./recipes/dispatcher.nix {
    inherit stdenv callPackage cmake
    nlohmann_json flatbuffers zeromq cppzmq catch2 fmt spdlog fil; 
  }; 
}