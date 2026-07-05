#!/usr/bin/env python3
"""Generate Makefile/CM7/lvgl.mk from 例程39 MDK project file list."""
import re
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
UVPROJX = Path(r"H:\STM32H7\例程39 压力测试\Projects\MDK-ARM\STM32H747.uvprojx")
MK_DIR = ROOT / "Makefile" / "CM7"
LVGL_PREFIX = "../../Middlewares/LVGL/"

SKIP_SUBSTR = ("lv_port_disp_template", "lv_port_indev_template")

EXTRA = [
    "BSP/LVGL/lv_port_disp.c",
    "BSP/LVGL/lv_port_indev.c",
    "CM7/Core/Src/lvgl_demo.c",
]

def main() -> None:
    text = UVPROJX.read_text(encoding="utf-8", errors="ignore")
    paths = re.findall(
        r"<FilePath>\.\.\\\.\.\\Middlewares\\LVGL\\(.+?\.c)</FilePath>", text
    )
    sources = []
    for rel in paths:
        if any(s in rel for s in SKIP_SUBSTR):
            continue
        sources.append((LVGL_PREFIX + rel.replace("\\", "/")).replace("//", "/"))

    for rel in EXTRA:
        p = f"../../{rel}"
        if (ROOT / rel).exists() and p not in sources:
            sources.append(p)

    sources = sorted(set(sources))

    out = MK_DIR / "lvgl.mk"
    lines = [
        "# Generated from 例程39 MDK project (tools/gen_lvgl_mk_from_uvprojx.py)",
        "LVGL_BASE := ../../Middlewares/LVGL",
        "LVGL_SOURCES := \\",
    ]
    for p in sources:
        lines.append(f"  {p} \\")
    lines[-1] = lines[-1].rstrip(" \\")
    lines += [
        "",
        "LVGL_INCLUDES := \\",
        "  -I$(LVGL_BASE)/GUI \\",
        "  -I$(LVGL_BASE)/GUI/lvgl \\",
        "  -I$(LVGL_BASE)/GUI/lvgl/examples/porting \\",
        "  -I../../BSP/LVGL",
        "",
    ]
    out.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {len(sources)} sources to {out}")

if __name__ == "__main__":
    main()
