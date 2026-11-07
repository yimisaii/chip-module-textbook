"""Build an editable Word chapter from the supplied Markdown and C sources."""

import argparse
import re
from pathlib import Path

from docx import Document
from docx.enum.style import WD_STYLE_TYPE
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_TAB_ALIGNMENT
from docx.opc.constants import RELATIONSHIP_TYPE
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Mm, Pt, RGBColor


INLINE = re.compile(r"(\x60[^\x60]+\x60|\[[^]]+]\([^)]+\)|\*\*[^*]+\*\*)")
IMAGE = re.compile(r"!\[([^]]*)]\(([^)]+)\)")
EQUATION = re.compile(r"(.+?)\u3000+\s*（(\d+)）")
FENCES = (chr(96) * 3, "~" * 3)


def configure_style(style, western, chinese, size):
    style.font.name = western
    style.font.size = Pt(size)
    style.font.color.rgb = RGBColor(0, 0, 0)
    style.element.get_or_add_rPr().rFonts.set(qn("w:eastAsia"), chinese)


def make_document():
    document = Document()
    section = document.sections[0]
    section.page_width, section.page_height = Mm(185), Mm(280)
    section.top_margin, section.bottom_margin = Mm(20), Mm(20)
    section.left_margin, section.right_margin = Mm(20), Mm(20)
    styles = document.styles
    configure_style(styles["Normal"], "Calibri", "宋体", 10.5)
    styles["Normal"].paragraph_format.line_spacing = 1.28
    styles["Normal"].paragraph_format.space_after = Pt(6)
    styles["Normal"].paragraph_format.widow_control = True
    for name, size in (("Title", 21), ("Heading 1", 14), ("Heading 2", 11.5)):
        configure_style(styles[name], "Calibri", "黑体", size)
        styles[name].font.bold = True
        styles[name].paragraph_format.keep_with_next = True
        styles[name].paragraph_format.space_before = Pt(11)
        styles[name].paragraph_format.space_after = Pt(6)
    title_props = styles["Title"].element.get_or_add_pPr()
    for border in title_props.findall(qn("w:pBdr")):
        title_props.remove(border)
    caption = styles["Caption"]
    configure_style(caption, "Calibri", "宋体", 9)
    caption.font.italic = False
    caption.paragraph_format.alignment = WD_ALIGN_PARAGRAPH.CENTER
    caption.paragraph_format.space_after = Pt(8)
    listing = styles.add_style("Source Listing", WD_STYLE_TYPE.PARAGRAPH)
    configure_style(listing, "Consolas", "微软雅黑", 8.5)
    listing.paragraph_format.line_spacing = 1.14
    listing.paragraph_format.space_after = Pt(0)
    listing.paragraph_format.left_indent = Mm(3)
    listing.paragraph_format.right_indent = Mm(2)
    shade = OxmlElement("w:shd")
    shade.set(qn("w:fill"), "F2F2F2")
    listing.element.get_or_add_pPr().append(shade)

    footer = section.footer.paragraphs[0]
    footer.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    page = OxmlElement("w:fldSimple")
    page.set(qn("w:instr"), "PAGE")
    footer._p.append(page)
    return document


def add_text(paragraph, value):
    for part in INLINE.split(value):
        if part.startswith("**") and part.endswith("**"):
            paragraph.add_run(part[2:-2]).bold = True
        elif part.startswith(chr(96)) and part.endswith(chr(96)):
            paragraph.add_run(part[1:-1]).font.name = "Consolas"
        elif part.startswith("[") and (link := re.fullmatch(r"\[([^]]+)]\(([^)]+)\)", part)):
            label, url = link.groups()
            if url.startswith(("https://", "http://")):
                hyperlink = OxmlElement("w:hyperlink")
                hyperlink.set(qn("r:id"), paragraph.part.relate_to(
                    url, RELATIONSHIP_TYPE.HYPERLINK, is_external=True))
                run = OxmlElement("w:r")
                properties = OxmlElement("w:rPr")
                underline = OxmlElement("w:u")
                underline.set(qn("w:val"), "single")
                properties.append(underline)
                run.append(properties)
                text = OxmlElement("w:t")
                text.text = label
                run.append(text)
                hyperlink.append(run)
                paragraph._p.append(hyperlink)
            else:
                paragraph.add_run(label)
        else:
            paragraph.add_run(part)


def add_paragraph(document, value, style=None):
    paragraph = document.add_paragraph(style=style)
    add_text(paragraph, value)
    return paragraph


def extract_code(path, anchor):
    lines = path.read_text(encoding="utf-8-sig").splitlines()
    begin = [i for i, line in enumerate(lines) if line.strip() == f"// textbook:begin {anchor}"]
    end = [i for i, line in enumerate(lines) if line.strip() == f"// textbook:end {anchor}"]
    if len(begin) != 1 or len(end) != 1 or end[0] <= begin[0] + 1:
        raise ValueError(f"{path}: invalid code boundaries for {anchor}")
    return lines[begin[0] + 1:end[0]]


def add_code(document, lines):
    for i, line in enumerate(lines):
        paragraph = document.add_paragraph(line, style="Source Listing")
        paragraph.paragraph_format.keep_with_next = (
            i < len(lines) - 1 and (not line.strip() or len(lines) <= 13)
        )
        if i == len(lines) - 1:
            paragraph.paragraph_format.space_after = Pt(5)


def add_figure(document, chapter_root, png_dir, description, path, caption):
    asset = chapter_root / path
    if asset.suffix.lower() == ".svg":
        if not asset.is_file() or png_dir is None:
            raise ValueError(f"provide the SVG and its raster in --figure-png-dir: {asset}")
        asset = png_dir / asset.with_suffix(".png").name
    paragraph = document.add_paragraph()
    paragraph.alignment = WD_ALIGN_PARAGRAPH.CENTER
    paragraph.paragraph_format.keep_with_next = True
    paragraph.add_run().add_picture(str(asset), width=Mm(132))
    paragraph._p.xpath(".//wp:docPr")[0].set("descr", description)
    add_paragraph(document, caption, "Caption")


def add_table(document, lines):
    cells = [[part.strip() for part in line.strip("|").split("|")] for line in lines]
    rows = [row for row in cells if not all(re.fullmatch(r":?-{3,}:?", value) for value in row)]
    table = document.add_table(rows=0, cols=len(rows[0]))
    table.style = "Table Grid"
    for i, row in enumerate(rows):
        if len(row) != len(rows[0]):
            raise ValueError("table row has the wrong number of cells")
        cells = table.add_row()
        row_props = cells._tr.get_or_add_trPr()
        row_props.append(OxmlElement("w:cantSplit"))
        if i == 0:
            row_props.append(OxmlElement("w:tblHeader"))
        for cell, value in zip(cells.cells, row):
            paragraph = cell.paragraphs[0]
            add_text(paragraph, value)
            paragraph.paragraph_format.space_after = Pt(3)
            if i == 0:
                paragraph.paragraph_format.keep_with_next = True
                for run in paragraph.runs:
                    run.bold = True


def build(chapter, output, png_dir):
    document = make_document()
    lines = chapter.read_text(encoding="utf-8-sig").splitlines()
    line = 0
    title_seen = False
    break_before_heading = False
    while line < len(lines):
        value = lines[line].strip()
        line += 1
        if not value:
            continue
        following = line
        while following < len(lines) and not lines[following].strip():
            following += 1
        next_value = lines[following].strip() if following < len(lines) else ""
        if value.startswith("# "):
            if title_seen:
                raise ValueError("the chapter must have one title")
            title_seen = True
            add_paragraph(document, value[2:], "Title")
            document.core_properties.title = value[2:]
        elif value.startswith(("## ", "### ")):
            hashes, heading = value.split(" ", 1)
            paragraph = add_paragraph(document, heading, f"Heading {len(hashes) - 1}")
            if break_before_heading:
                paragraph.paragraph_format.page_break_before = True
                break_before_heading = False
        elif value == "@page":
            if next_value.startswith(("## ", "### ")):
                break_before_heading = True
            else:
                document.add_page_break()
        elif value.startswith("@code "):
            _, path, anchor = value.split()
            add_code(document, extract_code(chapter.parent / path, anchor))
        elif figure := IMAGE.fullmatch(value):
            description, path = figure.groups()
            caption = description
            if re.match(r"图\s*\d", next_value):
                caption = next_value
                line = following + 1
            add_figure(document, chapter.parent, png_dir, description, path, caption)
        elif formula := EQUATION.fullmatch(value):
            expression, number = formula.groups()
            paragraph = document.add_paragraph()
            tabs = paragraph.paragraph_format.tab_stops
            tabs.add_tab_stop(Mm(72.5), WD_TAB_ALIGNMENT.CENTER)
            tabs.add_tab_stop(Mm(144), WD_TAB_ALIGNMENT.RIGHT)
            paragraph.add_run(f"\t{expression}\t（{number}）")
        elif value.startswith(FENCES):
            fence = value[:3]
            start = line
            while line < len(lines) and lines[line].strip() != fence:
                line += 1
            if line == len(lines):
                raise ValueError("unterminated fenced block")
            add_code(document, lines[start:line])
            line += 1
        elif value.startswith("|"):
            start = line - 1
            while line < len(lines) and lines[line].strip().startswith("|"):
                line += 1
            add_table(document, [row.strip() for row in lines[start:line]])
        elif value.startswith("- "):
            add_paragraph(document, value[2:], "List Bullet")
        elif value.startswith("> "):
            add_paragraph(document, value[2:], "Quote")
        else:
            paragraph = add_paragraph(document, value)
            if next_value.startswith(("@code ", "|")):
                paragraph.paragraph_format.keep_with_next = True
    if not title_seen:
        raise ValueError("chapter title is missing")
    document.save(output)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("chapter", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--figure-png-dir", type=Path,
                        help="directory with a PNG for each SVG, named after its stem")
    args = parser.parse_args()
    build(args.chapter, args.output, args.figure_png_dir)


if __name__ == "__main__":
    main()
