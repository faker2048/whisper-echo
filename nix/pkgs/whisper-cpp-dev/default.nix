{ llvmPackages_15, cmake, fetchFromGitHub }:

llvmPackages_15.stdenv.mkDerivation rec {
  pname = "whisper.cpp";

  version = "1.4.2";

  src = fetchFromGitHub {
    owner = "ggerganov";
    repo = pname;
    rev = "v${version}";
    sha256 = "sha256-Qea9zGLJ41D+l8h1Sg/KJI6Ou02jtbRIxYPGoabM8nY=";
  };

  nativeBuildInputs = [ cmake ];

  # TODO - add cuda to buildInputs and add WHISPER_CUBLAS=1 to cmakeFlags

}
