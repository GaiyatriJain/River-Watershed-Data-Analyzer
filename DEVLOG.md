# Developer Log

## Day 1 
I decided I wanted to learn a low level language and I chose C. I spent the first day learning the fundamentals but ultimately thought project based learning would be the most informative. 

## Day 2 
I wanted to do something with real data and kept thinking about how environmental agencies publish river monitoring data publicly, but there's no simple local tool to actually like query it ie you usually need like specialized software. I thought I could build a fast, offline C tool that any field researcher could drop a CSV into.

I outlined out the features I wanted/would be useful. Ie.
- Load any CSV from a monitoring dataset
- Get summary statistics 
- Flag readings that suggest flood conditions
- Spot pollution events
- Drill into a specific station's history
- Export filtered results for reporting

---

## Day 3

The natural unit of data is a single monitoring reading that looks like a station and its measurements. In C this becomes a `struct`:

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

First feature I really got working as `malloc`ing a `Reading`, filling its fields manually, and printing them back. it was small but it meant the memory was working.
---

## Day 4

I assumed `strtok` would be straightforward and it mostly was, but there were two small issues

**1:** `strtok` modifies the string in place. I was passing `line` directly from `fgets`, which was fine, but I had to be careful to call it in the right order and not re-use the buffer before I was done tokenising.

**2:** `fgets` leaves a trailing `\n` on the string. First run, every station name had a newline baked in. fixed with:
```c
line[strcspn(line, "\r\n")] = '\0';
```

I also wrote `free_db()` here which walks the list, frees every node, resets the head and count. Important to call this before loading a new file so you don't leak the old one.

---

## Day 2 (continued) — Building sample data

I realised I needed realistic data to test against — not just dummy values. I got chat gpt to make up 5 fictional monitoring stations with plausible patterns:

- `RiverMouth_A` — mid-size river, seasonal flow variation
- `MillCreek_B` — urban-influenced creek with elevated pollution
- `GlacierFork_C` — high flow, very clean (glacial melt driven)
- `Estuary_D` — moderate pollution from agricultural runoff
- `HeadwaterSpring_E` — tiny, pristine spring source

Each station has weekly/biweekly readings mimmicing seasonal patterns (higher flow in spring snowmelt, lower in summer drought, colder temps in winter). 
---

## Day 3 — Analysis functions

With data loading, I built analysis on top of the linked list. All of them follow the same pattern: traverse `db->head` with a `for` loop, compute or filter, print.

**Summary stats** uses a single pass to accumulate totals and track max/min value

## Day 5

The last feature I built was exporting filtered results back to CSV. this lets the tool make things that can be opened in Excel or fed into further analysis.

I made sure to count exported rows and print the result so the user knows the export worked.

I then built the interactive menu which was important to me as its what users actually see and accessibility is important to me. The box-drawing characters (`╔`, `║`, `╚` etc.) were a small component to make the terminal more usable. 

The menu loop is a `while(1)` with a `switch`. Each case either does work directly or prompts for additional parameters. 

## Bugs I fixed

**Bug 1:** Calling `free_db` on an empty database (head = NULL) crashed--> fixed by checking `cur` is not NULL before dereferencing in the while loop — which `while (cur)` already handles. 
---

## Reflections

What I'd do differently:
- Add date range filtering 
- Separate the code into multiple `.c`/`.h` files

What went well:
- The linked list approach made loading arbitrary-size datasets easy
- Keeping analysis functions so that they only read not modified meant I could add new ones without worrying about side effects
