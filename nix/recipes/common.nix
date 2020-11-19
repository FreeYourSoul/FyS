{pkgs ? import <nixpkgs> {}, use_revision ? builtins.getEnv "NIX_FYS_REVISION" }:
rec {
    version = "1.0.0-DEV";

    src = if (builtins.isNull use_revision || use_revision == "")
          then pkgs.nix-gitignore.gitignoreSource [ ".git" ] ./../..
          else builtins.fetchGit {
                 url = "https://github.com/FreeYourSoul/FyS.git";
                 rev = use_revision;
             };
}