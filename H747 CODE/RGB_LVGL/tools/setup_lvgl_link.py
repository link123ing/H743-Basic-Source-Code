#!/usr/bin/env python3
"""Create Middlewares/LVGL junction inside RGB_LVGL (no spaces in path for GNU make)."""
import os
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LINK = ROOT / "Middlewares" / "LVGL"
PARENT = ROOT.parent


def find_lvgl_root() -> Path:
    for child in PARENT.iterdir():
        if not child.is_dir():
            continue
        candidate = child / "Middlewares" / "LVGL"
        if (candidate / "GUI" / "lvgl" / "lvgl.h").is_file():
            return candidate
    raise SystemExit(f"LVGL tree not found under {PARENT}")


def main() -> None:
    src = find_lvgl_root().resolve()
    LINK.parent.mkdir(parents=True, exist_ok=True)

    if LINK.exists() or LINK.is_symlink():
        if LINK.resolve() == src:
            print(f"OK: {LINK} -> {src}")
            return
        print(f"Remove existing {LINK} first", file=sys.stderr)
        sys.exit(1)

    import subprocess

    subprocess.check_call(["cmd", "/c", "mklink", "/J", str(LINK), str(src)])
    print(f"Created junction:\n  {LINK}\n  -> {src}")


if __name__ == "__main__":
    main()
