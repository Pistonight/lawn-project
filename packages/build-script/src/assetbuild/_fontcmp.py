"""Compare two same-sized images, toggling between A and B with zoom + pan.

Usage:
    python compare_images.py IMAGE_A IMAGE_B

Controls:
    - Mouse wheel        : zoom in / out (centered on cursor)
    - Left-drag          : pan
    - Space / T / button : toggle between image A and B
    - R                  : reset view
"""

import signal
import tkinter as tk
from PIL import Image, ImageTk

def open_comparer(a, b) -> int:
    if a is None and b is None:
        print(">>> nothing to compare!")
        return 0
    if a is None:
        a = b
        b = None

    img_a = Image.open(a).convert("RGBA")
    if b:
        img_b = Image.open(b).convert("RGBA")
    else:
        img_b = Image.new("RGBA", (img_a.width, img_a.height), (0,0,0,0))

    if img_a.size != img_b.size:
        print(f"Image dimensions differ: "
                 f"A is {img_a.width}x{img_a.height}, "
                 f"B is {img_b.width}x{img_b.height}; "
                 f"padding both to a common size (top-left anchored)")
        w = max(img_a.width, img_b.width)
        h = max(img_a.height, img_b.height)
        canvas_a = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        canvas_a.paste(img_a, (0, 0))
        canvas_b = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        canvas_b.paste(img_b, (0, 0))
        img_a, img_b = canvas_a, canvas_b

    root = tk.Tk()
    root.title("Image Compare  (space/T = toggle, wheel = zoom, drag = pan)")
    root.geometry("900x700")
    Comparer(root, img_a, img_b, a, b)

    # Let Ctrl+C close the window immediately. Tk's mainloop sits in C and
    # won't process the signal until the next event, so keep the Python
    # interpreter ticking with a periodic no-op.
    signal.signal(signal.SIGINT, lambda *_: root.destroy())

    def _keep_ticking():
        root.after(100, _keep_ticking)

    _keep_ticking()
    root.mainloop()
    return 0


class Comparer:
    MIN_SCALE = 0.05
    MAX_SCALE = 64.0
    LABELS = {"A": "Theirs", "B": "Ours"}

    def __init__(self, root, img_a, img_b, path_a, path_b):
        self.root = root
        self.images = {"A": img_a, "B": img_b}
        self.paths = {"A": path_a, "B": path_b}
        self.current = "A"

        self.scale = 1.0
        # Top-left of the image in canvas coordinates.
        self.offset_x = 0.0
        self.offset_y = 0.0
        self._photo = None  # keep a reference so Tk doesn't GC it
        self._drag_start = None

        self.canvas = tk.Canvas(root, bg="#202020", highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        bar = tk.Frame(root)
        bar.pack(fill="x")
        self.toggle_btn = tk.Button(bar, text=f"Showing: {self.LABELS[self.current]}",
                                    width=14, command=self.toggle)
        self.toggle_btn.pack(side="left", padx=4, pady=4)
        tk.Button(bar, text="Reset view", command=self.reset_view).pack(
            side="left", padx=4, pady=4)
        self.status = tk.Label(bar, anchor="w")
        self.status.pack(side="left", fill="x", expand=True, padx=8)

        # Bindings.
        self.canvas.bind("<Configure>", lambda e: self.render())
        self.canvas.bind("<ButtonPress-1>", self.on_press)
        self.canvas.bind("<B1-Motion>", self.on_drag)
        # Windows / macOS wheel.
        self.canvas.bind("<MouseWheel>", self.on_wheel)
        # Linux wheel.
        self.canvas.bind("<Button-4>", self.on_wheel)
        self.canvas.bind("<Button-5>", self.on_wheel)
        root.bind("<space>", lambda e: self.toggle())
        root.bind("t", lambda e: self.toggle())
        root.bind("r", lambda e: self.reset_view())

        self.root.after(100, self.reset_view)

    # ----- view helpers ---------------------------------------------------
    def reset_view(self):
        img = self.images[self.current]
        cw = self.canvas.winfo_width() or img.width
        ch = self.canvas.winfo_height() or img.height
        self.scale = min(cw / img.width, ch / img.height, 1.0)
        self.offset_x = (cw - img.width * self.scale) / 2
        self.offset_y = (ch - img.height * self.scale) / 2
        self.render()

    def toggle(self):
        self.current = "B" if self.current == "A" else "A"
        self.toggle_btn.config(text=f"Showing: {self.LABELS[self.current]}")
        self.render()

    def on_press(self, event):
        self._drag_start = (event.x, event.y, self.offset_x, self.offset_y)

    def on_drag(self, event):
        if not self._drag_start:
            return
        sx, sy, ox, oy = self._drag_start
        self.offset_x = ox + (event.x - sx)
        self.offset_y = oy + (event.y - sy)
        self.render()

    def on_wheel(self, event):
        if getattr(event, "delta", 0) > 0 or getattr(event, "num", None) == 4:
            factor = 1.25
        else:
            factor = 0.8
        new_scale = max(self.MIN_SCALE, min(self.MAX_SCALE, self.scale * factor))
        if new_scale == self.scale:
            return
        # Keep the point under the cursor fixed.
        img_x = (event.x - self.offset_x) / self.scale
        img_y = (event.y - self.offset_y) / self.scale
        self.scale = new_scale
        self.offset_x = event.x - img_x * self.scale
        self.offset_y = event.y - img_y * self.scale
        self.render()

    # ----- rendering ------------------------------------------------------
    def render(self):
        img = self.images[self.current]
        cw = self.canvas.winfo_width()
        ch = self.canvas.winfo_height()
        if cw <= 1 or ch <= 1:
            return

        # Only render the visible region for speed at high zoom.
        vis_left = max(0, int(-self.offset_x / self.scale))
        vis_top = max(0, int(-self.offset_y / self.scale))
        vis_right = min(img.width, int((cw - self.offset_x) / self.scale) + 1)
        vis_bottom = min(img.height, int((ch - self.offset_y) / self.scale) + 1)

        self.canvas.delete("all")
        if vis_right <= vis_left or vis_bottom <= vis_top:
            self._update_status()
            return

        crop = img.crop((vis_left, vis_top, vis_right, vis_bottom))
        dw = max(1, int(round(crop.width * self.scale)))
        dh = max(1, int(round(crop.height * self.scale)))
        resample = Image.Resampling.NEAREST if self.scale >= 1 else Image.Resampling.LANCZOS
        disp = crop.resize((dw, dh), resample)

        # Flatten alpha onto the canvas background ourselves. ImageTk.PhotoImage
        # on an RGBA image uses Tk's per-pixel alpha path, which takes seconds for
        # large atlases; compositing to RGB here is instant and looks identical.
        if disp.mode == "RGBA":
            bg = Image.new("RGB", disp.size, (32, 32, 32))  # canvas bg #202020
            bg.paste(disp, mask=disp.getchannel("A"))
            disp = bg

        self._photo = ImageTk.PhotoImage(disp)
        px = self.offset_x + vis_left * self.scale
        py = self.offset_y + vis_top * self.scale
        self.canvas.create_image(px, py, anchor="nw", image=self._photo)
        self._update_status()

    def _update_status(self):
        img = self.images[self.current]
        self.status.config(
            text=f"{self.LABELS[self.current]}: {self.paths[self.current]}  "
                 f"({img.width}x{img.height})  zoom {self.scale:.2f}x")
