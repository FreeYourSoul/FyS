{
  description = "Sol3 : C++ LUA binding library.";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system: let pkgs = nixpkgs.legacyPackages.${system}; in rec {
    
        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "sol3";
          version = "3.2.3";
          src = pkgs.fetchgit {
            url = "https://github.com/ThePhD/sol2.git";
            rev = "c068aefbeddb3dd1f1fd38d42843ecb49a3b4cdb";
            sha256 = "sha256-etJMwVxdsnd3/JAI7an1Ve5cmYfXy5IKMTe1/vRwYwk=";
          };
          nativeBuildInputs = with pkgs; [ cmake ];
        };

     });
}