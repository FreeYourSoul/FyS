{ stdenv, callPackage, cmake }:

stdenv.mkDerivation rec {
  version = "6.2.0"; # not official release (last tag as of 20/11/2020)
  pname = "chaiscript";

  src = builtins.fetchGit {
    url = "https://github.com/nlohmann/json.git";
    rev = "db78ac1d7716f56fc9f1b030b715f872f93964e4";
  };

  buildInputs = [ cmake ];

  cmakeFlags = [ "-DBUILD_TESTING=OFF" ];

  enableParallelBuilding = true;
  doCheck = false;
}
