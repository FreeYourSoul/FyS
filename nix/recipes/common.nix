{}:
rec {
    version = "1.0.0-DEV";
    src = builtins.fetchGit {
        url = "https://github.com/FreeYourSoul/FyS.git";
        rev = "dc1885ad1cc7340667f3f180b29794bf198d76f9";
    };
}