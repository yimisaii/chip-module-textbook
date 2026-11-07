"""Retrieve cited Markdown evidence for one device, peripheral and SDK."""

import argparse
import re
from pathlib import Path


def identifier(value):
    if not re.fullmatch(r"[a-z0-9]+(?:[.-][a-z0-9]+)*", value):
        raise argparse.ArgumentTypeError("use lowercase IDs such as stm32u575 or hal1-1.6.3")
    return value


def search(scope, sdk, terms):
    for path in sorted(scope.glob("*.md")):
        lines = path.read_text(encoding="utf-8-sig").splitlines()
        if not lines or lines[0].strip() != f"sdk: {sdk}":
            continue
        if len(lines) < 3 or not lines[1].startswith("source: ") or not lines[1][8:].strip():
            raise ValueError(f"{path}: missing source on the second line")
        source = lines[1][8:].strip()
        title = next((line[2:] for line in lines[2:] if line.startswith("# ")), path.stem)
        body = "\n".join(lines[2:])
        for match in re.finditer(r"\S[^\n]*(?:\n[^\n]+)*", body):
            passage = match.group()
            if passage.startswith("#") and "\n" not in passage:
                continue
            score = sum(term in passage.casefold() for term in terms)
            if score:
                line = body.count("\n", 0, match.start()) + 3
                yield score, path, line, title, source, passage


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chip", required=True, type=identifier)
    parser.add_argument("--peripheral", required=True, type=identifier)
    parser.add_argument("--sdk", required=True, type=identifier)
    parser.add_argument("--library", type=Path,
                        default=Path(__file__).resolve().parents[1] / "references" / "platforms")
    parser.add_argument("query", help="space-separated identifiers or search terms")
    args = parser.parse_args()
    terms = set(args.query.casefold().split())
    if not terms:
        parser.error("provide at least one search term")
    scope = args.library / args.chip / args.peripheral
    try:
        hits = sorted(search(scope, args.sdk, terms),
                      key=lambda hit: (-hit[0], hit[1].name, hit[2]))
    except (OSError, ValueError) as error:
        parser.exit(2, f"{error}\n")
    if not hits:
        parser.exit(1, "No evidence matched this device, peripheral and SDK.\n")
    for _, path, line, title, source, passage in hits[:3]:
        print(f"{title}\n{path.relative_to(args.library).as_posix()}:{line}")
        print(f"{source}\n{passage}\n")


if __name__ == "__main__":
    main()
