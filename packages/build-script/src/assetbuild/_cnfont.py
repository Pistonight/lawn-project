from PIL import Image, ImageDraw, ImageFont
from pathlib import Path
from dataclasses import dataclass, field
from concurrent.futures import ThreadPoolExecutor
from typing import Literal

import numpy as np

from src.util import _fmt, _common

TEST_NAME = ""
TEST_LAYER = "Outline"

DEFAULT_CHARS=" !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ —‘’“”•…、。"
PUNCTS = "！（），．：；？"
CHAR_PER_ROW = 40
TRANSPARENT = (0,0,0,0)
Rgb = tuple[int, int, int]
Rgba = tuple[int, int, int, int]
Xy = tuple[int, int]
Xyf = tuple[float, float]

LOADED_FONT_CHARS = ""
LOADED_ALL_CHARS = ""

def main()->int:
    print(f"==> creating fonts")

    # load the shared char globals once, up front, so the worker threads below
    # only ever read them (no locking needed). test runs use per-font chars.
    if TEST_NAME:
        _exec_make_fonts(None)
        print(f"{_fmt.CLEAR_LINE}{_fmt.YELLOW}>>> exiting...")
        return 8
    else:
        global LOADED_FONT_CHARS
        global LOADED_ALL_CHARS
        packages = _common.get_packages_root()
        allstring = "".join([
            _read_txt(packages / "pvz-assets" / "main11zh" / "properties" / "LawnStrings.txt"),
            _read_txt(packages / "pvz-assets" / "main11zh" / "properties" / "ZombatarTOS.txt"),
            _read_txt(packages / "lawn-assets" / "mod" / "mainzh" / "properties" / "ExtraLawnStrings.txt"),
            _read_txt(packages / "lawn-assets" / "mod" / "mainzh" / "properties" / "ModStrings.txt"),
        ])
        x = _create_chars(allstring)
        LOADED_FONT_CHARS = x[0]
        LOADED_ALL_CHARS = x[1]

        # render each font on its own thread, then join them all by reading results
        with ThreadPoolExecutor() as pool:
            futures = _exec_make_fonts(pool)
            all_status = 0
            for f in futures:
                if not isinstance(f, tuple):
                    name, status = f.result()
                    if status != 0:
                        print(f"{_fmt.CLEAR_LINE}{_fmt.RED}>>> failed to render font {name}")
                        all_status = status
            print(f"{_fmt.CLEAR_LINE}",end="",flush=True)
            return all_status


def _exec_make_fonts(pool: ThreadPoolExecutor | None):
    DWARVEN_SHADE = (20,20,20,90)
    YELLOW=(215,160,45)
    LIGHT=(180,180,180,160)
    LIGHT2=(180,180,180,100)
    BRIGHT_GREEN=(0,230,0)
    GREEN=(0,200,0)
    DARK = (20,20,20,160)
    DARK2 = (0,0,0,200)
    BLACK = (0,0,0,255)
    INSET_DARK=[(-1,-2),(-1,-1),(-1,0)]
    INSET_LIGHT=[(1,1), (1,0)]
    return [
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZKaTong-M19S", font_pt=13,
                box_offset=(5,4), supersampling=4
            ),
            name="BrianneTod12ZH", orig_name="BrianneTod12", orig_png="_BrianneTod12.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZKaTong-M19S", font_pt=15.5,
                box_offset=(6,4), strength=3, supersampling=4
            ),
            name="BrianneTod16ZH", orig_name="BrianneTod16", orig_png="_BrianneTod16.png"
        ),
        _exec_make_font_multilayer(
            pool,
            layers={
                'Main': MultiLayerRenderArgs(
                    args= RenderArgs(
                        font_ttf="FZKaTong-M19S", font_pt=27,
                        box_offset=(10,8), bold_offsets=[(1,0)], strength=3
                    ),
                    orig_png="_BrianneTod32.png",
                    output_stem="BrianneTod32ZH"
                )
            },
            desc_name="BrianneTod32BlackZH", orig_desc_name="BrianneTod32Black", 
        ),
        _exec_make_font_multilayer(
            pool,
            layers={
                'Main': MultiLayerRenderArgs(
                    args="REUSE_IMAGE", orig_png="", output_stem="BrianneTod32ZH"
                ),
                'Outline': MultiLayerRenderArgs(
                    args=RenderArgs(
                        font_ttf="FZKaTong-M19S", font_pt=27,
                        box_offset=(10,8), bold_offsets=[(1,0)], strength=2, stroke=2
                    ),
                    orig_png="_BrianneTod32Outline.png", output_stem="BrianneTod32OutlineZH"
                ),
            },
            desc_name="BrianneTod32ZH", orig_desc_name="BrianneTod32",
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=10.5,
                box_offset=(4,3), supersampling=4, gamma=1
            ),
            name="DwarvenTodcraft12ZH", orig_name="DwarvenTodcraft12", orig_png="DwarvenTodcraft12.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=14.2,
                box_offset=(4.5,3.5), supersampling=4, gamma=1,
                inset_dark=DWARVEN_SHADE, inset_dark_offsets=[(-1,-1)]
            ),
            name="DwarvenTodcraft15ZH", orig_name="DwarvenTodcraft15", orig_png="DwarvenTodcraft15.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=15,
                box_offset=(6,4), gamma=1,
                inset_dark=BLACK, inset_dark_offsets=[(-1,-1), (1,1), (-1,1), (1,-1), (-2, 0), (2, 0), (0, -2), (0,2)]
            ),
            name="DwarvenTodcraft18ZH", orig_name="DwarvenTodcraft18", orig_png="DwarvenTodcraft18.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=15,
                box_offset=(6,4), gamma=1, color=YELLOW, alpha_clamp=20,
                inset_dark=DWARVEN_SHADE, inset_dark_offsets=INSET_DARK,
            ),
            name="DwarvenTodcraft18YellowZH", orig_name="DwarvenTodcraft18Yellow", orig_png="DwarvenTodcraft18Yellow.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=15.2,
                box_offset=(6,4), supersampling=2, gamma=1, color=BRIGHT_GREEN, alpha_clamp=20,
                inset_dark=DARK2, inset_dark_offsets=INSET_DARK,
                inset_light=LIGHT2, inset_light_offsets=INSET_LIGHT
            ),
            name="DwarvenTodcraft18BrightGreenInsetZH", orig_name="DwarvenTodcraft18BrightGreenInset", orig_png="DwarvenTodcraft18BrightGreenInset.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=15.2,
                box_offset=(6,4), supersampling=2, gamma=1, color=GREEN, alpha_clamp=20,
                inset_dark=DARK2, inset_dark_offsets=INSET_DARK,
                inset_light=LIGHT2, inset_light_offsets=INSET_LIGHT
            ),
            name="DwarvenTodcraft18GreenInsetZH", orig_name="DwarvenTodcraft18GreenInset", orig_png="DwarvenTodcraft18GreenInset.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=24,
                box_offset=(9,6), gamma=1,
                inset_dark=BLACK, inset_dark_offsets=[(-1,-1), (1,1), (-1,1), (1,-1), (-2, 0), (2, 0), (0, -2), (0,2)]
            ),
            name="DwarvenTodcraft24ZH", orig_name="DwarvenTodcraft24", orig_png="DwarvenTodcraft24.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=35,
                box_offset=(12,9), color=BRIGHT_GREEN,
                inset_dark=DARK, inset_dark_offsets=INSET_DARK,
                inset_light=LIGHT, inset_light_offsets=INSET_LIGHT,
            ),
            name="DwarvenTodcraft36BrightGreenInsetZH", orig_name="DwarvenTodcraft36BrightGreenInset", orig_png="DwarvenTodcraft36BrightGreenInset.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZJianZhi-M23S", font_pt=35,
                box_offset=(12,9),                 color=GREEN,
                inset_dark=DARK, inset_dark_offsets=INSET_DARK,
                inset_light=LIGHT, inset_light_offsets=INSET_LIGHT,
            ),
            name="DwarvenTodcraft36GreenInsetZH", orig_name="DwarvenTodcraft36GreenInset", orig_png="DwarvenTodcraft36GreenInset.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZCuQian-M17S", font_pt=15,
                box_offset=(6,4), supersampling=4, sand_alpha=100
            ),
            name="ContinuumBold14ZH", orig_name="ContinuumBold14", orig_png="_ContinuumBold14.png"
        ),
        _exec_make_font_multilayer(
            pool,
            layers={
                'Main': MultiLayerRenderArgs(
                    args= RenderArgs(
                        font_ttf="FZYiHei-M20S", font_pt=15,
                        box_offset=(6,4), supersampling=4, sand_alpha=100
                    ),
                    orig_png="_HouseofTerror16.png", output_stem="HouseofTerror16ZH",
                ),
                'Outline': MultiLayerRenderArgs(
                    args= RenderArgs(
                        font_ttf="FZYiHei-M20S", font_pt=15,
                        box_offset=(6,4), supersampling=4, color=BLACK[:3], stroke=8.5
                    ),
                    orig_png="HouseofTerror16Outline.png", output_stem="HouseofTerror16OutlineZH",
                ),
            },
            desc_name="HouseofTerror16ZH", orig_desc_name="HouseofTerror16"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZYiHei-M20S", font_pt=21,
                box_offset=(8,5), supersampling=4, sand_alpha=100
            ),
            name="HouseofTerror20ZH", orig_name="HouseofTerror20", orig_png="_HouseofTerror20.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZYiHei-M20S", font_pt=21,
                box_offset=(8,5), supersampling=4, color=BLACK[:3], stroke=8.5
            ),
            name="HouseofTerror20OutlineZH", orig_name="HouseofTerror20Outline", orig_png="HouseofTerror20Outline.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZYiHei-M20S", font_pt=26,
                box_offset=(9,6), sand_alpha=100,
                inset_light=BLACK, inset_light_offsets=[(-1,0),(0,-1),(1,0),(0,1),(2,0),(0,2),(2,1),(1,2),(3,2),(2,3),(3,3)]
            ),
            name="HouseofTerror28ZH", orig_name="HouseofTerror28", orig_png="HouseofTerror28.png"
        ),



        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZCuQian-M17S", font_pt=15,
                box_offset=(6,4), supersampling=4, sand_alpha=100
            ),
            name="ContinuumBold14ZH", orig_name="ContinuumBold14", orig_png="_ContinuumBold14.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="FZCuQian-M17S", font_pt=15,
                box_offset=(6,4), supersampling=4, sand_alpha=100, stroke=6
            ),
            name="ContinuumBold14OutlineZH", orig_name="ContinuumBold14outback", orig_png="_ContinuumBold14outback.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="DouyinSansBold", font_pt=12,
                box_offset=(6,4), supersampling=4, gamma=1.9, sand_alpha=100
            ),
            name="Pix118BoldZH", orig_name="Pix118Bold", orig_png="_Pix118Bold.png"
        ),
        _exec_make_font(
            pool,
            RenderArgs(
                font_ttf="DouyinSansBold", font_pt=12,
                box_offset=(6,4), supersampling=4, gamma=1.9, sand_alpha=100
            ),
            name="Pico129ZH", orig_name="Pico129", orig_png="_Pico129.png"
        ),
    ]

def _exec_make_font(
    pool: ThreadPoolExecutor | None,
    args: RenderArgs,
    *,
    name: str,
    orig_name: str,
    orig_png: str,
):
    if not pool:
        return _make_font(args, name=name, orig_name=orig_name, orig_png=orig_png)
    return pool.submit(_make_font, args, name=name, orig_name=orig_name, orig_png=orig_png)

def _exec_make_font_multilayer(
    pool: ThreadPoolExecutor | None,
    *,
    layers: dict[str, MultiLayerRenderArgs],
    desc_name: str,
    orig_desc_name: str,
):
    if not pool:
        return _make_font_multilayer(layers=layers, desc_name=desc_name, orig_desc_name=orig_desc_name)
    return pool.submit(_make_font_multilayer, layers=layers, desc_name=desc_name, orig_desc_name=orig_desc_name)

def _create_chars(chars: str) -> tuple[str, str]:
    charset = set(chars)
    charset -= set(DEFAULT_CHARS)
    charset -= set(PUNCTS)
    charset -= set([' ', '\n', '\t', '\r'])
    s = ""
    for c in sorted(charset):
        s += c
    font_chars = s + PUNCTS
    return font_chars, DEFAULT_CHARS + font_chars

def _make_font(
    args: RenderArgs,
    *,
    name: str,
    orig_name: str,
    orig_png: str,
) -> tuple[str, int]:
    return _make_font_multilayer(
        layers={
            'Main': MultiLayerRenderArgs(
                args=args,
                orig_png=orig_png,
                output_stem=name,
            )
        },
        desc_name=name,
        orig_desc_name=orig_name
    )

def _make_font_multilayer(
    *,
    layers: dict[str, MultiLayerRenderArgs],
    desc_name: str,
    orig_desc_name: str,
) -> tuple[str,int]:
    is_test = bool(TEST_NAME and TEST_NAME == orig_desc_name)
    if is_test:
        print(f"{_fmt.YELLOW}>>> testing: {TEST_NAME}{_fmt.RESET}")
    if TEST_NAME and not is_test:
        return desc_name, 0
    packages = _common.get_packages_root()
    data_dir = packages / "pvz-assets" / "main11zh" / "data"
    desc_path = data_dir / (orig_desc_name + ".txt")
    target_data = _common.get_root_root() / "target" / "assets" / "shared" / "data"
    target_desc_path = target_data / (desc_name + ".txt")

    desc = _parse_desc(desc_path)
    if is_test:
        font_chars, all_chars = _create_chars(desc.raw)
    else:
        font_chars = LOADED_FONT_CHARS
        all_chars = LOADED_ALL_CHARS

    layer_to_image = {}
    for layer, args in layers.items():
        orig_atlas_path = data_dir / args.orig_png
        # if the image has _ prefix then it's saved without alpha and alpha is calculated at runtime automatically
        alpha = not args.orig_png.startswith("_")
        alpha_prefix = "" if alpha else "_"
        target_atlas_path = target_data / (alpha_prefix + args.output_stem + ".png")
        if not isinstance(args.args, str):
            status = _create_atlas_png(
                args.args,
                target_png=target_atlas_path,
                orig_png=orig_atlas_path,
                box_size=desc.box_size,
                alpha=alpha,
                chars=font_chars,
                test=is_test and layer == TEST_LAYER
            )
            if status != 0: return desc_name, status
            print(f". {_fmt.CLEAR_LINE}Render: {desc_name} (Layer={layer})" , end="", flush=True)
        layer_to_image[layer] = args.output_stem
    serialized_desc, status = _create_desc(desc, layer_to_image, all_chars)
    if status != 0: return desc_name, status

    target_desc_path.write_bytes(serialized_desc.encode("utf-8"))
    return desc_name, 0


@dataclass
class MultiLayerRenderArgs:
    args: RenderArgs | Literal["REUSE_IMAGE"]
    orig_png: str
    output_stem: str


def _create_atlas_png(
    args: RenderArgs,
    *,
    target_png: Path,
    orig_png: Path,
    box_size: int,
    alpha: bool,
    chars: str,
    test: bool
) -> int:
    font_ttf = args.font_ttf
    font_pt = args.font_pt
    box_offset = args.box_offset
    color = args.color
    supersampling = args.supersampling
    inset_light = args.inset_light
    inset_light_offsets = args.inset_light_offsets
    inset_dark = args.inset_dark
    inset_dark_offsets = args.inset_dark_offsets
    bold_offsets = args.bold_offsets
    strength = args.strength
    stroke = args.stroke
    gamma = args.gamma
    alpha_clamp = args.alpha_clamp
    sand_alpha = args.sand_alpha

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
    box_size_scaled = box_size * supersampling
    font = ImageFont.truetype(str(ttf_path), font_pt * supersampling)
    start_index = START_ROW* CHAR_PER_ROW + START_COL
    last_index = start_index + len(chars) - 1
    rows = last_index // CHAR_PER_ROW + 1
    image_width_scaled = CHAR_PER_ROW * box_size_scaled
    image_height_scaled = rows * box_size_scaled
    offx_scaled = box_offset[0] * supersampling
    offy_scaled = box_offset[1] * supersampling

    # create base glyph mask image
    base = Image.new("L", (image_width_scaled, image_height_scaled), 0)
    base_draw = ImageDraw.Draw(base)
    for i, ch in enumerate(chars):
        cell = start_index + i
        col = cell % CHAR_PER_ROW
        row = cell // CHAR_PER_ROW
        x = col * box_size_scaled + offx_scaled
        y = row * box_size_scaled + offy_scaled
        base_draw.text((x, y), ch, font=font, fill=255, stroke_width=stroke, stroke_fill=255)

    # convert image to numpy to manipulate
    # compose the masks according to inset/bold/strength
    base_np = np.asarray(base, dtype=np.float32) / 255.0  # (H, W) coverage
    dst = np.zeros((image_height_scaled, image_width_scaled, 4), dtype=np.float32)  # straight-alpha RGBA
    def over(mask: np.ndarray, col: Rgb | Rgba) -> None:
        src_a = mask * ((col[3] if len(col) == 4 else 255) / 255.0)
        src_rgb = np.asarray(col[:3], dtype=np.float32) / 255.0
        dst_rgb, dst_a = dst[..., :3], dst[..., 3]

        out_a = src_a + dst_a * (1.0 - src_a)
        nz = out_a > 0
        new_rgb = src_rgb * src_a[..., None] + dst_rgb * (dst_a * (1.0 - src_a))[..., None]
        np.divide(new_rgb, out_a[..., None], out=dst_rgb, where=nz[..., None])
        dst_a[...] = out_a

    if len(inset_light_offsets):
        over(_merge_mask_np(base_np, inset_light_offsets, supersampling), inset_light)
    if len(inset_dark_offsets):
        over(_merge_mask_np(base_np, inset_dark_offsets, supersampling), inset_dark)
    if len(bold_offsets):
        over(_merge_mask_np(base_np, bold_offsets, supersampling), color)
    over(base_np, color)
    for _ in range(strength - 1):
        over(base_np, color)

    # Downscaling
    lin_u8 = np.clip(np.power(dst, gamma) * 255.0 + 0.5, 0, 255).astype(np.uint8)
    img = Image.fromarray(lin_u8, "RGBA")
    if supersampling != 1:
        img = img.resize(
            (CHAR_PER_ROW * box_size, rows * box_size), Image.Resampling.LANCZOS
        )
    arr = np.asarray(img, dtype=np.float32) / 255.0
    out = np.clip(np.power(arr, 1.0 / gamma) * 255.0 + 0.5, 0, 255).astype(np.uint8)

    # clamp faint edge coverage, then drop disconnected sand (both on alpha)
    out_alpha = out[..., 3]
    if alpha_clamp:
        out_alpha[out_alpha <= alpha_clamp] = 0
    _sand_array(out_alpha, sand_alpha)

    out = Image.fromarray(out, "RGBA")

    # composite onto the background, if any. drawing on transparent first means
    # the clamp above ran on glyph coverage only, not on a baked-in background.
    if not alpha:
        bg = Image.new("RGBA", out.size, (0,0,0,255))
        out = Image.alpha_composite(bg, out)

    # steal the original pixels for DEFAULT_CHARS (every cell before start_index),
    # which are the english/punctuation glyphs we don't re-render because they have variable positions
    orig = Image.open(orig_png).convert("RGBA")
    top = START_ROW * box_size
    left = START_COL * box_size
    out.paste(orig.crop((0, 0, CHAR_PER_ROW * box_size, top)), (0, 0))
    out.paste(orig.crop((0, top, left, top + box_size)), (0, top))

    # drop the (now fully-opaque) alpha channel so the png is saved as RGB
    if not alpha:
        out = out.convert("RGB")

    out.save(target_png)

    if test:
        from src.assetbuild import _fontcmp
        return _fontcmp.open_comparer(orig_png, target_png)

    return 0




def _merge_mask_np(cov: np.ndarray, offsets: list[Xy], scale: int) -> np.ndarray:
    h, w = cov.shape
    merged = np.zeros_like(cov)
    for ox, oy in offsets:
        dx, dy = ox * scale, oy * scale
        # destination region (where the shifted copy lands), clipped to bounds
        sx0, sy0 = max(0, dx), max(0, dy)
        sx1, sy1 = min(w, w + dx), min(h, h + dy)
        # matching source region
        cx0, cy0 = max(0, -dx), max(0, -dy)
        if sx1 > sx0 and sy1 > sy0:
            # max the shifted source straight into the merged view (no temp array);
            # pixels outside this region keep their accumulated value (max with 0).
            dst = merged[sy0:sy1, sx0:sx1]
            src = cov[cy0:cy0 + (sy1 - sy0), cx0:cx0 + (sx1 - sx0)]
            np.maximum(dst, src, out=dst)
    return merged


def _sand_array(a: np.ndarray, anchor_alpha: int) -> None:
    mask = a > 0
    marker = (a >= anchor_alpha) & mask
    while True:
        grown = marker.copy()
        grown[1:, :] |= marker[:-1, :]
        grown[:-1, :] |= marker[1:, :]
        grown[:, 1:] |= marker[:, :-1]
        grown[:, :-1] |= marker[:, 1:]
        grown &= mask
        if np.array_equal(grown, marker):
            break
        marker = grown
    a[~marker] = 0



@dataclass
class RenderArgs:
    font_ttf: str
    font_pt: float
    box_offset: Xyf
    gamma: float=2.1
    supersampling: int=1
    color: Rgb=(255,255,255)
    inset_light: Rgba=TRANSPARENT
    inset_light_offsets: list[Xy]= field(default_factory=list)
    inset_dark: Rgba=TRANSPARENT
    inset_dark_offsets: list[Xy]= field(default_factory=list)
    bold_offsets: list[Xy]= field(default_factory=list)
    strength: int=1
    stroke: float=0
    alpha_clamp:int=50
    sand_alpha:int=180


@dataclass
class MiniDesc:
    widthlist: list[int]
    box_size: int
    offsetlist: list[tuple[int,int]]
    defines: list[str]
    layer_to_commands: dict[str, list[str]]
    raw: str

def _create_desc(desc: MiniDesc, layer_to_image: dict[str, str], chars: str) -> tuple[str, int]:
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
    max_chars = len(chars)
    for i in range(max_chars):
        if i < len(desc.widthlist):
            widthlist.append(f"{desc.widthlist[i]:2}")
        else:
            widthlist.append(widthlist[-1])
    if len(charlist) != len(widthlist):
        print(f"{_fmt.CLEAR_LINE}{_fmt.RED}>>> error: charlist ({len(charlist)}) and widthlist ({len(widthlist)}) size mismatch!!!{_fmt.RESET}")
        return "", 1
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
    for d in desc.defines:
        lines.append("Define "+d)
        lines.append("")

    for layer in desc.layer_to_commands.keys():
        if layer not in layer_to_image:
            print(f"{_fmt.CLEAR_LINE}{_fmt.RED}>>> error: image for layer '{layer}' not found{_fmt.RESET}")
            return "", 1
        lines.append(f"CreateLayer               {layer};")
        lines.append(f"LayerSetImage             {layer} '{layer_to_image[layer]}';")
        lines += desc.layer_to_commands[layer]
    return "\n".join(lines)+'\n', 0


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
    raw = _read_txt(path)
    txt = ""
    for line in raw.splitlines():
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
    CREATELAYER_KEY = "CreateLayer";
    LAYERSETIMAGE_KEY = "LayerSetImage "; # there's another LayerSetImageMap so space is important
    DEFINE_KEY = "Define "
    current_layer_name = None
    layer_to_commands: dict[str, list[str]] = {}
    defines = []
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
            continue # we use our own chars
        if command.startswith(CREATELAYER_KEY):
            current_layer_name = command[len(CREATELAYER_KEY):].strip()
            continue
        if command.startswith(LAYERSETIMAGE_KEY):
            continue # we set a new image
        if command.startswith(DEFINE_KEY):
            # need to keep other defines
            command = command[len(DEFINE_KEY):].strip()
            defines.append(command.replace(",", "\n", 1) + ';')
        if current_layer_name:
            if current_layer_name not in layer_to_commands:
                layer_to_commands[current_layer_name] = [command + ';']
            else:
                layer_to_commands[current_layer_name].append(command+';')
    if not widthlist:
        raise Exception(f"did not find WidthList0 in {path}")
    if not offsetlist:
        raise Exception(f"did not find OffsetList0 in {path}")
    if not box_size:
        raise Exception(f"did not find box_size (from RectList0) in {path}")
    if len(layer_to_commands) == 0:
        raise Exception(f"did not find layer_name (from CreateLayer) in {path}")
    return MiniDesc(
        widthlist=widthlist,
        offsetlist=offsetlist,
        box_size=box_size,
        layer_to_commands=layer_to_commands,
        raw=raw,
        defines=defines
    )


def _read_txt(path: Path) -> str:
    data = path.read_bytes()
    if data[:2] in (b"\xff\xfe", b"\xfe\xff"):
        return data.decode("utf-16")
    if data[:3] == b"\xef\xbb\xbf":
        return data.decode("utf-8-sig")
    return data.decode("utf-8")
