# whisper_echo

:warning: **The project is currently under active development. Expect frequent updates and changes.**

`whisper_echo` is a server service repository, tailored for voice-to-text transcription using the whisper-cpp. The service takes in audio data, processes it through the Whisper system, and returns the corresponding text transcription. Implemented in C++, specifically designed for Nix environments.

## Getting Started

These instructions will guide you on how to clone, build, and test this project in your local Nix environment.

### Prerequisites
- [Nix package manager](https://nixos.org/download.html) installed and configured on your system.

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

### Run

```bash whisper_echo
./bin/whisper_echo ./models/ggml-base.en.bin
```

Instead `./models/ggml-base.en.bin` use your own model_path

### Client

Here is a usable Python version of the client example. Run `nix develop` to enter the Python environment, and then execute `python ./whisper_echo/examples/py_client/client.py --file path/to/your_audio.wav`.

## Running the tests

TODO

## Contributing

TODO

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
