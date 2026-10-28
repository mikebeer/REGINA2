// Layout helpers for the Regina OO manual. The look follows the style of
// an IBM publication: A4, Arial headings, Times body text, numbered
// figures, grey table headers and framed screens for code.
const d = require('docx');
const {
  Paragraph, TextRun, Table, TableRow, TableCell, WidthType, BorderStyle,
  ShadingType, AlignmentType, HeadingLevel, TabStopType, PageBreak,
  LevelFormat, ImageRun,
} = d;
const fs = require('fs');
const path = require('path');

// the colours of the Queen of Clubs on the cover
const RED = 'B3262A', GOLD = 'F6D148', GOLD_LIGHT = 'FCF0BF', BLUE = '1C78BE';

// an image of doc/manual/art, w and h in points
const ART = path.join(__dirname, 'art');
function img(file, w, h, alt) {
  return new ImageRun({
    type: file.endsWith('.jpg') ? 'jpg' : 'png',
    data: fs.readFileSync(path.join(ART, file)),
    // docx sizes images in pixels of 1/96 inch
    transformation: { width: Math.round(w * 4 / 3), height: Math.round(h * 4 / 3) },
    altText: { title: alt || file, description: alt || file, name: file },
  });
}
// an ornament in a paragraph of its own
const ornament = (file, w, h, before = 0, after = 0) => new Paragraph({
  alignment: AlignmentType.CENTER, spacing: { before, after }, children: [img(file, w, h)] });

const BODY_W = 9638;                 // A4 width minus 2 x 1134 twips
const ARIAL = 'Arial', TIMES = 'Times New Roman', COURIER = 'Courier New';
const GRID = { style: BorderStyle.SINGLE, size: 4, color: '808080' };
const BOX = { style: BorderStyle.SINGLE, size: 4, color: '000000' };

let figure = 0;
const figures = [];

// inline markup: `code`, **bold**, __italic__
function runs(text, base = {}) {
  const out = [];
  const re = /(`[^`]+`|\*\*[^*]+\*\*|__[^_]+__)/g;
  let last = 0, m;
  while ((m = re.exec(text)) !== null) {
    if (m.index > last) out.push(new TextRun({ text: text.slice(last, m.index), ...base }));
    const t = m[0];
    if (t.startsWith('`'))
      out.push(new TextRun({ text: t.slice(1, -1), font: COURIER, size: (base.size || 21) - 2,
                             bold: base.bold }));
    else if (t.startsWith('**'))
      out.push(new TextRun({ text: t.slice(2, -2), ...base, bold: true }));
    else
      out.push(new TextRun({ text: t.slice(2, -2), ...base, italics: true }));
    last = m.index + t.length;
  }
  if (last < text.length) out.push(new TextRun({ text: text.slice(last), ...base }));
  return out;
}

const P = (text, opts = {}) => new Paragraph({
  children: runs(text), spacing: { after: 120 }, ...opts });

// a chapter title; the band of white clubs is added after it (see decorate)
const H1 = (text, pageBreak = true) => {
  const p = new Paragraph({
    heading: HeadingLevel.HEADING_1, pageBreakBefore: pageBreak, children: [new TextRun(text)] });
  p._isH1 = true;
  return p;
};
// the band (from the collar of the queen) under every chapter title and a
// flower at the end of every chapter
function decorate(children) {
  const out = [];
  children.forEach((c, i) => {
    if (c && c._isH1 && i > 0) out.push(ornament('flower.png', 26, 21, 240, 0));
    out.push(c);
    if (c && c._isH1)
      out.push(new Paragraph({ spacing: { before: 0, after: 300 }, children: [img('band.png', 482, 16, 'band of clubs')] }));
  });
  return out;
}
const H2 = (text) => new Paragraph({ heading: HeadingLevel.HEADING_2, children: [new TextRun(text)] });
const H3 = (text) => new Paragraph({ heading: HeadingLevel.HEADING_3, children: [new TextRun(text)] });

const UL = (items) => items.map((t) => new Paragraph({
  children: runs(t), numbering: { reference: 'bullets', level: 0 }, spacing: { after: 60 } }));
const OL = (items, ref = 'steps') => items.map((t) => new Paragraph({
  children: runs(t), numbering: { reference: ref, level: 0 }, spacing: { after: 60 } }));

function caption(text) {
  figure += 1;
  figures.push(text);
  return new Paragraph({
    spacing: { before: 80, after: 200 },
    children: [
      new TextRun({ text: `Figure ${figure}. `, font: ARIAL, bold: true, size: 18 }),
      new TextRun({ text, font: ARIAL, size: 18 }),
    ],
  });
}

const cellMargins = { top: 60, bottom: 60, left: 100, right: 100 };

// a table with a grey header row; widths in twips must add up to BODY_W
function table(headers, rows, widths, cap) {
  const head = new TableRow({
    tableHeader: true, cantSplit: true,
    children: headers.map((h, i) => new TableCell({
      width: { size: widths[i], type: WidthType.DXA }, margins: cellMargins,
      shading: { fill: GOLD_LIGHT, type: ShadingType.CLEAR, color: 'auto' },
      children: [new Paragraph({ children: runs(h, { font: ARIAL, bold: true, size: 18 }) })],
    })),
  });
  const body = rows.map((r) => new TableRow({
    cantSplit: true,
    children: r.map((c, i) => new TableCell({
      width: { size: widths[i], type: WidthType.DXA }, margins: cellMargins,
      children: String(c).split('\n').map((line) =>
        new Paragraph({ children: runs(line, { size: 18 }) })),
    })),
  }));
  const t = new Table({
    width: { size: BODY_W, type: WidthType.DXA }, columnWidths: widths,
    borders: { top: GRID, bottom: GRID, left: GRID, right: GRID, insideHorizontal: GRID,
               insideVertical: GRID },
    rows: [head, ...body],
  });
  return cap ? [t, caption(cap)] : [t, new Paragraph({ spacing: { after: 120 }, children: [] })];
}

// a framed screen with monospaced text
function screen(text, cap) {
  const lines = text.replace(/\s+$/, '').split('\n');
  const t = new Table({
    width: { size: BODY_W, type: WidthType.DXA }, columnWidths: [BODY_W],
    borders: { top: BOX, bottom: BOX, left: BOX, right: BOX, insideHorizontal: BOX,
               insideVertical: BOX },
    rows: [new TableRow({ children: [new TableCell({
      width: { size: BODY_W, type: WidthType.DXA },
      shading: { fill: 'F2F2F2', type: ShadingType.CLEAR, color: 'auto' },
      margins: { top: 100, bottom: 100, left: 140, right: 140 },
      children: lines.map((l) => new Paragraph({
        spacing: { after: 0, line: 228 },
        children: [new TextRun({ text: l.length ? l : ' ', font: COURIER, size: 17 })],
      })),
    })] })],
  });
  return cap ? [t, caption(cap)] : [t, new Paragraph({ spacing: { after: 120 }, children: [] })];
}

const pageBreak = () => new Paragraph({ children: [new PageBreak()] });

const styles = {
  default: { document: { run: { font: TIMES, size: 21 } } },
  paragraphStyles: [
    { id: 'Heading1', name: 'Heading 1', basedOn: 'Normal', next: 'Normal', quickFormat: true,
      run: { font: ARIAL, size: 36, bold: true },
      paragraph: { spacing: { before: 0, after: 120 }, outlineLevel: 0 } },
    { id: 'Heading2', name: 'Heading 2', basedOn: 'Normal', next: 'Normal', quickFormat: true,
      run: { font: ARIAL, size: 26, bold: true, color: RED },
      paragraph: { spacing: { before: 320, after: 140 }, outlineLevel: 1, keepNext: true } },
    { id: 'TOC1', name: 'toc 1', basedOn: 'Normal', next: 'Normal',
      run: { font: 'Arial', size: 19, bold: true },
      paragraph: { spacing: { before: 120, after: 20 } } },
    { id: 'TOC2', name: 'toc 2', basedOn: 'Normal', next: 'Normal',
      run: { size: 19 }, paragraph: { indent: { left: 280 }, spacing: { after: 0 } } },
    { id: 'Heading3', name: 'Heading 3', basedOn: 'Normal', next: 'Normal', quickFormat: true,
      run: { font: ARIAL, size: 22, bold: true, color: BLUE },
      paragraph: { spacing: { before: 220, after: 100 }, outlineLevel: 2, keepNext: true } },
  ],
};

const numbering = {
  config: [
    { reference: 'bullets', levels: [{ level: 0, format: LevelFormat.BULLET, text: '\u2663',
      alignment: AlignmentType.LEFT, style: { run: { font: 'DejaVu Sans', size: 16 },
        paragraph: { indent: { left: 540, hanging: 270 } } } }] },
    ...['steps', 's2', 's3', 's4', 's5', 's6'].map((ref) => ({
      reference: ref, levels: [{ level: 0, format: LevelFormat.DECIMAL, text: '%1.',
        alignment: AlignmentType.LEFT, style: { paragraph: { indent: { left: 540, hanging: 300 } } } }] })),
  ],
};

module.exports = {
  d, runs, P, H1, H2, H3, UL, OL, table, screen, caption, pageBreak, styles, numbering,
  BODY_W, ARIAL, TIMES, COURIER, figures, img, ornament, decorate, RED, GOLD, BLUE,
};
