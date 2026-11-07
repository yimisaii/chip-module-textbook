// Convert editable chapter SVGs to 144 dpi PNGs for Word.
const fs = require("node:fs/promises");
const path = require("node:path");
const sharp = require("sharp");

async function main() {
  const [output, ...figures] = process.argv.slice(2);
  if (!output || figures.length === 0) {
    throw new Error("Usage: node rasterize_figures.cjs OUTPUT_DIR FIGURE.svg [...]");
  }
  await fs.mkdir(output, { recursive: true });
  for (const figure of figures) {
    const filename = path.basename(figure, ".svg") + ".png";
    await sharp(figure, { density: 144 }).png().toFile(path.join(output, filename));
  }
}

main().catch((error) => {
  console.error(error.message);
  process.exitCode = 1;
});
