{}:
rec {
    version = "1.0.0-DEV";
    # src = nix-gitignore.gitignoreSource [ ".git" ] /home/FyS/Project/FyS;
    src = builtins.fetchGit {
        url = "https://github.com/FreeYourSoul/FyS.git";
        rev = "84533fec838b8a41686460c20f380ac9e238c3f6";
    };
}