# River & Watershed Data Analyzer

A command-line tool written in C that takes river monitoring station data and lets you explore it interactively. Practically, this is meant to support conservation workflows looking at things like flood risks, pollution spikes, and understanding long-term hydrological trends. Personally, this was a project based way for me to learn C which was a language I was excited to learn.

---

## Features

CSV Loads -> can take many stations' river monitoring data from a csv
Summarizes Statistics -> Finds mean flow rate, water level, temperature, and pollution index across all readings
Alerts to Flood Risks -> Flags any reading where water level/flow rate exceeds thrshold
Shows Pollution Spikes -> Identifies readings above a pollution index threshold
Gives Station Reports -> summarizes all readings and averages for stations
Exports Data -> Saves results to a new `.csv` for further analysis 

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

You can export your own monitoring data from most environmental agency databases like Environment Canada in CSV format and use it straight in this code. 

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

- Station deduplication is capped at 256 unique stations (thats adjustable though)
- no date filtering 


## AI statement


This was my first github project so i asked ai for formatting stuff because i wasn't sure what the convention was. It told me how to make my code readable by adding comments and helped me make my project shippable. 


---

## License

MIT — do whatever you want with it.
