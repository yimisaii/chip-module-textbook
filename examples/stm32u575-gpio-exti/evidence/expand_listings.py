"""Build the HTML reading edition from chapter.md and the project C sources."""

from pathlib import Path
import re
import subprocess


root = Path(__file__).resolve().parent.parent


def listing(directive):
    path, anchor = directive.groups()
    source = (root / path).read_text(encoding="utf-8").splitlines()
    markers = [line.strip() for line in source]
    begin = markers.index(f"// textbook:begin {anchor}")
    end = markers.index(f"// textbook:end {anchor}", begin + 1)
    return "```c\n" + "\n".join(source[begin + 1:end]) + "\n```"


chapter = (root / "chapter.md").read_text(encoding="utf-8")
expanded, count = re.subn(r"^@code (\S+) (\S+)$", listing, chapter, flags=re.M)
if count == 0:
    raise ValueError("No code listings were expanded")

subprocess.run(
    ["pandoc", "-f", "markdown", "-t", "html", "-s", "-V", "maxwidth=52rem",
     "-M", "lang=zh-CN", "-M", "pagetitle=STM32U575 GPIO 按键中断与消抖",
     "-o", str(root / "chapter.html")],
    input=expanded,
    encoding="utf-8",
    check=True,
)
