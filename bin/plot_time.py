#!/bin/python3
"""Plot time-domain waveforms stored in NIDAQ HDF5 files.

Events are either placed back to back on one time axis (default) or
overlaid on top of each other (--overlay). Long records are decimated
for display; the default min/max envelope keeps spikes visible.
Optionally export the plotted traces to two-column CSV files whose
leading '#' comment lines carry all of the HDF5 metadata.
"""

import argparse
import os
import sys
from datetime import datetime, timezone

import h5py
import numpy as np


EPILOG = """\
input specification:
  FILE[:CHANNELS[:LABEL]]
    FILE      path to an HDF5 file produced by the NIDAQ recorder
    CHANNELS  comma-separated channel indices (default: all channels)
    LABEL     legend label prefix (default: the file's base name)

event selection (-e):
  comma-separated 1-based event numbers and ranges, e.g. 1-5,8,10-12

decimation:
  By default each trace is reduced to about --max-points points.
  'minmax' keeps the min and max of every block (spikes stay visible),
  'mean' averages each block, 'stride' keeps every k-th sample.
  Use -D 1 to plot every sample.

examples:
  %(prog)s run001.hdf5
  %(prog)s run001.hdf5:0,1 -e 1-10 --subplots
  %(prog)s run001.hdf5:2 -e 3 -D 1              # one event, full resolution
  %(prog)s run001.hdf5 --overlay -e 1-20        # triggered pulses on top of each other
  %(prog)s run001.hdf5:0 -e 1 -D 1 --csv -n     # export one event at full rate
"""


def parse_input_spec(spec):
    """Split FILE[:CHANNELS[:LABEL]] into its parts."""
    parts = spec.split(':', 2)
    filename = parts[0]
    channels = None
    label = os.path.basename(filename)

    if len(parts) > 1 and parts[1].strip():
        try:
            channels = [int(x) for x in parts[1].split(',') if x.strip()]
        except ValueError:
            raise argparse.ArgumentTypeError(
                f"invalid channel list '{parts[1]}' in '{spec}' "
                "(expected comma-separated integers)")
    if len(parts) > 2 and parts[2]:
        label = parts[2]

    return filename, channels, label


def parse_event_list(text):
    """Parse '1-5,8,10-12' into a sorted list of unique 1-based event numbers."""
    events = set()
    for tok in text.split(','):
        tok = tok.strip()
        if not tok:
            continue
        try:
            if '-' in tok:
                lo, hi = (int(x) for x in tok.split('-', 1))
                if lo > hi:
                    raise ValueError
                events.update(range(lo, hi + 1))
            else:
                events.add(int(tok))
        except ValueError:
            raise argparse.ArgumentTypeError(f"invalid event selection '{tok}'")
    if not events or min(events) < 1:
        raise argparse.ArgumentTypeError("event numbers start at 1")
    return sorted(events)


def format_attr(value):
    """Render an HDF5 attribute value as a single-line string."""
    if isinstance(value, bytes):
        return value.decode('utf-8', errors='replace')
    if isinstance(value, np.ndarray):
        if value.dtype.kind in ('S', 'O', 'U'):
            items = [format_attr(v) for v in value.ravel()]
            return '[' + ', '.join(items) + ']' if value.size != 1 else items[0]
        if value.size == 1:
            return format_attr(value.item())
        return np.array2string(value, separator=', ', max_line_width=np.inf,
                               threshold=np.inf).replace('\n', '')
    if isinstance(value, np.generic):
        return str(value.item())
    return str(value)


def decimate(t, y, k, mode):
    """Reduce (t, y) by factor k. Returns new (t, y)."""
    if k <= 1:
        return t, y
    if mode == 'stride':
        return t[::k], y[::k]

    nblk = len(y) // k
    if nblk == 0:
        return t[:1], y[:1]
    tb = t[:nblk * k].reshape(nblk, k)
    yb = y[:nblk * k].reshape(nblk, k)

    if mode == 'mean':
        return tb.mean(axis=1), yb.mean(axis=1)

    # minmax: two points per block, in time order, so the envelope is drawn.
    imin = yb.argmin(axis=1)
    imax = yb.argmax(axis=1)
    first = np.minimum(imin, imax)
    second = np.maximum(imin, imax)
    rows = np.arange(nblk)
    t_out = np.column_stack((tb[rows, first], tb[rows, second])).ravel()
    y_out = np.column_stack((yb[rows, first], yb[rows, second])).ravel()
    return t_out, y_out


def write_csv(path, t, y, metadata, column_name):
    """Write a two-column CSV with metadata as leading '#' comment lines."""
    with open(path, 'w', encoding='utf-8', newline='') as fh:
        for section, entries in metadata:
            fh.write(f"# [{section}]\n")
            for key, val in entries:
                fh.write(f"# {key} = {val}\n")
        fh.write(f"Time (s),{column_name}\n")
        np.savetxt(fh, np.column_stack((t, y)), delimiter=',', fmt='%.9e')


def build_parser():
    parser = argparse.ArgumentParser(
        description="Plot time-domain waveforms from NIDAQ HDF5 files, and "
                    "optionally export the traces as CSV.",
        epilog=EPILOG,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('inputs', nargs='+', metavar='FILE[:CHANNELS[:LABEL]]',
                        help="input HDF5 file(s), optionally with channel selection and label")
    parser.add_argument('-g', '--group', default='adc1',
                        help="HDF5 group containing the events (default: %(default)s)")

    sel = parser.add_argument_group('data selection')
    sel.add_argument('-e', '--events', type=parse_event_list, metavar='LIST',
                     help="events to plot, e.g. 1-5,8 (default: all)")
    sel.add_argument('-r', '--raw', action='store_true',
                     help="plot raw ADC counts instead of converting to volts")
    sel.add_argument('-o', '--overlay', action='store_true',
                     help="overlay events on a common 0..T axis instead of "
                          "placing them back to back")

    dec = parser.add_argument_group('decimation')
    dec.add_argument('-D', '--decimate', type=int, metavar='K',
                     help="fixed decimation factor (1 = every sample; "
                          "default: automatic from --max-points)")
    dec.add_argument('-m', '--max-points', type=int, default=200000, metavar='N',
                     help="target points per trace for automatic decimation "
                          "(default: %(default)s)")
    dec.add_argument('--mode', choices=['minmax', 'mean', 'stride'], default='minmax',
                     help="decimation method (default: %(default)s)")

    csv = parser.add_argument_group('CSV export')
    csv.add_argument('-c', '--csv', action='store_true',
                     help="write each plotted trace (after decimation) to "
                          "time_<file>_ch<N>.csv")
    csv.add_argument('-d', '--csv-dir', default='.', metavar='DIR',
                     help="directory for CSV output (default: current directory)")

    plot = parser.add_argument_group('plotting')
    plot.add_argument('-n', '--no-plot', action='store_true',
                      help="do not display the plot (useful with --csv or --save)")
    plot.add_argument('-s', '--save', metavar='FIG',
                      help="save the figure to this file (format from extension)")
    plot.add_argument('--subplots', action='store_true',
                      help="one panel per channel (shared time axis)")
    plot.add_argument('--xlim', nargs=2, type=float, metavar=('MIN', 'MAX'),
                      help="time range in s")
    plot.add_argument('--ylim', nargs=2, type=float, metavar=('MIN', 'MAX'),
                      help="vertical range (V, or counts with --raw)")
    plot.add_argument('--title', help="plot title")
    plot.add_argument('--figsize', nargs=2, type=float, default=[10, 6],
                      metavar=('W', 'H'), help="figure size in inches (default: 10 6)")

    return parser


def load_file(path, channel_list, args):
    """Read the selected events/channels of one file.

    Returns (info dict, {channel: list of (event_number, t, y)}) or None on error.
    """
    try:
        fh = h5py.File(path, 'r')
    except OSError as e:
        print(f"  error: cannot open '{path}': {e}", file=sys.stderr)
        return None

    with fh:
        if args.group not in fh:
            print(f"  error: group '{args.group}' not found in '{path}'", file=sys.stderr)
            return None
        group = fh[args.group]
        attrs = group.attrs

        nb_events = int(attrs['nb_events'])
        nb_samples = int(attrs['nb_samples'])
        sample_rate = float(attrs['sample_rate'])
        nb_channels = int(attrs['nb_channels'])
        poly_coeffs = np.asarray(attrs['adc_conversion_factor'])[:, ::-1]

        if channel_list is None:
            channel_list = list(range(nb_channels))
        bad = [c for c in channel_list if not 0 <= c < nb_channels]
        if bad:
            print(f"  warning: channel(s) {bad} out of range "
                  f"(file has {nb_channels}), skipping them", file=sys.stderr)
            channel_list = [c for c in channel_list if c not in bad]

        events = args.events or list(range(1, nb_events + 1))
        missing = [e for e in events if e > nb_events]
        if missing:
            print(f"  warning: file has {nb_events} events; skipping "
                  f"{len(missing)} requested event(s) beyond that", file=sys.stderr)
            events = [e for e in events if e <= nb_events]

        if args.decimate is not None:
            k = max(1, args.decimate)
        else:
            per_trace = nb_samples if args.overlay else nb_samples * len(events)
            pts_per_block = 2 if args.mode == 'minmax' else 1
            k = max(1, int(np.ceil(per_trace * pts_per_block / max(1, args.max_points))))

        t_frame = np.arange(nb_samples) / sample_rate
        frame_len = nb_samples / sample_rate
        traces = {c: [] for c in channel_list}

        for ev in events:
            data = np.asarray(group['event_{}'.format(ev)])   # read once, all channels
            t = t_frame if args.overlay else t_frame + (ev - 1) * frame_len
            for c in channel_list:
                y = data[c, :].astype(float) if args.raw else np.polyval(poly_coeffs[c], data[c, :])
                traces[c].append((ev, *decimate(t, y, k, args.mode)))

        info = dict(
            nb_events=nb_events, nb_samples=nb_samples, sample_rate=sample_rate,
            events=events, decimation=k,
            file_meta=[(key, format_attr(v)) for key, v in fh.attrs.items()],
            group_meta=[(key, format_attr(v)) for key, v in attrs.items()],
        )
        return info, traces


def summarize_events(events):
    """Compact '1-5,8' style string for a sorted list of ints."""
    out, start, prev = [], None, None
    for e in events + [None]:
        if start is None:
            start = prev = e
        elif e is not None and e == prev + 1:
            prev = e
        else:
            out.append(str(start) if start == prev else f"{start}-{prev}")
            start = prev = e
    return ','.join(out)


def main(argv=None):
    parser = build_parser()
    args = parser.parse_args(argv)

    try:
        specs = [parse_input_spec(s) for s in args.inputs]
    except argparse.ArgumentTypeError as e:
        parser.error(str(e))

    unit = 'ADC counts' if args.raw else 'V'
    column_name = 'ADC (counts)' if args.raw else 'Voltage (V)'

    if args.csv:
        os.makedirs(args.csv_dir, exist_ok=True)

    # Load everything first so subplot layout knows all channels.
    loaded = []
    for filename, channel_list, label_prefix in specs:
        print("Processing", filename)
        res = load_file(filename, channel_list, args)
        if res is None:
            continue
        info, traces = res
        if not info['events'] or not traces:
            print("  nothing to plot", file=sys.stderr)
            continue
        if info['decimation'] > 1:
            print(f"  decimating by {info['decimation']} ({args.mode})")
        loaded.append((filename, label_prefix, info, traces))

    if args.csv:
        for filename, label_prefix, info, traces in loaded:
            stem = os.path.splitext(os.path.basename(filename))[0]
            for c, segs in traces.items():
                out = os.path.join(args.csv_dir, f"time_{stem}_ch{c}.csv")
                if args.overlay:
                    # Overlaid events share time values; keep them separable
                    # by writing one file per event.
                    for ev, t, y in segs:
                        write_one_csv(out.replace('.csv', f'_ev{ev}.csv'), t, y,
                                      filename, c, label_prefix, info, [ev],
                                      args, column_name)
                else:
                    t = np.concatenate([s[1] for s in segs])
                    y = np.concatenate([s[2] for s in segs])
                    write_one_csv(out, t, y, filename, c, label_prefix, info,
                                  info['events'], args, column_name)

    make_figure = not args.no_plot or args.save
    if not make_figure:
        return 0

    if args.no_plot:
        import matplotlib
        matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    all_channels = sorted({c for *_, traces in loaded for c in traces})
    if args.subplots and len(all_channels) > 1:
        fig, axes = plt.subplots(len(all_channels), 1, sharex=True,
                                 figsize=args.figsize, squeeze=False)
        ax_for = {c: axes[i, 0] for i, c in enumerate(all_channels)}
        for c, ax in ax_for.items():
            ax.set_ylabel(f"Ch {c} [{unit}]")
        axes_list = list(ax_for.values())
    else:
        fig, ax = plt.subplots(figsize=args.figsize)
        ax_for = {c: ax for c in all_channels}
        ax.set_ylabel(f"Voltage [{unit}]" if not args.raw else f"[{unit}]")
        axes_list = [ax]

    for filename, label_prefix, info, traces in loaded:
        for c, segs in traces.items():
            ax = ax_for[c]
            label = f"{label_prefix}, Channel {c}"
            if args.overlay:
                color = None
                for j, (ev, t, y) in enumerate(segs):
                    line, = ax.plot(t, y, lw=0.8, alpha=0.7 if len(segs) > 1 else 1,
                                    color=color, label=label if j == 0 else None)
                    color = line.get_color()   # one color per file/channel
            else:
                # Break the line between non-adjacent events so gaps aren't bridged.
                ts, ys, prev = [], [], None
                for ev, t, y in segs:
                    if prev is not None and ev != prev + 1:
                        ts.append([np.nan]); ys.append([np.nan])
                    ts.append(t); ys.append(y)
                    prev = ev
                ax.plot(np.concatenate(ts), np.concatenate(ys), lw=0.8, label=label)

    axes_list[-1].set_xlabel('Time within event [s]' if args.overlay else 'Time [s]')
    for ax in axes_list:
        if args.xlim:
            ax.set_xlim(*args.xlim)
        if args.ylim:
            ax.set_ylim(*args.ylim)
        if ax.get_legend_handles_labels()[0]:
            ax.legend(loc='upper right')
        ax.grid(alpha=0.3)
    if args.title:
        fig.suptitle(args.title)
    fig.tight_layout()

    if args.save:
        fig.savefig(args.save, bbox_inches='tight')
        print("Saved figure to", args.save)
    if not args.no_plot:
        plt.show()

    return 0


def write_one_csv(path, t, y, filename, channel, label, info, events, args, column_name):
    k = info['decimation']
    analysis_meta = [
        ('source_file', os.path.abspath(filename)),
        ('group', args.group),
        ('channel', channel),
        ('label', label),
        ('events', summarize_events(list(events))),
        ('time_axis', 'time within event' if args.overlay
                      else 'continuous: (event_number-1)*nb_samples/sample_rate + sample/sample_rate'),
        ('units', 'raw ADC counts' if args.raw else 'volts (adc_conversion_factor applied)'),
        ('decimation_factor', k),
        ('decimation_mode', 'none' if k == 1 else args.mode),
        ('columns', f'Time (s), {column_name}'),
        ('created', datetime.now(timezone.utc).isoformat(timespec='seconds')),
    ]
    write_csv(path, t, y, [
        ('analysis', analysis_meta),
        ('file attributes', info['file_meta']),
        (f'{args.group} attributes', info['group_meta']),
    ], column_name)
    print("  wrote", path)


if __name__ == '__main__':
    sys.exit(main())
