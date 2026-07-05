#!/usr/bin/env python3
"""Convert GBK-encoded BSP sources to UTF-8."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FILES = [
    ROOT / "BSP" / "LCD" / "lcd.c",
    ROOT / "BSP" / "LCD" / "lcd.h",
    ROOT / "BSP" / "LCD" / "lcd_ltdc.c",
    ROOT / "BSP" / "LCD" / "lcdfont.h",
]

def convert(path: Path) -> None:
    raw = path.read_bytes()
    text = None
    mode = None

    try:
        candidate = raw.decode("utf-8")
        if "显示" in candidate or "驱动" in candidate or "慧勤" in candidate:
            print(f"skip (already utf-8): {path}")
            return
        try:
            text = candidate.encode("latin-1").decode("gbk")
            mode = "mojibake latin-1 -> gbk"
        except (UnicodeDecodeError, UnicodeEncodeError):
            text = None
    except UnicodeDecodeError:
        pass

    if text is None:
        for enc in ("gbk", "gb2312", "cp936"):
            try:
                text = raw.decode(enc)
                mode = enc
                break
            except UnicodeDecodeError:
                continue

    if text is None:
        raise SystemExit(f"cannot decode: {path}")

    path.write_text(text, encoding="utf-8", newline="\n")
    print(f"converted ({mode}): {path}")

def main() -> None:
    for path in FILES:
        if path.is_file():
            convert(path)

if __name__ == "__main__":
    main()
