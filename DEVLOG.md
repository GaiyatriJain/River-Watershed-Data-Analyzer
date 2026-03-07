# Developer Log

## Day 1 
I decided I wanted to learn a low level language and I chose C. I spent the first day learning the fundamentals but ultimately thought project based learning would be the most informative. 

## Day 2 
I wanted to do something with real data and kept thinking about how environmental agencies publish river monitoring data publicly, but there's no simple local tool to actually query it, you usually need a web connection or proprietary software. I thought I could build a fast, offline C tool that any field researcher could drop a CSV into.

I sketched out the features I'd actually want:
- Load any CSV from a monitoring dataset
- Get summary statistics in one shot
- Flag readings that suggest flood conditions
- Spot pollution events
- Drill into a specific station's history
- Export filtered results for reporting

---

## Day 3

The natural unit of data is a single monitoring reading looks like one station, one date, and a handful of measurements. In C this becomes a `struct`:

```c
typedef struct Reading {
    char station[64];
    char date[64];
    float flow_rate;
    float water_level;
    float temperature;
    float pollution_index;
    struct Reading *next;
} Reading;
```

The `*next` pointer makes it a node in a linked list, which worked as I didn't know how many readings I'd have to load upfront, so a dynamic structure works better than a fixed array. I wrapped the list in a `Database` struct holding just the head pointer and a count.

First feature I really got working as `malloc`ing a `Reading`, filling its fields manually, and printing them back. It was a small task but it meant the memory was working.
---

## Day 4

I assumed `strtok` would be straightforward and it mostly was, but there were two small issues

**1:** `strtok` modifies the string in place. I was passing `line` directly from `fgets`, which was fine, but I had to be careful to call it in the right order and not re-use the buffer before I was done tokenising.

**2:** `fgets` leaves a trailing `\n` on the string. First run, every station name had a newline baked in. Fixed with:
```c
line[strcspn(line, "\r\n")] = '\0';
```

The parser reads line-by-line, tokenises on commas, converts strings to floa   with `atof`, and appends each valid `Reading` to the linked list. Malformed rows (wrong field count, missing values) are counted and reported at load time rather than crashing which mattered for  real world messier data.

I also wrote `free_db()` here which walks the list, frees every node, resets the head and count. Important to call this before loading a new file so you don't leak the old one.

---

## Day 2 (continued) — Building sample data

I realised I needed realistic data to test against — not just dummy values. I made up 5 fictional monitoring stations with plausible hydrological patterns:

- `RiverMouth_A` — mid-size river, seasonal flow variation
- `MillCreek_B` — urban-influenced creek with elevated pollution
- `GlacierFork_C` — high flow, very clean (glacial melt driven)
- `Estuary_D` — moderate pollution from agricultural runoff
- `HeadwaterSpring_E` — tiny, pristine spring source

Each station has weekly/biweekly readings through 2024 with values that reflect real seasonal patterns (higher flow in spring snowmelt, lower in summer drought, colder temps in winter). This made testing much more interesting — `MillCreek_B` in late December reliably triggers pollution alerts, `GlacierFork_C` hits flood thresholds in May/June.

---

## Day 3 — Analysis functions

With data loading, I built analysis on top of the linked list. All of them follow the same pattern: traverse `db->head` with a `for` loop, compute or filter, print.

**Summary stats** uses a single pass to accumulate totals and track max/min values simultaneously. No sorting needed.

**Flood risk**implementation  was interesting, I made the thresholds *configurable* at runtime rather than hardcoded. This made the function useful as  different rivers have different baselines, so a one-size-fits-all threshold is useless in practice.

**Pollution spikes** works the same way. Default threshold of 60 (on a 0–100 index) flags obviously bad readings..

**Station report** does a filtered traversal — only prints rows where `strcmp(r->station, station) == 0`. 



## Day 5

The last feature I built was exporting filtered results back to CSV. This closes the loop: load data → analyse → save findings. It uses `fopen` in write mode (`"w"`), `fprintf` for structured output, and `fclose`. this lets the  tool make artefacts that can be opened in Excel or fed into further analysis.

I made sure to count exported rows and print the result so the user knows the export worked (and isn't staring at a silent successful outcome).

I then built the interactive menu which was important to me as its what users actually see.  I used `fgets` for all input, not `scanf`, because `scanf` has horrible edge cases with leftover newlines in the buffer that cause phantom menu selections. `fgets` + `atoi` / `atof` is safer.

The menu loop is a `while(1)` with a `switch`. Each case either does work directly or prompts for additional parameters. I added sensible defaults (e.g. pressing Enter at the pollution threshold prompt gives you 60) so casual users don't need to think too hard.

The box-drawing characters (`╔`, `║`, `╚` etc.) were a small touch to make the terminal more usable. 

---

## Bugs I fixed

**Bug 1:** Calling `free_db` on an empty database (head = NULL) crashed. Fixed by checking `cur` is not NULL before dereferencing in the while loop — which `while (cur)` already handles. The real bug was I had mistakenly written `while (cur->next)` which skips the last node.

**Bug 2:** The station de-duplication list was on the stack with a fixed size of 256. If someone loaded a dataset with more than 256 unique stations, it would silently truncate. Added a bounds check and a note in the README under Limitations.

---

## Reflections

What I'd do differently:
- Use a hash set for station de-duplication instead of the O(n²) linear scan
- Add date range filtering — "show me only January readings" — which would need date parsing
- Separate the code into multiple `.c`/`.h` files once it grew past ~300 lines

What went well:
- The linked list approach made loading arbitrary-size datasets trivial
- Keeping analysis functions pure (they only read the list, never modify it) meant I could add new ones without worrying about side effects
- The sample data being realistic made debugging funner — you're spotting real-looking pollution events, not just checking if numbers print
