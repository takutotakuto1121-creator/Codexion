#!/usr/bin/env python3
"""Codexion visualizer.

Usage:
    ./codexion 4 800 200 200 200 5 10 fifo | ./visualizer.py [options]

Reads "<timestamp_ms> <id> <action>" lines from stdin and draws one row per
coder.  The bar is a timeline (left = t0, right = latest time); every state
has its own letter AND color, so it stays readable without color.

    .  waiting (holding 0-1 dongle, wants to compile)
    C  compiling      D  debugging      R  refactoring
    X  burned out (the coder is dead, timeline stops here)

The simulation ends when either
  * every coder has compiled at least <compiles> times  -> SUCCESS, or
  * any coder goes time_to_burn_out ms without starting a compile
    -> BURNED OUT (the whole simulation stops at that moment).
Pass --compiles and --burnout (the same values given to ./codexion) to see
per-coder progress and burnout countdowns.
"""

import argparse
import re
import shutil
import sys
import time

LINE_RE = re.compile(r"^\s*(\d+)\s+(\d+)\s+(.*?)\s*$")

ACTIONS = {
    "has taken a dongle": "wait",
    "is compiling": "compile",
    "is debugging": "debug",
    "is refactoring": "refactor",
    "burned out": "burnout",
}

# state: (label, ANSI color, timeline char, description)
STATES = {
    "init": ("not started", "90", " ", ""),
    "wait": ("waiting", "33", ".", "waiting for dongles"),
    "compile": ("COMPILING", "1;32", "C", "compiling (has 2 dongles)"),
    "debug": ("debugging", "36", "D", "debugging"),
    "refactor": ("refactoring", "34", "R", "refactoring"),
    "burnout": ("BURNED OUT", "1;41;97", "X", "burned out"),
}
LEGEND_ORDER = ("wait", "compile", "debug", "refactor", "burnout")

RESET = "\033[0m"
REFRESH_INTERVAL = 0.05


def paint(code, text):
    return "\033[%sm%s%s" % (code, text, RESET)


class Coder:
    def __init__(self, cid):
        self.id = cid
        self.state = "init"
        self.dongles = 0
        self.compiles = 0
        self.last_compile = None  # ts of last compile start
        self.dead_at = None
        self.done_at = None  # ts when the compile quota was reached
        self.events = []  # (timestamp, state)

    def apply(self, ts, state, required=0):
        if self.dead_at is not None:
            return
        if state == "wait":
            self.dongles += 1
        elif state == "compile":
            self.compiles += 1
            self.dongles = 0
            self.last_compile = ts
            if required and self.compiles >= required and \
                    self.done_at is None:
                self.done_at = ts
        elif state == "burnout":
            self.dead_at = ts
        self.state = state
        self.events.append((ts, state))

    def dominant(self, t0, t1):
        """State that covers most of [t0, t1); a one-tick event still wins
        over 'init' so short events are never lost."""
        best, best_len = "init", -1
        for i, (ts, st) in enumerate(self.events):
            end = self.events[i + 1][0] if i + 1 < len(self.events) else None
            if st == "burnout":
                end = ts + 1
            if end is None:
                end = max(t1, ts + 1)
            ov = min(end, t1) - max(ts, t0)
            if ts >= t0 and ts < t1 and st in ("compile", "burnout"):
                ov += (t1 - t0)  # make sure these are never hidden
            if ov > best_len and ov > 0:
                best, best_len = st, ov
        return best


class Visualizer:
    def __init__(self, live, width, burnout, required=0):
        self.live = live
        self.width = width
        self.burnout = burnout  # time_to_burn_out in ms (optional)
        self.required = required  # number_of_compiles_required (optional)
        self.result = None  # None (running) / "success" / "burnout"
        self.end_at = None
        self.coders = {}
        self.now = 0
        self.last_log = []
        self.last_draw = 0.0
        self.drawn_lines = 0

    def feed(self, line):
        m = LINE_RE.match(line)
        if not m:
            return False
        ts, cid, text = int(m.group(1)), int(m.group(2)), m.group(3)
        state = ACTIONS.get(text)
        if state is None:
            return False
        if self.result == "burnout":
            return False  # simulation already over
        coder = self.coders.setdefault(cid, Coder(cid))
        coder.apply(ts, state, self.required)
        self.now = max(self.now, ts)
        self.last_log.append(line.strip())
        self.last_log = self.last_log[-5:]
        if state == "burnout":
            self.result, self.end_at = "burnout", ts
        elif self.result is None and self.required and \
                all(c.done_at is not None for c in self.coders.values()):
            # coders that never logged anything can't be seen, so this is
            # only exact once every coder has appeared in the log
            self.result = "success"
            self.end_at = max(c.done_at for c in self.coders.values())
        return True

    def progress(self, c):
        """'[#####-----] 3/5' style compile progress."""
        if not self.required:
            return ""
        n = min(c.compiles, self.required)
        w = min(self.required, 10)
        filled = n * w // self.required
        bar = "#" * filled + "-" * (w - filled)
        text = "[%s] %d/%d" % (bar, c.compiles, self.required)
        return paint("1;32", text) if c.done_at is not None else text

    def bar(self, coder, bar_w):
        span = self.now + 1  # +1 so an event at the latest time is drawn
        out = []
        for i in range(bar_w):
            t0 = span * i // bar_w
            t1 = max(span * (i + 1) // bar_w, t0 + 1)
            st = coder.dominant(t0, t1)
            if st == "init":
                out.append(" ")
            else:
                out.append(paint(STATES[st][1], STATES[st][2]))
            if coder.dead_at is not None and t0 > coder.dead_at:
                out[-1] = " "
        return "".join(out)

    def ruler(self, bar_w):
        span = max(self.now, 1)
        ticks = [" "] * bar_w
        marks = [" "] * bar_w
        n = max(2, min(8, bar_w // 12))
        for k in range(n + 1):
            pos = min(bar_w - 1, k * (bar_w - 1) // n)
            label = "%dms" % (span * pos // max(bar_w - 1, 1))
            ticks[pos] = "|"
            start = min(pos, bar_w - len(label))
            if all(c == " " for c in marks[start:start + len(label)]):
                marks[start:start + len(label)] = list(label)
        return "".join(marks), "".join(ticks)

    def status(self, c):
        label = STATES[c.state][0]
        text = paint(STATES[c.state][1], "%-12s" % label)
        if c.state == "burnout":
            return text, "died at %dms (%dms after last compile)" % (
                c.dead_at,
                c.dead_at - (c.last_compile or 0))
        if c.state == "wait":
            extra = "has %d/2 dongles" % min(c.dongles, 2)
        elif c.state == "init":
            extra = ""
        else:
            extra = ""
        if c.last_compile is None:
            since = self.now
        else:
            since = self.now - c.last_compile
        note = "last compile %dms ago" % since if c.compiles else \
            "never compiled yet"
        if self.burnout and c.state != "init" and self.result is None:
            left = self.burnout - since
            if left <= self.burnout * 0.25:
                note = paint("1;31", "%s (DANGER: %dms left)"
                             % (note, max(left, 0)))
        return text, (extra + "  " if extra else "") + note

    def banner(self):
        if self.result == "success":
            return paint("1;42;30", " SUCCESS ") + paint(
                "1;32", " all coders compiled >= %d times (done at %dms)"
                % (self.required, self.end_at))
        if self.result == "burnout":
            who = [c.id for c in self.coders.values()
                   if c.dead_at is not None]
            return paint("1;41;97", " BURNED OUT ") + paint(
                "1;31", " coder #%s burned out at %dms - simulation stopped"
                % (", #".join(map(str, who)), self.end_at))
        goal = " (goal: %d compiles each)" % self.required \
            if self.required else ""
        return paint("33", "running...") + goal

    def render(self):
        cols = self.width or shutil.get_terminal_size((100, 24)).columns
        ids = sorted(self.coders)
        lines = [paint("1", "Codexion visualizer") +
                 "   time: %d ms   coders: %d" % (self.now, len(ids)),
                 self.banner(), ""]
        legend = "  ".join(paint(STATES[k][1], STATES[k][2]) + " " +
                           STATES[k][3] for k in LEGEND_ORDER)
        lines.append("legend: " + legend + "  (blank = no log yet)")
        lines.append("")
        pre = 12  # "coder #NN  " prefix
        bar_w = max(10, cols - pre - 2)
        r1, r2 = self.ruler(bar_w)
        lines.append(" " * pre + r1)
        lines.append(" " * pre + r2)
        for cid in ids:
            c = self.coders[cid]
            lines.append("coder #%-3d %s" % (cid, self.bar(c, bar_w)))
        lines.append("")
        lines.append(paint("1", "current status"))
        for cid in ids:
            c = self.coders[cid]
            st, note = self.status(c)
            prog = self.progress(c) or "compiles: %-3d" % c.compiles
            lines.append("  coder #%-3d %s %s  %s" % (cid, st, prog, note))
        lines.append("")
        lines.append(paint("1", "latest log"))
        lines.extend("  " + l for l in self.last_log)
        return lines

    def draw(self, force=False):
        nowt = time.monotonic()
        if not force and nowt - self.last_draw < REFRESH_INTERVAL:
            return
        self.last_draw = nowt
        lines = self.render()
        out = []
        if self.live and self.drawn_lines:
            out.append("\033[%dA" % self.drawn_lines)
        for l in lines:
            out.append("\r\033[K" + l + "\n")
        if self.live and self.drawn_lines > len(lines):
            extra = self.drawn_lines - len(lines)
            out.append(("\r\033[K\n" * extra) + "\033[%dA" % extra)
        sys.stdout.write("".join(out))
        sys.stdout.flush()
        self.drawn_lines = len(lines) if self.live else 0


def main():
    ap = argparse.ArgumentParser(description="Codexion visualizer")
    ap.add_argument("--no-live", action="store_true",
                    help="only print the final view")
    ap.add_argument("--width", type=int, default=0,
                    help="override terminal width")
    ap.add_argument("--delay", type=float, default=0.0,
                    help="seconds to sleep per log line (slow replay)")
    ap.add_argument("--burnout", type=int, default=0,
                    help="time_to_burn_out (ms): warn when a coder is close")
    ap.add_argument("--compiles", type=int, default=0,
                    help="number_of_compiles_required: show progress and "
                         "detect the success end condition")
    args = ap.parse_args()

    live = sys.stdout.isatty() and not args.no_live
    viz = Visualizer(live, args.width, args.burnout, args.compiles)

    try:
        for raw in sys.stdin:
            if viz.feed(raw) and live:
                viz.draw()
                if args.delay:
                    time.sleep(args.delay)
    except KeyboardInterrupt:
        pass
    viz.draw(force=True)


if __name__ == "__main__":
    main()
