{ stdenv, callPackage, cmake }:

stdenv.mkDerivation rec {
  version = "6.2.0"; # not official release (last tag as of 20/11/2020)
  pname = "chaiscript";

  src = builtins.fetchGit {
    url = "https://github.com/ChaiScript/ChaiScript.git";
    rev = "c8c9f805f6d53ff01a18d440851a872316eb626b";
  };

  buildInputs = [ cmake ];

  cmakeFlags = [ "-DBUILD_TESTING=OFF" ];

  enableParallelBuilding = true;
  doCheck = false;
}
