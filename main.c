#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 64
#define MAX_LINE 256

/* ─── Data Structures ─────────────────────────────────────── */

typedef struct Reading {
    char station[MAX_STR];   /* station name / ID         */
    char date[MAX_STR];      /* YYYY-MM-DD                */
    float flow_rate;         /* m³/s                      */
    float water_level;       /* metres                    */
    float temperature;       /* °C                        */
    float pollution_index;   /* 0–100 (100 = worst)       */
    struct Reading *next;
} Reading;

typedef struct {
    Reading *head;
    int count;
} Database;

/* ─── Memory helpers ──────────────────────────────────────── */

Reading *new_reading(void) {
    Reading *r = malloc(sizeof(Reading));
    if (!r) { fprintf(stderr, "Out of memory.\n"); exit(1); }
    r->next = NULL;
    return r;
}

void free_db(Database *db) {
    Reading *cur = db->head;
    while (cur) {
        Reading *next = cur->next;
        free(cur);
        cur = next;
    }
    db->head  = NULL;
    db->count = 0;
}

/* ─── CSV Parser ──────────────────────────────────────────── */

/* Returns 1 on success, 0 on failure */
int load_csv(const char *path, Database *db) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open '%s'\n", path);
        return 0;
    }

    char line[MAX_LINE];
    int line_num = 0;
    int skipped  = 0;
    Reading *tail = NULL;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        /* skip header */
        if (line_num == 1) continue;

        /* strip trailing newline */
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        Reading *r = new_reading();
        char *tok;

        tok = strtok(line, ","); if (!tok) { free(r); skipped++; continue; }
        strncpy(r->station, tok, MAX_STR - 1);

        tok = strtok(NULL, ","); if (!tok) { free(r); skipped++; continue; }
        strncpy(r->date, tok, MAX_STR - 1);

        tok = strtok(NULL, ","); if (!tok) { free(r); skipped++; continue; }
        r->flow_rate = atof(tok);

        tok = strtok(NULL, ","); if (!tok) { free(r); skipped++; continue; }
        r->water_level = atof(tok);

        tok = strtok(NULL, ","); if (!tok) { free(r); skipped++; continue; }
        r->temperature = atof(tok);

        tok = strtok(NULL, ","); if (!tok) { free(r); skipped++; continue; }
        r->pollution_index = atof(tok);

        /* append to list */
        if (!db->head) { db->head = r; tail = r; }
        else           { tail->next = r; tail = r; }
        db->count++;
    }

    fclose(fp);

    printf("Loaded %d readings", db->count);
    if (skipped) printf(" (%d malformed rows skipped)", skipped);
    printf(".\n");
    return 1;
}

/* ─── Analysis Functions ──────────────────────────────────── */

void summary_stats(const Database *db) {
    if (!db->count) { printf("No data loaded.\n"); return; }

    float total_flow = 0, total_level = 0, total_temp = 0, total_poll = 0;
    float max_flow = -1e9, min_flow = 1e9;
    float max_poll = -1e9;
    char  max_flow_station[MAX_STR], max_poll_station[MAX_STR];

    for (Reading *r = db->head; r; r = r->next) {
        total_flow  += r->flow_rate;
        total_level += r->water_level;
        total_temp  += r->temperature;
        total_poll  += r->pollution_index;

        if (r->flow_rate > max_flow) {
            max_flow = r->flow_rate;
            strncpy(max_flow_station, r->station, MAX_STR - 1);
        }
        if (r->flow_rate < min_flow) min_flow = r->flow_rate;
        if (r->pollution_index > max_poll) {
            max_poll = r->pollution_index;
            strncpy(max_poll_station, r->station, MAX_STR - 1);
        }
    }

    int n = db->count;
    printf("\n══════════════ SUMMARY STATISTICS ══════════════\n");
    printf("  Total readings      : %d\n", n);
    printf("  Avg flow rate       : %.2f m³/s\n", total_flow  / n);
    printf("  Avg water level     : %.2f m\n",    total_level / n);
    printf("  Avg temperature     : %.2f °C\n",   total_temp  / n);
    printf("  Avg pollution index : %.2f / 100\n",total_poll  / n);
    printf("  Peak flow           : %.2f m³/s  @ %s\n", max_flow,  max_flow_station);
    printf("  Lowest flow         : %.2f m³/s\n", min_flow);
    printf("  Worst pollution     : %.2f / 100 @ %s\n", max_poll, max_poll_station);
    printf("═════════════════════════════════════════════════\n\n");
}

void flood_risk(const Database *db, float level_thresh, float flow_thresh) {
    if (!db->count) { printf("No data loaded.\n"); return; }

    printf("\n══════════════ FLOOD RISK ALERTS ════════════════\n");
    printf("  Threshold — Level: %.2f m  |  Flow: %.2f m³/s\n\n",
           level_thresh, flow_thresh);

    int found = 0;
    for (Reading *r = db->head; r; r = r->next) {
        if (r->water_level >= level_thresh || r->flow_rate >= flow_thresh) {
            printf("  [!] %-20s  %s   Level: %5.2f m   Flow: %7.2f m³/s\n",
                   r->station, r->date, r->water_level, r->flow_rate);
            found++;
        }
    }
    if (!found) printf("  No readings exceed the thresholds. All clear!\n");
    printf("\n  %d alert(s) found.\n", found);
    printf("═════════════════════════════════════════════════\n\n");
}

void pollution_spikes(const Database *db, float threshold) {
    if (!db->count) { printf("No data loaded.\n"); return; }

    printf("\n══════════════ POLLUTION SPIKES ═════════════════\n");
    printf("  Showing readings with pollution index > %.1f\n\n", threshold);

    int found = 0;
    for (Reading *r = db->head; r; r = r->next) {
        if (r->pollution_index > threshold) {
            printf("  [!] %-20s  %s   Pollution: %5.1f   Temp: %.1f°C\n",
                   r->station, r->date, r->pollution_index, r->temperature);
            found++;
        }
    }
    if (!found) printf("  No spikes above %.1f detected.\n", threshold);
    printf("\n  %d spike(s) found.\n", found);
    printf("═════════════════════════════════════════════════\n\n");
}

void station_report(const Database *db, const char *station) {
    printf("\n══════════════ STATION REPORT: %-16s═\n", station);

    int count = 0;
    float total_flow = 0, total_level = 0, total_poll = 0;

    for (Reading *r = db->head; r; r = r->next) {
        if (strcmp(r->station, station) == 0) {
            printf("  %s   Flow: %7.2f m³/s   Level: %5.2f m   "
                   "Temp: %5.1f°C   Pollution: %5.1f\n",
                   r->date, r->flow_rate, r->water_level,
                   r->temperature, r->pollution_index);
            total_flow  += r->flow_rate;
            total_level += r->water_level;
            total_poll  += r->pollution_index;
            count++;
        }
    }

    if (!count) {
        printf("  No readings found for station '%s'.\n", station);
    } else {
        printf("\n  Averages over %d reading(s): Flow %.2f  Level %.2f  Pollution %.1f\n",
               count, total_flow/count, total_level/count, total_poll/count);
    }
    printf("═════════════════════════════════════════════════\n\n");
}

void list_stations(const Database *db) {
    if (!db->count) { printf("No data loaded.\n"); return; }

    /* simple de-dup with a small local array */
    char seen[256][MAX_STR];
    int  seen_n = 0;

    for (Reading *r = db->head; r; r = r->next) {
        int found = 0;
        for (int i = 0; i < seen_n; i++) {
            if (strcmp(seen[i], r->station) == 0) { found = 1; break; }
        }
        if (!found && seen_n < 256) {
            strncpy(seen[seen_n++], r->station, MAX_STR - 1);
        }
    }

    printf("\n  Known stations (%d):\n", seen_n);
    for (int i = 0; i < seen_n; i++)
        printf("    • %s\n", seen[i]);
    printf("\n");
}

/* ─── Export ──────────────────────────────────────────────── */

void export_pollution_report(const Database *db, float threshold, const char *out_path) {
    FILE *fp = fopen(out_path, "w");
    if (!fp) { fprintf(stderr, "Could not create '%s'\n", out_path); return; }

    fprintf(fp, "Pollution Report\n");
    fprintf(fp, "Threshold: %.1f\n", threshold);
    fprintf(fp, "station,date,pollution_index,temperature\n");

    int count = 0;
    for (Reading *r = db->head; r; r = r->next) {
        if (r->pollution_index > threshold) {
            fprintf(fp, "%s,%s,%.2f,%.2f\n",
                    r->station, r->date, r->pollution_index, r->temperature);
            count++;
        }
    }

    fclose(fp);
    printf("Exported %d row(s) to '%s'.\n", count, out_path);
}

/* ─── Menu ────────────────────────────────────────────────── */

void print_menu(void) {
    printf("╔══════════════════════════════════════╗\n");
    printf("║   River & Watershed Data Analyzer    ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  1. Load CSV data file               ║\n");
    printf("║  2. Summary statistics               ║\n");
    printf("║  3. Flood risk alerts                ║\n");
    printf("║  4. Pollution spike detection        ║\n");
    printf("║  5. Station report                   ║\n");
    printf("║  6. List all stations                ║\n");
    printf("║  7. Export pollution report to CSV   ║\n");
    printf("║  8. Quit                             ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("  Choice: ");
}

int main(void) {
    Database db = { NULL, 0 };
    int choice;
    char buf[MAX_LINE];

    printf("\nWelcome to the Water Data Analyzer.\n\n");

    while (1) {
        print_menu();
        if (!fgets(buf, sizeof(buf), stdin)) break;
        choice = atoi(buf);

        switch (choice) {

        case 1: {
            printf("  CSV path: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\r\n")] = '\0';
            free_db(&db);
            load_csv(buf, &db);
            break;
        }

        case 2:
            summary_stats(&db);
            break;

        case 3: {
            float lev, flow;
            printf("  Water level threshold (m)  [default 4.5]: ");
            fgets(buf, sizeof(buf), stdin);
            lev = (atof(buf) > 0) ? atof(buf) : 4.5f;

            printf("  Flow rate threshold (m³/s) [default 800]: ");
            fgets(buf, sizeof(buf), stdin);
            flow = (atof(buf) > 0) ? atof(buf) : 800.0f;

            flood_risk(&db, lev, flow);
            break;
        }

        case 4: {
            printf("  Pollution index threshold [default 60]: ");
            fgets(buf, sizeof(buf), stdin);
            float thresh = (atof(buf) > 0) ? atof(buf) : 60.0f;
            pollution_spikes(&db, thresh);
            break;
        }

        case 5: {
            list_stations(&db);
            printf("  Station name: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\r\n")] = '\0';
            station_report(&db, buf);
            break;
        }

        case 6:
            list_stations(&db);
            break;

        case 7: {
            printf("  Pollution threshold [default 60]: ");
            fgets(buf, sizeof(buf), stdin);
            float thresh = (atof(buf) > 0) ? atof(buf) : 60.0f;
            printf("  Output filename [default: report.csv]: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\r\n")] = '\0';
            if (strlen(buf) == 0) strncpy(buf, "report.csv", sizeof(buf));
            export_pollution_report(&db, thresh, buf);
            break;
        }

        case 8:
            printf("Goodbye. Stay conservancy-minded!\n");
            free_db(&db);
            return 0;

        default:
            printf("  Invalid choice. Try 1–8.\n\n");
        }
    }

    free_db(&db);
    return 0;
}
