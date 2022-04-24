{
  description = "FyS Server";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
  };

  outputs = { self, nixpkgs, flake-utils }:
    {

      world_service = nixpkgs.callPackage ./nix/recipes/world_service.nix { };

    };
}
