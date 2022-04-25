{ stdenv, callPackage, cmake }:

stdenv.mkDerivation rec {
  version = "1.7.0";
  pname = "spdlog";

  src = builtins.fetchGit {
    url = "https://github.com/gabime/spdlog/";
    rev = "616caa5d30172b65cc3a06800894c575d70cb8e6";
  };

  buildInputs = [ ];
  nativeBuildInputs = [ cmake ];

  cmakeFlags = [ "-DBUILD_TESTING=OFF" ];

  enableParallelBuilding = true;
  doCheck = false;
}
