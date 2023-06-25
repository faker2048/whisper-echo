# Usage: python client.py --file test.wav --host localhost --port 8008

import asyncio
import numpy as np
import websockets
import librosa
import argparse
import msgpack


def read_audio_file(file_path):
    audio, sr = librosa.load(file_path, sr=16000, mono=True, dtype="float32")
    return audio


class WebsocketClient:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.uri = f"ws://{host}:{port}/whisper"
        self.websocket: websockets.WebSocketClientProtocol = None

    async def connect(self):
        self.websocket = await websockets.connect(self.uri)

    async def send_and_recv(self, data: str):
        await self.websocket.send(data)
        ret = await self.websocket.recv()
        # use msgpack
        ret = msgpack.unpackb(ret, raw=False)
        return ret


class WhisperWebsocketClient:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.uri = f"ws://{host}:{port}/whisper"
        self.client = WebsocketClient(host, port)

    async def connect(self):
        await self.client.connect()

    def create_message(
        self, type_str: str, sequence_number: int = -1, audio_data: bytes = bytes([])
    ):
        # Ensure datatype
        if not isinstance(type_str, str) or type_str not in ["start", "end", "data"]:
            raise ValueError("type_str must be one of 'start', 'end', 'data'")
        if not isinstance(sequence_number, int):
            raise ValueError("sequence_number must be an integer")
        elif not isinstance(audio_data, bytes):
            raise ValueError("audio_data must be bytes")
        # Create a dictionary representation of your message
        message = {
            "type_str": type_str,
            "sequence_number": sequence_number,
            "audio_data": audio_data,
        }

        # Use msgpack to serialize your message to a byte stream
        packed_message = msgpack.packb(message)

        return packed_message

    async def send_start(self):
        # use msgpack
        msg = self.create_message("start")
        ret = await self.client.send_and_recv(msg)
        print(ret)

    async def send_end(self):
        # use msgpack
        msg = self.create_message("end")
        ret = await self.client.send_and_recv(msg)
        print(ret)

    async def send_data(self, sequence_number: int, audio_data: bytes | np.ndarray):
        # use msgpack
        if isinstance(audio_data, np.ndarray):
            audio_data = audio_data.tobytes()
        msg = self.create_message("data", sequence_number, audio_data)
        ret = await self.client.send_and_recv(msg)
        print(ret)

    async def audio2text(self, audio: np.ndarray):
        pass


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--file", type=str, default="test.wav")
    parser.add_argument("--host", type=str, default="localhost")
    parser.add_argument("--port", type=int, default=8008)
    return parser.parse_args()


async def amain():
    args = parse_args()
    client = WhisperWebsocketClient(args.host, args.port)
    await client.connect()
    await client.send_start()
    audio = read_audio_file(args.file)
    # split into 4 chunks, just for demo. You can split into any number
    # of chunks or send the whole audio at once
    chunk_size = len(audio) // 4
    await client.send_data(0, audio[:chunk_size])
    await client.send_data(1, audio[chunk_size : 2 * chunk_size])
    await client.send_data(2, audio[2 * chunk_size : 3 * chunk_size])
    await client.send_data(3, audio[3 * chunk_size :])
    await client.send_end()


if __name__ == "__main__":
    asyncio.run(amain())
