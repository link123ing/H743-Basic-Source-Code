#!/usr/bin/env python3
from pathlib import Path

p = Path(__file__).resolve().parents[1] / "Makefile" / "CM7" / "lvgl.mk"
lines = []
for line in p.read_text(encoding="utf-8").splitlines():
    if line.strip().startswith('"') and ".c" in line:
        line = line.replace('"', "")
    lines.append(line)
p.write_text("\n".join(lines) + "\n", encoding="utf-8")
print("fixed", p)
