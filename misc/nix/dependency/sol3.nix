{ stdenv, callPackage, cmake, lua }:

stdenv.mkDerivation rec {
  pname = "sol3";
  version = "3.2.3";
  
  src = builtins.fetchGit {
    url = "https://github.com/FreeYourSoul/sol2.git";
    rev = "d0eba0a7544b0b62e37f08972c0f431f3c0e96b4";
  };

  buildInputs = [ ];
  nativeBuildInputs = [ cmake ];

  cmakeFlags = [ "-DBUILD_TESTING=OFF" ];

  enableParallelBuilding = true;
  doCheck = false;
}
