#!/bin/python3
"""Plot the one-sided spectral density of waveforms stored in NIDAQ HDF5 files.

Optionally export each spectrum to a two-column CSV file whose leading
'#' comment lines carry all of the HDF5 metadata (file-level and ADC-group
attributes) together with the analysis parameters.
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

examples:
  %(prog)s run001.hdf5
  %(prog)s run001.hdf5:0,2 run002.hdf5:1:"after fix"
  %(prog)s run001.hdf5 --csv --csv-dir spectra/ --no-plot
  %(prog)s run001.hdf5 --save fft.png --xlim 1 1e4
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


def make_window(name, n):
    """Return a periodic (DFT-even) window of length n."""
    if name == 'hann':
        return 0.5 - 0.5 * np.cos(2.0 * np.pi * np.arange(n) / n)
    return np.ones(n)


def compute_spectrum(group, channel, nb_events, nb_samples, sample_rate, poly_coeffs,
                     window='hann', detrend=True):
    """One-sided power spectral density of one channel, averaged over events.

    Each event is treated as one segment (Welch's method without overlap):
        P_k = |rfft(w * (x - mean))|_k^2 / (fs * sum(w^2))
    doubled for all bins except DC and Nyquist, then averaged over events.
    Returns (freq [Hz], psd [V^2/Hz], df [Hz], enbw [Hz]).
    """
    w = make_window(window, nb_samples)
    scale = 1.0 / (sample_rate * np.sum(w ** 2))

    accum = np.zeros(nb_samples // 2 + 1)
    for i in range(nb_events):
        event = np.asarray(group['event_{}'.format(i + 1)])
        volts = np.polyval(poly_coeffs[channel], event[channel, :])
        if detrend:
            volts = volts - np.mean(volts)
        accum += np.abs(np.fft.rfft(w * volts)) ** 2

    psd = accum * scale / nb_events
    # Fold negative frequencies onto positive: double all but DC (and Nyquist if N even).
    if nb_samples % 2 == 0:
        psd[1:-1] *= 2.0
    else:
        psd[1:] *= 2.0

    freq = np.fft.rfftfreq(nb_samples, 1.0 / sample_rate)
    df = sample_rate / nb_samples
    enbw = sample_rate * np.sum(w ** 2) / np.sum(w) ** 2
    return freq, psd, df, enbw


def write_csv(path, freq, density, metadata, column_name):
    """Write a two-column CSV with metadata as leading '#' comment lines."""
    with open(path, 'w', encoding='utf-8', newline='') as fh:
        for section, entries in metadata:
            fh.write(f"# [{section}]\n")
            for key, val in entries:
                fh.write(f"# {key} = {val}\n")
        fh.write(f"Frequency (Hz),{column_name}\n")
        np.savetxt(fh, np.column_stack((freq, density)), delimiter=',', fmt='%.6e')


def build_parser():
    parser = argparse.ArgumentParser(
        description="Plot the one-sided spectral density (ASD in V/√Hz, or PSD "
                    "in V^2/Hz) of waveforms in NIDAQ HDF5 files, averaged over "
                    "events in power (Welch's method), and optionally export the "
                    "spectra as CSV.",
        epilog=EPILOG,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('inputs', nargs='+', metavar='FILE[:CHANNELS[:LABEL]]',
                        help="input HDF5 file(s), optionally with channel selection and label")
    parser.add_argument('-g', '--group', default='adc1',
                        help="HDF5 group containing the events (default: %(default)s)")

    spec = parser.add_argument_group('spectrum')
    spec.add_argument('-w', '--window', choices=['hann', 'boxcar'], default='hann',
                      help="window applied to each event (default: %(default)s)")
    spec.add_argument('--no-detrend', action='store_true',
                      help="do not subtract the mean of each event before the FFT")
    spec.add_argument('-p', '--psd', action='store_true',
                      help="output power spectral density in V^2/Hz instead of "
                           "amplitude spectral density in V/√Hz")

    csv = parser.add_argument_group('CSV export')
    csv.add_argument('-c', '--csv', action='store_true',
                     help="write each spectrum to a CSV file "
                          "named fft_<file>_ch<N>.csv")
    csv.add_argument('-d', '--csv-dir', default='.', metavar='DIR',
                     help="directory for CSV output (default: current directory)")

    plot = parser.add_argument_group('plotting')
    plot.add_argument('-n', '--no-plot', action='store_true',
                      help="do not display the plot (useful with --csv or --save)")
    plot.add_argument('-s', '--save', metavar='FIG',
                      help="save the figure to this file (format from extension)")
    plot.add_argument('--xscale', choices=['log', 'linear'], default='log',
                      help="x-axis scale (default: %(default)s)")
    plot.add_argument('--yscale', choices=['log', 'linear'], default='log',
                      help="y-axis scale (default: %(default)s)")
    plot.add_argument('--xlim', nargs=2, type=float, metavar=('MIN', 'MAX'),
                      help="x-axis range in Hz")
    plot.add_argument('--ylim', nargs=2, type=float, metavar=('MIN', 'MAX'),
                      help="y-axis range (in V/√Hz, or V^2/Hz with --psd)")
    plot.add_argument('--title', help="plot title")
    plot.add_argument('--figsize', nargs=2, type=float, default=[8, 6],
                      metavar=('W', 'H'), help="figure size in inches (default: 8 6)")

    return parser


def main(argv=None):
    parser = build_parser()
    args = parser.parse_args(argv)

    try:
        specs = [parse_input_spec(s) for s in args.inputs]
    except argparse.ArgumentTypeError as e:
        parser.error(str(e))

    make_figure = not args.no_plot or args.save
    if make_figure:
        if args.no_plot:
            import matplotlib
            matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        plt.figure(figsize=args.figsize)

    if args.csv:
        os.makedirs(args.csv_dir, exist_ok=True)

    column_name = 'PSD (V^2/Hz)' if args.psd else 'ASD (V/√Hz)'

    n_curves = 0
    for filename, channel_list, label_prefix in specs:
        print("Processing", filename)

        try:
            fh = h5py.File(filename, 'r')
        except OSError as e:
            print(f"  error: cannot open '{filename}': {e}", file=sys.stderr)
            continue

        with fh:
            if args.group not in fh:
                print(f"  error: group '{args.group}' not found in '{filename}'", file=sys.stderr)
                continue
            group = fh[args.group]
            attrs = group.attrs

            nb_events = int(attrs['nb_events'])
            nb_samples = int(attrs['nb_samples'])
            sample_rate = float(attrs['sample_rate'])
            nb_channels = int(attrs['nb_channels'])
            poly_coeffs = np.asarray(attrs['adc_conversion_factor'])[:, ::-1]

            if nb_events < 1:
                print(f"  warning: no events in '{filename}', skipping", file=sys.stderr)
                continue

            if channel_list is None:
                channel_list = list(range(nb_channels))
            bad = [c for c in channel_list if not 0 <= c < nb_channels]
            if bad:
                print(f"  warning: channel(s) {bad} out of range "
                      f"(file has {nb_channels}), skipping them", file=sys.stderr)
                channel_list = [c for c in channel_list if c not in bad]

            file_meta = [(k, format_attr(v)) for k, v in fh.attrs.items()]
            group_meta = [(k, format_attr(v)) for k, v in attrs.items()]

            for channel in channel_list:
                freq, psd, df, enbw = compute_spectrum(
                    group, channel, nb_events, nb_samples, sample_rate, poly_coeffs,
                    window=args.window, detrend=not args.no_detrend)
                density = psd if args.psd else np.sqrt(psd)

                if make_figure:
                    plt.plot(freq, density, label=f"{label_prefix}, Channel {channel}")
                    n_curves += 1

                if args.csv:
                    stem = os.path.splitext(os.path.basename(filename))[0]
                    out = os.path.join(args.csv_dir, f"fft_{stem}_ch{channel}.csv")
                    analysis_meta = [
                        ('source_file', os.path.abspath(filename)),
                        ('group', args.group),
                        ('channel', channel),
                        ('label', label_prefix),
                        ('events_averaged', nb_events),
                        ('frequency_resolution_hz', f"{df:.6e}"),
                        ('spectrum', 'one-sided ' + ('PSD' if args.psd else 'ASD = sqrt(PSD)')),
                        ('method', 'Welch average over events, one event per segment, no overlap; '
                                   'PSD_k = 2|rfft(w*x)|_k^2 / (fs*sum(w^2)), DC/Nyquist not doubled'),
                        ('window', args.window),
                        ('detrend', 'none' if args.no_detrend else 'constant (mean removed)'),
                        ('enbw_hz', f"{enbw:.6e}"),
                        ('columns', f'Frequency (Hz), {column_name}'),
                        ('created', datetime.now(timezone.utc).isoformat(timespec='seconds')),
                    ]
                    write_csv(out, freq, density, [
                        ('analysis', analysis_meta),
                        ('file attributes', file_meta),
                        (f'{args.group} attributes', group_meta),
                    ], column_name)
                    print("  wrote", out)

    if not make_figure:
        return 0

    plt.xlabel(r'Frequency [Hz]')
    plt.ylabel(r'PSD [V$^2$/Hz]' if args.psd else r'ASD [V/$\sqrt{Hz}$]')
    plt.xscale(args.xscale)
    plt.yscale(args.yscale)
    if args.xlim:
        plt.xlim(*args.xlim)
    if args.ylim:
        plt.ylim(*args.ylim)
    if args.title:
        plt.title(args.title)
    if n_curves:
        plt.legend()

    if args.save:
        plt.savefig(args.save, bbox_inches='tight')
        print("Saved figure to", args.save)
    if not args.no_plot:
        plt.show()

    return 0


if __name__ == '__main__':
    sys.exit(main())
