{
  description = ''
    A server service designed to process voice inputs. The server receives audio data, passes it to 
    the openai's whisper model, converts the voice input into textual data, and sends the resulting 
    transcription back to the requester.
  '';

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    utils.url = "github:numtide/flake-utils";

    vitalpkgs = {
      url = "github:nixvital/vitalpkgs";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    nixpackbox = {
      url = "github:faker2048/NixPackBox";
      inputs.nixpkgs.follows = "nixpkgs";
    };

  };

  outputs = { self, nixpkgs, ... }@inputs:
    inputs.utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs-dev = import nixpkgs {
          inherit system;
          overlays = [
            inputs.vitalpkgs.overlays.default
            (self: super: { whisper-cpp-dev = inputs.nixpackbox.packages.${system}.whisper-cpp-dev; })
          ];
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
              # tools for building
              llvmPackages_15.clang
              cmake
              cmakeCurses

              # deps
              whisper-cpp-dev
              spdlog
              drogon

              # deps for testing
              gtest
              gmock

              # tools for development
              vscode-include-fix
            ];

            shellHook = ''
              export PS1="$(echo -e '\uf3e2') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
            '';

          };

        defaultPackage = pkgs-dev.callPackage ./default.nix { };
      });
}
