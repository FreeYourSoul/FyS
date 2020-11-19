{ stdenv, callPackage, cmake, lua }:

stdenv.mkDerivation rec {
    version = "3.2.3";
    pname = "sol3";

    src = builtins.fetchGit {
        url = "https://github.com/ThePhD/sol2.git";
        rev = "c068aefbeddb3dd1f1fd38d42843ecb49a3b4cdb";
    };

    buildInputs = [ ];
    nativeBuildInputs = [ cmake ];

    cmakeFlags = [
      "-DBUILD_TESTING=OFF"
    ];

    enableParallelBuilding = true;
    doCheck = false;
}