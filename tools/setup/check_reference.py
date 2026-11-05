import hashlib
import json
from pathlib import Path
import sys


def main():
    root = Path(__file__).resolve().parents[2]
    manifest = json.loads((root / "config/targets/wpse-v1.json").read_text(encoding="utf-8"))
    reference = root / "orig/wpse-v1/main.dol"
    expected = manifest["artifacts"][0]
    try:
        data = reference.read_bytes()
    except OSError:
        print("Missing reference: orig/wpse-v1/main.dol. Supply your own WPSE USA v1 main.dol.", file=sys.stderr)
        return 1
    if len(data) != expected["size"] or hashlib.sha256(data).hexdigest() != expected["sha256"]:
        print("Reference main.dol does not match the frozen WPSE USA v1 manifest in config/targets/wpse-v1.json.", file=sys.stderr)
        return 1
    print("Reference main.dol matches the frozen manifest.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
