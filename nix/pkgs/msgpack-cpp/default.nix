{ llvmPackages_15, cmake, fetchFromGitHub }:

llvmPackages_15.stdenv.mkDerivation rec {
  pname = "msgpack-c";

  version = "6.0.0";

  src = fetchFromGitHub {
    owner = "msgpack";
    repo = pname;
    rev = "cpp-${version}";
    sha256 = "sha256-p0eLd0fHhsgnRomubYadumMNiC2itdePJC9B55m49LI=";
  };

  nativeBuildInputs = [ cmake ];

  cmakeFlags = [
    "-DMSGPACK_USE_BOOST=OFF"
    "-DMSGPACK_CXX20=ON"
  ];
}
