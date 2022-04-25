{
  description = "C++ TMXLite library";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system: let pkgs = nixpkgs.legacyPackages.${system}; in rec {
    
        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "tmxlite";
          version = "1.2.1";
          src = pkgs.fetchgit {
            url = "https://github.com/fallahn/tmxlite.git";
            rev = "b87724c732bf8521364c9b1d49e3e2f4e3834b5c";
            sha256 = "sha256-SozPP7eKuMbe9q1miuQotRUMu3ri5DK0uEb8I52+o5E=";
          };
	  postUnpack = "sourceRoot=$sourceRoot/tmxlite";
	    
	  buildInputs = [ ];
          nativeBuildInputs = with pkgs; [ cmake ];

          cmakeFlags = [ "-DBUILD_TESTING=OFF" ];

          enableParallelBuilding = true;
  	  doCheck = false;
        };

	tmxlite = defaultPackage;

     });
}