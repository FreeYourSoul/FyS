{ stdenv, callPackage, cmake, zeromq }:

stdenv.mkDerivation rec {
    version = "4.8.0";
    pname = "cppzmq";

    src = builtins.fetchGit {
        url = "https://github.com/zeromq/cppzmq.git";
        rev = "03243ad64db652c43d150e9fb5a80a13c1a1a4a9";
    };

    buildInputs = [ zeromq cmake ];

    cmakeFlags = [
      "-DCPPZMQ_BUILD_TESTS=OFF"
    ];

    enableParallelBuilding = true;
    doCheck = false;
}