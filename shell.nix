{ pkgs ? import <nixpkgs> { }, d ? import ./default.nix }:
pkgs.mkShell {
  # buildInputs is for dependencies you'd need "at run time",
  # were you to to use nix-build not nix-shell and build whatever you were working on
  buildInputs = (d.dispatcher.buildInputs ++ d.arena_service.buildInputs
    ++ d.world_service.buildInputs ++ d.fil.buildInputs ++ d.fseam.buildInputs
    ++ d.chaiscript.buildInputs ++ d.tmxlite.buildInputs
    ++ d.cppzmq.buildInputs);
}
