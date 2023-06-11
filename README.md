# whisper_echo

`whisper_echo` is a server service repository, tailored for voice-to-text transcription using the Whisper tool. The service takes in audio data, processes it through the Whisper system, and returns the corresponding text transcription. The service is implemented in C++, specifically designed for Nix environments.

## Getting Started

These instructions will guide you on how to clone, build, and test this project in your local Nix environment.

### Prerequisites

- A C++ compiler supporting at least C++20.
- [Nix package manager](https://nixos.org/download.html) installed and configured on your system.
- CMake for building the project (version 3.17 or later).

### Installing Dependencies

This project utilizes Nix, a potent package manager that simplifies dependency management. Run the following command to install all necessary dependencies:

```bash
nix develop
```

### Building

```bash
git clone https://github.com/faker2048/whisper_echo.git
cd whisper_echo
mkdir build && cd build
cmake ..
make
```

Upon successful build, the resulting binary can be found in the `build` directory.

## Running the tests

Explain how to run the automated tests for this system.

## Contributing

TODO

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
