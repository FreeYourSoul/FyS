{ stdenv, callPackage, cmake }:

stdenv.mkDerivation rec {
  version = "1.2.1";
  pname = "tmxlite";

  src = builtins.fetchGit {
    url = "https://github.com/fallahn/tmxlite.git";
    rev = "8aea23ea52a72f82931d950d878bbb1b946c99c0xo";
  };
  sourceRoot = "source/tmxlite";

  buildInputs = [ ];
  nativeBuildInputs = [ cmake ];

  cmakeFlags = [ "-DBUILD_TESTING=OFF" ];

  enableParallelBuilding = true;
  doCheck = false;
}
