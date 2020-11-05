{}:
rec {
    version = "1.0.0-DEV";
    src = builtins.fetchGit {
        url = "https://github.com/FreeYourSoul/FyS.git";
        rev = "76c656becf90e033dc7ee4d802c2390f9baae89d";
    };
}