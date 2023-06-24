{ llvmPackages_15, cmake, fetchFromGitHub }:

llvmPackages_15.stdenv.mkDerivation rec {
  pname = "msgpack-c";

  version = "6.0.0";

  src = fetchFromGitHub {
    owner = "msgpack";
    repo = pname;
    rev = "cpp-${version}";
    sha256 = "sha256-Qea9zGLJ41D+l8h1Sg/KJI6Ou02jtbRIxYPGoabM8nY=";
  };

  nativeBuildInputs = [ cmake ];

  cmakeFlags = [ ];
}
