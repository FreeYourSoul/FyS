{
  description = "FyS Server";

  inputs = {
     nixpkgs.url = github:NixOS/nixpkgs;
     flake-utils.url = github:numtide/flake-utils;
  };

  outputs = { self, nixpkgs, flake-utils }:
     flake-utils.lib.eachDefaultSystem (system: 
     let pkgs = nixpkgs.legacyPackages.${system};
     in {

     	inherit ( import "./default.nix" { inherit world_service arena_service dispatcher; } );
	 
     });
}
