{
  description = "Chaiscript library.";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system: let pkgs = nixpkgs.legacyPackages.${system}; in rec {
    
        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "chaiscript";
          version = "6.2.0";
          src = pkgs.fetchgit {
            url = "https://github.com/ChaiScript/ChaiScript.git";
            rev = "c8c9f805f6d53ff01a18d440851a872316eb626b";
            sha256 = "sha256-QSkf+vtErd7PZL7Y4rZmXQMbViA1mDOytBps1AZNt+w=";
          };
          nativeBuildInputs = with pkgs; [ cmake ];
        };

	chaiscript = defaultPackage;

     });
}