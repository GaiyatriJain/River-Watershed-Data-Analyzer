# River & Watershed Data Analyzer

A command-line tool written in C that ingests river monitoring station data and lets you explore it interactively. Designed to support real-world conservation workflows: identify flood risks, spot pollution spikes, and understand long-term hydrological trends.

---

## Features

| Feature | Description |
|---|---|
| 📂 CSV Loader | Parses multi-station river monitoring data from any `.csv` file |
| 📊 Summary Stats | Mean flow rate, water level, temperature, and pollution index across all readings |
| 🚨 Flood Risk Alerts | Flags any reading where water level or flow rate exceeds configurable thresholds |
| ☠️ Pollution Spikes | Identifies readings above a pollution index cutoff |
| 🏞️ Station Report | Per-station breakdown of all readings and averages |
| 💾 Export | Saves pollution spike results to a new `.csv` for further analysis |

---

## Building

Requires `gcc` and `make`.

```bash
git clone https://github.com/yourname/waterwatch.git
cd waterwatch
make
```

Run with:
```bash
./waterwatch
```

---

## Usage

On launch you get an interactive menu. Load a CSV first, then run any analysis.

```
╔══════════════════════════════════════╗
║   River & Watershed Data Analyzer    ║
╠══════════════════════════════════════╣
║  1. Load CSV data file               ║
║  2. Summary statistics               ║ 
║  3. Flood risk alerts                ║
║  4. Pollution spike detection        ║
║  5. Station report                   ║
║  6. List all stations                ║
║  7. Export pollution report to CSV   ║
║  8. Quit                             ║
╚══════════════════════════════════════╝
```

A sample dataset (`sample_data.csv`) is included with 85 readings across 5 fictional monitoring stations.

---

## CSV Format

```
station,date,flow_rate,water_level,temperature,pollution_index
RiverMouth_A,2024-01-03,523.4,3.21,8.2,22.1
...
```

| Column | Unit | Description |
|---|---|---|
| `station` | — | Station name or ID |
| `date` | YYYY-MM-DD | Date of reading |
| `flow_rate` | m³/s | Volumetric water flow |
| `water_level` | metres | Depth above baseline |
| `temperature` | °C | Water temperature |
| `pollution_index` | 0–100 | 0 = pristine, 100 = critically polluted |

You can export your own monitoring data from most environmental agency databases (e.g., USGS Water Resources, Environment Canada) in CSV format and drop it straight in.

---

## Technical Design

```
main.c
├── Reading struct          — one record per monitoring event
├── Database struct         — head pointer + count (linked list)
├── load_csv()             — File I/O + tokenisation with strtok
├── summary_stats()        — single-pass aggregation via pointer traversal
├── flood_risk()           — threshold filtering with user input
├── pollution_spikes()     — threshold filtering + reporting
├── station_report()       — filtered traversal by station name
├── export_pollution_report() — File I/O output to CSV
└── main()                 — menu loop with fgets-based input
```

**C concepts used:**
- `struct` for typed data records
- Singly linked list with dynamic `malloc` / `free`
- File I/O with `fopen` / `fgets` / `fclose`
- Pointer arithmetic and traversal
- `strtok` for CSV tokenisation
- `atof` / `atoi` for string-to-number parsing
- Graceful error handling for bad files and malformed rows

---

## Sample Output

```
══════════════ SUMMARY STATISTICS ══════════════
  Total readings      : 85
  Avg flow rate       : 612.34 m³/s
  Avg water level     : 3.42 m
  Avg temperature     : 13.87 °C
  Avg pollution index : 28.14 / 100
  Peak flow           : 3102.80 m³/s  @ GlacierFork_C
  Lowest flow         : 27.60 m³/s
  Worst pollution     : 74.80 / 100 @ MillCreek_B
═════════════════════════════════════════════════
```

---

## Limitations & Future Work

- Station deduplication is capped at 256 unique stations (trivially adjustable)
- Date filtering (e.g. "show me summer readings only") would be a natural next feature
- A `gnuplot` integration could visualise flow-over-time graphs directly from the terminal
- Currently single-threaded; large datasets (100k+ rows) load fine but analysis could be parallelised

---

## License

MIT — do whatever you want with it.
