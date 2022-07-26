{ stdenv, callPackage, cmake, zeromq }:

stdenv.mkDerivation rec {
  version = "4.8.1";
  pname = "cppzmq";

  src = builtins.fetchGit {
    url = "https://github.com/zeromq/cppzmq.git";
    rev = "dd663fafd830466d34cba278c2cfd0f92eb67614";
  };

  buildInputs = [ zeromq cmake ];

  cmakeFlags = [ "-DCPPZMQ_BUILD_TESTS=OFF" ];

  enableParallelBuilding = true;
  doCheck = false;
}
