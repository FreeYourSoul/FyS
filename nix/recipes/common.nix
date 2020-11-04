{}:
rec {
    version = "1.0.0-DEV";
    src = builtins.fetchGit {
        url = "https://github.com/FreeYourSoul/FyS.git";
        rev = "be0181b575b84727b7556bd3d929615c0264ef82";
    };
}