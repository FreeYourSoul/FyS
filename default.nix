let
  pkgs = import <nixpkgs> {};
  
in with pkgs; rec {

  # External Dependencies (personal)
  fil = (callPackage (builtins.fetchurl "https://raw.githubusercontent.com/FreeYourSoul/FiL/9a38ffe32a7cd709efe56bf7f05255259acb95a0/fil-recipee.nix")
    { rev="9a38ffe32a7cd709efe56bf7f05255259acb95a0"; }
  );

  fseam = (callPackage (builtins.fetchurl "https://raw.githubusercontent.com/FreeYourSoul/FSeam/9f0ca84beab1b4c6152238820029450d589f319d/fseam-recipee.nix")
        { rev="9f0ca84beab1b4c6152238820029450d589f319d"; }
      );

  # External Dependencies (not default nixpkg)
  tmxlite = (callPackage ./nix/dependency/tmxlite.nix) { };

  # FyS Dependencies
  dispatcher = (callPackage ./nix/recipes/dispatcher.nix) { inherit fil; };
  world_service = (callPackage ./nix/recipes/world_service.nix) { inherit fil tmxlite fseam; };
}