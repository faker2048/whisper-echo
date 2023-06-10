{
  description = ''
    A Flake for the whisper.cpp project
  '';

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs:
    inputs.utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs-dev = import nixpkgs {
          inherit system;
        };
      in
      {
        devShells.default = pkgs-dev.mkShell.override
          {
            stdenv = pkgs-dev.llvmPackages_15.stdenv;
          }
          rec {
            name = "whisper";

            packages = with pkgs-dev; [
              llvmPackages_15.clang
              cmake
              cmakeCurses
              openai-whisper-cpp
            ];

            shellHook = ''
              export PS1="$(echo -e '\uf3e2') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
            '';

          };

        defaultPackage = pkgs-dev.callPackage ./default.nix { };
      });
}
