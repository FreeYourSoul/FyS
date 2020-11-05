let
  pkgs = import <nixpkgs> {};
  
in with pkgs; rec {

  # External Depedencies

  fil = (import (builtins.fetchurl "https://raw.githubusercontent.com/FreeYourSoul/FiL/9a38ffe32a7cd709efe56bf7f05255259acb95a0/fil-recipee.nix") 
    { inherit stdenv cmake rocksdb catch2 fmt; rev="9a38ffe32a7cd709efe56bf7f05255259acb95a0"; }
  );

  # FyS Depedencies

  dispatcher = import ./recipes/dispatcher.nix {
    inherit stdenv callPackage cmake flatcc
    nlohmann_json flatbuffers zeromq cppzmq catch2 fmt spdlog fil boost; 
  }; 
}