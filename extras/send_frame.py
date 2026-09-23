"""Run against example 05_BinarySerial. Requires pyserial."""
import argparse
import time
import serial


def encode(objects):
    if len(objects) > 16 or sum(map(len, objects)) > 256:
        raise ValueError("Maximum 16 objects / 256 total points")
    packet = bytearray([0xFF, len(objects)])
    for points in objects:
        if not 1 <= len(points) <= 64:
            raise ValueError("Each object needs 1..64 points")
        packet.append(len(points))
        for x, y in points:
            if not (0 <= x <= 15 and 0 <= y <= 15):
                raise ValueError("Coordinates must be 0..15")
            packet.append((x << 4) | y)
    return packet


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default="/dev/cu.usbmodem2101")
    args = parser.parse_args()
    with serial.Serial(args.port, 115200, timeout=2, write_timeout=2) as port:
        time.sleep(2)  # Uno resets when the port opens.
        port.reset_input_buffer()
        packet = encode([[(2, 2), (13, 2), (13, 13), (2, 13), (2, 2)],
                         [(8, 8)]])
        port.write(packet)
        if port.read(1) != b"K":
            raise RuntimeError("No acknowledgement. Check sketch, port and wiring.")
        print("Frame accepted. The Uno will keep drawing it.")


if __name__ == "__main__":
    main()
