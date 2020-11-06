let
  pkgs = import <nixpkgs> {};
  
in with pkgs; rec {

  # External Dependencies

  fil = (callPackage (builtins.fetchurl "https://raw.githubusercontent.com/FreeYourSoul/FiL/9a38ffe32a7cd709efe56bf7f05255259acb95a0/fil-recipee.nix") 
    { rev="9a38ffe32a7cd709efe56bf7f05255259acb95a0"; }
  );

  # FyS Dependencies
  dispatcher = (callPackage ./recipes/dispatcher.nix) { inherit fil; }; 
}