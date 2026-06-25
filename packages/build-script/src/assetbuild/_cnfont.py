from PIL import Image, ImageChops, ImageDraw, ImageFont
from pathlib import Path
from dataclasses import dataclass

from src.util import _fmt, _common

DEFAULT_CHARS=" !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ —‘’“”•…、。"
PUNCTS = "！（），．：；？"
CHAR_PER_ROW = 40
TRANSPARENT = (0,0,0,0)
Rgb = tuple[int, int, int]
Rgba = tuple[int, int, int, int]
Xy = tuple[int, int]

def main()->int:
    print(f"==> creating fonts")
    packages = _common.get_packages_root()
    allstring = "".join([
        _read_txt(packages / "pvz-assets" / "main11zh" / "properties" / "LawnStrings.txt"),
        _read_txt(packages / "pvz-assets" / "main11zh" / "properties" / "ZombatarTOS.txt"),
        _read_txt(packages / "lawn-assets" / "mod" / "mainzh" / "properties" / "ExtraLawnStrings.txt"),
        _read_txt(packages / "lawn-assets" / "mod" / "mainzh" / "properties" / "ModStrings.txt"),
    ])

    font_chars = _create_chars(allstring)
    all_chars = DEFAULT_CHARS + font_chars

    WHITE = _rgb("#ffffff")
    BLACK = _rgba("#000000ff")

    status = _make_font(
        name="BrianneTod12_zh",
        orig_name="BrianneTod12",
        orig_png="_BrianneTod12.png",
        font_chars=font_chars,
        all_chars=all_chars,
        font_ttf="FZKaTong-M19S",
        font_pt=13,
        box_offset=(5,4),
        color=WHITE,
        background=BLACK,
        supersampling_scale=4,
        gamma=2.1,
        alpha_clamp=50
    )
    if status != 0: return status

    return 0


def _create_chars(chars: str) -> str:
    charset = set(chars)
    charset -= set(DEFAULT_CHARS)
    charset -= set(PUNCTS)
    charset -= set([' ', '\n', '\t', '\r'])
    s = ""
    for c in sorted(charset):
        s += c
    return s + PUNCTS

def _make_font(
    *,
    name: str,
    orig_name: str,
    orig_png: str,
    font_chars: str,
    all_chars: str,
    font_ttf: str,
    font_pt: int,
    box_offset: Xy,
    color: Rgb,
    background: Rgba,
    supersampling_scale: int=1,
    inset_light: Rgba=TRANSPARENT,
    inset_light_offsets: list[Xy]=[],
    inset_dark: Rgba=TRANSPARENT,
    inset_dark_offsets: list[Xy]=[],
    gamma: float,
    alpha_clamp: int=0,
) -> int:
    packages = _common.get_packages_root()
    data_dir = packages / "pvz-assets" / "main11zh" / "data"
    desc_path = data_dir / (orig_name + ".txt")
    atlas_path = data_dir / orig_png
    target_data = _common.get_root_root() / "target" / "assets" / "shared" / "data"
    target_desc_path = target_data / (name + ".txt")
    target_atlas_path = target_data / (name + ".png")

    desc = _parse_desc(desc_path)
    target_desc_path.write_bytes(_create_desc(desc, all_chars).encode("utf-8"))

    return _create_atlas_png(
        target_png=target_atlas_path,
        orig_png=atlas_path,
        font_ttf=font_ttf,
        font_pt=font_pt,
        box_size=desc.box_size,
        box_offset=box_offset,
        color=color,
        background=background,
        supersampling_scale=supersampling_scale,
        chars=font_chars,
        inset_light=inset_light,
        inset_light_offsets=inset_light_offsets,
        inset_dark=inset_dark,
        inset_dark_offsets=inset_dark_offsets,
        gamma=gamma,
        alpha_clamp=alpha_clamp,
    )


def _create_atlas_png(
    *,
    target_png: Path,
    orig_png: Path,
    font_ttf: str,
    font_pt: int,
    box_size: int,
    box_offset: Xy,
    color: Rgb,
    background: Rgba,
    supersampling_scale: int,
    chars: str,
    inset_light: Rgba,
    inset_light_offsets: list[Xy],
    inset_dark: Rgba,
    inset_dark_offsets: list[Xy],
    gamma: float,
    alpha_clamp: int,
) -> int:
    assets_path = _common.get_packages_root() / "lawn-assets"
    ttf_path = assets_path / "fonts" / f"{font_ttf}.ttf"
    if not ttf_path.exists():
        print(f"{_fmt.RED}>>> cannot find font {font_ttf}.ttf {_fmt.RESET}")
        return 1
    if not orig_png.exists():
        print(f"{_fmt.RED}>>> cannot find original font atlas {orig_png} {_fmt.RESET}")
        return 1

    "Create the New ATLAS"
    # - don't need to draw the "english" chars
    START_ROW= 2 # 0,1,2 actually 3rd row
    START_COL= 24 # 0-39 col index

    # - compute draw constants
    box_size_scaled = box_size * supersampling_scale
    font = ImageFont.truetype(str(ttf_path), font_pt * supersampling_scale)
    start_index = START_ROW* CHAR_PER_ROW + START_COL
    last_index = start_index + len(chars) - 1
    rows = last_index // CHAR_PER_ROW + 1
    image_width_scaled = CHAR_PER_ROW * box_size_scaled
    image_height_scaled = rows * box_size_scaled
    offx_scaled = box_offset[0] * supersampling_scale
    offy_scaled = box_offset[1] * supersampling_scale

    # create base glyph mask image
    base = Image.new("L", (image_width_scaled, image_height_scaled), 0)
    base_draw = ImageDraw.Draw(base)
    for i, ch in enumerate(chars):
        cell = start_index + i
        col = cell % CHAR_PER_ROW
        row = cell // CHAR_PER_ROW
        x = col * box_size_scaled + offx_scaled
        y = row * box_size_scaled + offy_scaled
        base_draw.text((x, y), ch, font=font, fill=255)

    # draw the font onto a transparent canvas; the real background (if any) is
    # composited under the finished glyphs at the very end so the alpha clamp
    # operates on the glyph coverage alone, not on a baked-in background.
    big = Image.new("RGBA", (image_width_scaled, image_height_scaled), TRANSPARENT)
    if len(inset_light_offsets):
        inset = _merge_insets(base, inset_light_offsets, supersampling_scale)
        big = _draw_colorized(big, inset, inset_light)
    if len(inset_dark_offsets):
        inset = _merge_insets(base, inset_dark_offsets, supersampling_scale)
        big = _draw_colorized(big, inset, inset_dark)
    big = _draw_colorized(big, base, color)

    # downscale with gamma
    bands = len(big.getbands())
    to_linear = [round((i / 255) ** gamma * 255) for i in range(256)] * bands
    to_srgb = [round((i / 255) ** (1 / gamma) * 255) for i in range(256)] * bands
    if supersampling_scale == 1:
        # still round trip for the math to clamp away some artifacts
        out = big.point(to_linear).point(to_srgb)
    else:
        out = (
            big.point(to_linear)
            .resize((CHAR_PER_ROW * box_size, rows * box_size), Image.Resampling.LANCZOS)
            .point(to_srgb)
        )

    # explicitly clamp away faint edge coverage: any alpha <= alpha_clamp -> 0,
    # everything above is left untouched. (alpha_clamp=0 is a no-op.)
    if alpha_clamp:
        r, g, b, a = out.split()
        a = a.point([0] * (alpha_clamp + 1) + list(range(alpha_clamp + 1, 256)))
        out = Image.merge("RGBA", (r, g, b, a))

    # composite onto the background, if any. drawing on transparent first means
    # the clamp above ran on glyph coverage only, not on a baked-in background.
    if background[3] != 0:
        bg = Image.new("RGBA", out.size, background)
        out = Image.alpha_composite(bg, out)

    # steal the original pixels for DEFAULT_CHARS (every cell before start_index),
    # which are the english/punctuation glyphs we don't re-render because they have variable positions
    orig = Image.open(orig_png).convert("RGBA")
    top = START_ROW * box_size
    left = START_COL * box_size
    out.paste(orig.crop((0, 0, CHAR_PER_ROW * box_size, top)), (0, 0))
    out.paste(orig.crop((0, top, left, top + box_size)), (0, top))

    out.save(target_png)
    return 0

def _draw_colorized(canvas, mask, color):
    rgb = color[:3]
    a = color[3] if len(color) == 4 else 255
    alpha = mask if a == 255 else mask.point(lambda v: v * a // 255)
    layer = Image.new("RGBA", canvas.size, rgb + (0,))
    layer.putalpha(alpha)
    return Image.alpha_composite(canvas, layer)

def _merge_insets(base, offsets, scale):
    merged = None
    for ox, oy in offsets:
        shifted = Image.new("L", base.size, 0)
        shifted.paste(base, (ox * scale, oy * scale))
        merged = shifted if merged is None else ImageChops.lighter(merged, shifted)
    return merged 

def _rgb(hex: str) -> Rgb:
    hex = hex.strip('#')
    if len(hex) != 6:
        raise Exception("invalid hex rgb: " + hex)
    r = int(hex[0:2], 16)
    g = int(hex[2:4], 16)
    b = int(hex[4:6], 16)
    return (r, g, b)
def _rgba(hex: str) -> Rgba:
    hex = hex.strip('#')
    if len(hex) != 8:
        raise Exception("invalid hex rgba: " + hex)
    r = int(hex[0:2], 16)
    g = int(hex[2:4], 16)
    b = int(hex[4:6], 16)
    a = int(hex[6:8], 16)
    return (r, g, b, a)


@dataclass
class MiniDesc:
    widthlist: list[int]
    box_size: int
    offsetlist: list[tuple[int,int]]
    commands: list[str]

def _create_desc(desc: MiniDesc, chars: str) -> str:
    if len(chars) < 105:
        raise Exception("chars too short, probably a bug")
    if chars[0] != ' ':
        raise Exception("chars must start with space, probably a bug")
    if len(desc.widthlist) < 106:
        raise Exception("desc.widthlist too short, probably a bug")
    lines = []
    lines.append(f"Define CharList0")
    charlist = []
    for c in chars:
        if c == "\"" or c == "\'" or c == "\\":
            c = "\\" + c
        charlist.append(f"'{c}'")
    lines += _create_desc_rows(charlist)
    lines.append("")
    lines.append(f"Define WidthList0")
    widthlist = []
    max_chars = min(len(chars), len(desc.widthlist))
    for i in range(max_chars):
        if i < len(desc.widthlist):
            widthlist.append(f"{desc.widthlist[i]:2}")
        else:
            widthlist.append(widthlist[-1])
    lines += _create_desc_rows(widthlist)
    lines.append("")
    lines.append(f"Define RectList0")
    rectlist_n = [(0,0,0)]
    box_size = desc.box_size
    max_coord = 0
    for i in range(max_chars-1): #first is 0,0,0,0
        x = (i%CHAR_PER_ROW) * box_size
        y = (i//CHAR_PER_ROW) * box_size
        rectlist_n.append((x,y,box_size))
        max_coord = max(max_coord, x)
        max_coord = max(max_coord, y)
    digits = len(str(max_coord))
    rectlist = []
    for x,y,s in rectlist_n:
        x = f"{x:{digits}}"
        y = f"{y:{digits}}"
        s = f"{s:2}"
        rectlist.append(f"({x}, {y}, {s}, {s})")
    lines += _create_desc_rows(rectlist)
    lines.append("")
    lines.append(f"Define OffsetList0")
    offsetlist_n: list[tuple[int,int]] = []
    max_chars = min(len(chars), len(desc.offsetlist))
    for i in range(max_chars):
        if i < len(desc.offsetlist):
            offsetlist_n.append(desc.offsetlist[i])
        else:
            offsetlist_n.append(offsetlist_n[-1])
    offsetlist = []
    for x,y in offsetlist_n:
        offsetlist.append(f"({x}, {y})")
    lines += _create_desc_rows(offsetlist)
    lines.append("")
    lines += desc.commands
    return "\n".join(lines)+'\n'


def _create_desc_rows(item_strs: list[str]) -> list[str]:
    rows = []
    curr_row = []
    for item in item_strs:
        curr_row.append(item)
        if len(curr_row) == CHAR_PER_ROW:
            rows.append(", ".join(curr_row))
            curr_row = []
    if len(curr_row):
        rows.append(", ".join(curr_row))
        curr_row = []
    row_strs = []
    for i, cl in enumerate(rows):
        prefix = " ( " if i == 0 else "   "
        suffix = " );" if i == len(rows) - 1 else ""
        row_strs.append(prefix + cl + suffix)
    return row_strs


def _parse_desc(path: Path) -> MiniDesc:
    txt = ""
    for line in _read_txt(path).splitlines():
        line = line.strip()
        if not line:
            continue
        txt += line
        if not line.endswith(";"):
            txt += ","
    WIDTHLIST_KEY = "Define WidthList0,"
    widthlist = None
    OFFSETLIST_KEY = "Define OffsetList0,"
    offsetlist = None
    RECTLIST_KEY = "Define RectList0,"
    box_size = None
    CHARLIST_KEY = "Define CharList0,"
    commands = []
    txt = txt.replace("';'", "<<<QUOTE_SEMI>>>").replace(";<=>?@", "<<<SEMI_SPACESHIP_QUESTION_AT>>>")
    for command in txt.split(";"):
        command = (
            command.strip()
            .replace("<<<QUOET_SEMI>>>", "';'")
            .replace("<<<SEMI_SPACESHIP_QUESTION_AT>>>", ";<=>?@")
       )
        if not command:
            continue
        if command.startswith("Define ExInfo"):
            continue
        if command.startswith(WIDTHLIST_KEY):
            widthlist = command[len(WIDTHLIST_KEY):].strip()
            if not (widthlist.startswith('(') and widthlist.endswith(')')):
                raise Exception("unexpected widthlist0 format")
            widthlist = [int(x.strip()) for x in widthlist[1:-1].split(',')]
            continue
        if command.startswith(OFFSETLIST_KEY):
            offsetlist = command[len(OFFSETLIST_KEY):].strip()
            if not (offsetlist.startswith('(') and offsetlist.endswith(')')):
                raise Exception("unexpected offsetlist0 format")
            tmp = []
            for x in offsetlist[1:-1].split('),'):
                x = x.strip()
                while x.startswith("("):
                    x = x[1:]
                while x.endswith(")"):
                    x = x[:-1]
                x = x.split(',')
                if len(x) != 2:
                    raise Exception("unexpected offsetlist0 format, should be pairs")
                tmp.append((int(x[0].strip()), int(x[1].strip())))
            offsetlist=tmp
            continue
        if command.startswith(RECTLIST_KEY):
            rectlist = command[len(RECTLIST_KEY):].strip()
            if not (rectlist.startswith('(') and rectlist.endswith(')')):
                raise Exception("unexpected rectlist0 format")
            tmp = []
            for x in rectlist[1:-1].split('),'):
                x = x.strip()
                while x.startswith("("):
                    x = x[1:]
                while x.endswith(")"):
                    x = x[:-1]
                x = x.split(',')
                if len(x) != 4:
                    raise Exception("unexpected rectlist format, should be pairs")
                size1 = int(x[2].strip())
                size2 = int(x[3].strip())
                if size1 != 0 and size1 == size2:
                    box_size = size1
                    break
            offsetlist=tmp
            continue
        if command.startswith(CHARLIST_KEY):
            continue
        commands.append(command+';')
    if not widthlist:
        raise Exception(f"did not find WidthList0 in {path}")
    if not offsetlist:
        raise Exception(f"did not find OffsetList0 in {path}")
    if not box_size:
        raise Exception(f"did not find box_size (from RectList0) in {path}")
    return MiniDesc(
        widthlist=widthlist,
        offsetlist=offsetlist,
        commands=commands,
        box_size=box_size
    )


def _read_txt(path: Path) -> str:
    data = path.read_bytes()
    if data[:2] in (b"\xff\xfe", b"\xfe\xff"):
        return data.decode("utf-16")
    if data[:3] == b"\xef\xbb\xbf":
        return data.decode("utf-8-sig")
    return data.decode("utf-8")
def _test2():
    assets_path = _common.get_packages_root() / "lawn-assets"
    font_path = assets_path / "fonts" / "FZJianZhi-M23S.ttf"
    # font_path = assets_path / "fonts" / "FZKaTong-M19S.ttf"

    START_ROW_FOR_TEST = 2 # 0,1,2 actually 3rd row
    START_COL_FOR_TEST = 24 # 0-39 col index
    # TEST_BOX_SIZE = 24
    # TEST_BOX_SIZE = 61
    TEST_BOX_SIZE = 27
    # FONT_SIZE_PT = 13 BRIANNETOD12
    # FONT_SIZE_PT = 35 DTC36BGI
    FONT_SIZE_PT = 15

    box = TEST_BOX_SIZE
    FILL_COLOR = (0, 230, 0) # BGI
    FILL_COLOR = (215, 160, 45) # YELLOW
    # FILL_COLOR = (0, 230, 0) # BGI
    BG_COLOR = (0, 0, 0, 0)  # transparent; use (0, 0, 0, 255) for black

    # Optional engraved/inset look: a translucent gray copy nudged to the
    # bottom-right and a translucent black copy nudged to the top-left, both
    # drawn behind the fill so only thin edge slivers show through.
    INSET = True
    # Each colour is the union (merged shape) of its offset copies, coloured
    # once -- so copies sharing a colour don't double-blend where they overlap.
    # Offsets are (dx, dy) in final px (+x right, +y down), before supersampling.
    INSET_LIGHT = (180, 180, 180, 160)
    # INSET_LIGHT_OFFSETS = [(1, 1), (1, 0)]  # bottom-right + right
    INSET_LIGHT_OFFSETS = []  # bottom-right + right
    # INSET_DARK = (20, 20, 20, 160)
    INSET_DARK = (20, 20, 20, 90)
    # INSET_DARK_OFFSETS = [(-1, -2), (-1, -1), (-1, 0)]  # top-left + left
    INSET_DARK_OFFSETS = [(-1, -1)]  # top-left + left

    # The font ships 1-bit embedded bitmaps that FreeType renders without
    # anti-aliasing at small pixel sizes. Rasterize the outline supersampled,
    # then shrink with LANCZOS so the final 24x24 cells are anti-aliased.
    # SCALE = 4
    # SCALE = 1 36BGI
    SCALE = 1
    sbox = box * SCALE
    font = ImageFont.truetype(str(font_path), FONT_SIZE_PT * SCALE)

    start_index = START_ROW_FOR_TEST * CHAR_PER_ROW + START_COL_FOR_TEST
    last_index = start_index + len("TEST_TEXT") - 1
    rows = last_index // CHAR_PER_ROW + 1

    W = CHAR_PER_ROW * sbox
    H = rows * sbox

    # Render the glyph coverage once as a white-on-black mask. Coloured layers
    # are derived by shifting/merging this base mask, so same-colour copies
    # union into one shape instead of alpha-blending where they overlap.
    base = Image.new("L", (W, H), 0)
    mdraw = ImageDraw.Draw(base)

    offset_x = 6 * SCALE
    offset_y = 4 * SCALE
    # offset_x = 12 * SCALE
    # offset_y = 9 * SCALE

    for i, ch in enumerate("TEST_TEXT"):
        cell = start_index + i
        col = cell % CHAR_PER_ROW
        row = cell // CHAR_PER_ROW
        x = col * sbox + offset_x
        y = row * sbox + offset_y
        mdraw.text((x, y), ch, font=font, fill=255)

    def shifted(mask, dx, dy):
        s = Image.new("L", mask.size, 0)
        s.paste(mask, (dx, dy))
        return s

    def merged(offsets):
        u = None
        for ox, oy in offsets:
            s = shifted(base, ox * SCALE, oy * SCALE)
            u = s if u is None else ImageChops.lighter(u, s)
        return u

    def over(canvas, mask, color):
        rgb = color[:3]
        a = color[3] if len(color) == 4 else 255
        alpha = mask if a == 255 else mask.point(lambda v: v * a // 255)
        layer = Image.new("RGBA", canvas.size, rgb + (0,))
        layer.putalpha(alpha)
        return Image.alpha_composite(canvas, layer)

    big = Image.new("RGBA", (W, H), BG_COLOR)
    if INSET:
        if len(INSET_LIGHT_OFFSETS):
            big = over(big, merged(INSET_LIGHT_OFFSETS), INSET_LIGHT)
        if len(INSET_DARK_OFFSETS):
            big = over(big, merged(INSET_DARK_OFFSETS), INSET_DARK)
    big = over(big, base, FILL_COLOR)

    # Downscale in linear light, not sRGB, so anti-aliased edges aren't
    # darkened. Decode sRGB -> linear, shrink, then re-encode to sRGB.
    # GAMMA = 2.1 whight, BGI
    GAMMA = 1.1
    bands = len(big.getbands())
    to_linear = [round((i / 255) ** GAMMA * 255) for i in range(256)] * bands
    to_srgb = [round((i / 255) ** (1 / GAMMA) * 255) for i in range(256)] * bands
    if SCALE == 1:
            out = big.point(to_linear).point(to_srgb)
    else:
        out = (
            big.point(to_linear)
            .resize((CHAR_PER_ROW * box, rows * box), Image.Resampling.LANCZOS)
            .point(to_srgb)
        )
    out_path = assets_path / "fonts" / "cnfont_test3.png"
    out.save(out_path)
    print(f">>> wrote {out_path} ({out.width}x{out.height}, {rows} rows)")
