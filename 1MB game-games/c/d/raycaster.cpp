extern "C" {
    // --- Rust imports ---
    extern int calculate_loot_chance(int global_seed, int chunk_x, int chunk_y, int depth);
    extern int calculate_level(int xp);
    extern int calculate_enemy_count(int global_seed, int chunk_x, int chunk_y, int depth);
    extern int calculate_enemy_type(int global_seed, int chunk_x, int chunk_y, int depth, int index);
    extern int calculate_enemy_hp(int enemy_type, int depth);
    extern int calculate_enemy_xp(int enemy_type, int depth);
    extern int wall_bright(int d_scaled);
    extern int ent_light(int d_scaled);
    extern int rps_seed_mix(int a, int b, int salt);

    // --- Config globals (replaces #define — set by JS via set_config_* after XML parse) ---
    // Defaults match shadow_seed.xml plain values
    int   CFG_CHUNK_W          = 10;
    int   CFG_CHUNK_H          = 10;
    int   CFG_GRID_W           = 9;
    int   CFG_GRID_H           = 9;
    int   CFG_MAX_ENTS         = 64;
    int   CFG_MAX_PARTS        = 128;
    int   CFG_WALL_DENSITY     = 5;
    int   CFG_MAX_DRAW_DIST    = 20;
    float CFG_MOVE_SPEED       = 0.065f;
    float CFG_SPRINT_SPEED     = 0.10f;
    float CFG_WALK_DRAIN       = 0.10f;
    float CFG_SPRINT_DRAIN     = 0.35f;
    float CFG_ST_REGEN         = 0.30f;
    float CFG_ST_REGEN_UPG     = 0.08f;
    float CFG_WEAPON_DMG       = 20.0f;
    float CFG_WEAPON_RANGE     = 6.0f;
    float CFG_WEAPON_ANGLE     = 0.35f;
    // Backrooms (from obfuscated section)
    int   CFG_BR_WALL_DENSITY  = 12;
    float CFG_THING_HP_BASE    = 80.0f;
    float CFG_THING_HP_DEPTH   = 12.0f;
    float CFG_THING_DAMAGE     = 0.35f;
    int   CFG_THING_XP         = 150;
    int   CFG_THING_SCRAP      = 5;
    int   CFG_FLICKER_MIN      = 800;
    int   CFG_FLICKER_MAX      = 4000;
    // Wallhack (from obfuscated secrets section)
    int   CFG_WALLHACK_CHANCE  = 1000;
    // Loot
    float CFG_HP_HEAL          = 15.0f;
    int   CFG_SCRAP_PICKUP     = 3;
    int   CFG_SCRAP_TO_COINS   = 5;
    int   CFG_BM_KILL_COINS    = 10;
    int   CFG_NORM_KILL_COINS  = 5;

    // Keep #define for array sizes — must be compile-time constants
    #define CHUNK_W   10
    #define CHUNK_H   10
    #define GRID_W    9
    #define GRID_H    9
    #define MAX_ENTS  64
    #define MAX_PARTS 128

    // --- Wallhack easter egg --- declared early so set_config_bool can see it
    int wallhack_enabled = 0;
    unsigned int wallhack_rng = 1u;

    // ---------------------------------------------------------------
    // CONFIG SETTERS — called by JS after parsing shadow_seed.xml
    // Key is a simple djb2 hash of the ini key name
    // ---------------------------------------------------------------
    static unsigned int cfg_hash(const char* s) {
        unsigned int h = 5381;
        while (*s) h = ((h << 5) + h) ^ (unsigned char)*s++;
        return h;
    }

    __attribute__((visibility("default")))
    void set_config_int(unsigned int key_hash, int val) {
        // World
        if (key_hash == 0x6b9b0a3bu) { CFG_WALL_DENSITY    = val; return; } // wall_density
        if (key_hash == 0x2e4b3d1fu) { CFG_MAX_DRAW_DIST   = val; return; } // max_draw_dist
        if (key_hash == 0x7c9f2a8eu) { CFG_MAX_ENTS         = val; return; } // max_entities
        if (key_hash == 0x3f8a1c4du) { CFG_MAX_PARTS        = val; return; } // max_particles
        // Backrooms (from base64 section)
        if (key_hash == 0x9d3e7f2cu) { CFG_BR_WALL_DENSITY  = val; return; } // backrooms_wall_density
        if (key_hash == 0x4a7b9e1fu) { CFG_THING_XP         = val; return; } // thing_xp
        if (key_hash == 0x2c8f4a3bu) { CFG_THING_SCRAP      = val; return; } // thing_scrap
        if (key_hash == 0x6e3d8c2au) { CFG_FLICKER_MIN      = val; return; } // flicker_min
        if (key_hash == 0x5f2a7b1eu) { CFG_FLICKER_MAX      = val; return; } // flicker_max
        // Secrets (wallhack)
        if (key_hash == 0x8b4e9f3du) { CFG_WALLHACK_CHANCE  = val; return; } // wallhack_chance
        // Loot
        if (key_hash == 0x3d9a2f1cu) { CFG_SCRAP_PICKUP     = val; return; } // scrap_pickup
        if (key_hash == 0x7f4b8e2du) { CFG_SCRAP_TO_COINS   = val; return; } // scrap_to_coins
        if (key_hash == 0x9e2c5a4bu) { CFG_BM_KILL_COINS    = val; return; } // blood_moon_kill_coins
        if (key_hash == 0x4c7f3b9eu) { CFG_NORM_KILL_COINS  = val; return; } // normal_kill_coins
    }

    __attribute__((visibility("default")))
    void set_config_float(unsigned int key_hash, float val) {
        if (key_hash == 0x5e8a3f2cu) { CFG_MOVE_SPEED       = val; return; } // move_speed
        if (key_hash == 0x7b4d9e3fu) { CFG_SPRINT_SPEED     = val; return; } // sprint_speed
        if (key_hash == 0x3f2a8c1eu) { CFG_WALK_DRAIN       = val; return; } // walk_drain
        if (key_hash == 0x8c4f7b2du) { CFG_SPRINT_DRAIN     = val; return; } // sprint_drain
        if (key_hash == 0x2d9e4a3cu) { CFG_ST_REGEN         = val; return; } // stamina_regen
        if (key_hash == 0x6f3b8e1fu) { CFG_ST_REGEN_UPG     = val; return; } // stamina_regen_per_upgrade
        if (key_hash == 0x4e7c2a9fu) { CFG_WEAPON_DMG       = val; return; } // weapon_damage
        if (key_hash == 0x9f3d5b2eu) { CFG_WEAPON_RANGE     = val; return; } // weapon_range
        if (key_hash == 0x1e8f4c3au) { CFG_WEAPON_ANGLE     = val; return; } // weapon_angle
        // Backrooms
        if (key_hash == 0x7a4e2d9cu) { CFG_THING_HP_BASE    = val; return; } // thing_hp_base
        if (key_hash == 0x3c9f1b7eu) { CFG_THING_HP_DEPTH   = val; return; } // thing_hp_per_depth
        if (key_hash == 0x8e2b6f4au) { CFG_THING_DAMAGE     = val; return; } // thing_damage
        if (key_hash == 0x5b3a9e2fu) { CFG_HP_HEAL          = val; return; } // hp_heal
    }

    __attribute__((visibility("default")))
    void set_config_bool(unsigned int key_hash, int val) {
        // wallhack_enabled — only the obfuscated section sets this
        if (key_hash == 0x6d4f8b3eu) { wallhack_enabled = val; return; } // wallhack_enabled
        // no_loot — backrooms has no loot (JS checks this)
        // handled JS-side
    }

    // --- Tile storage ---
    int tiles[GRID_W * GRID_H * CHUNK_W * CHUNK_H];
    int grid_cx[GRID_W * GRID_H];
    int grid_cy[GRID_W * GRID_H];
    int grid_valid[GRID_W * GRID_H];

    int player_chunk_x = 0, player_chunk_y = 0;
    int current_depth  = 0;
    int global_seed    = 1;

    // --- Backrooms state ---
    // Saved normal world state for when player exits backrooms
    int   saved_depth  = 0;
    int   saved_seed   = 1;
    int   saved_pcx    = 0, saved_pcy = 0;
    int   in_backrooms = 0; // 0 = normal world, 1 = backrooms

    // --- Entities ---
    // type 0=Stalker 1=Crawler 2=Tank 3=Ghost 4=BlueDoor
    // type 5=BrownDoor(backrooms exit) 6=TheThing(backrooms enemy)
    struct Entity {
        float x, y, vx, vy;
        int   type;
        float hp, mhp;
        int   active;
        int   chunk_gx, chunk_gy;
    };
    Entity ents[MAX_ENTS];
    int ent_count = 0;

    // --- Particles ---
    struct Particle {
        float x, y, vx, vy;
        int   life, maxlife;
        int   color_r, color_g, color_b;
        int   active;
    };
    Particle parts[MAX_PARTS];

    // --- Player stats ---
    float p_hp = 100.0f, p_mh = 100.0f, p_st = 100.0f;
    int   p_xp = 0, p_scrap = 0;
    int   st_upgrade = 0;

    // --- Spawn result floats ---
    float spawn_result_x = 1.5f;
    float spawn_result_y = 1.5f;

    // ---------------------------------------------------------------
    // INTERNAL HELPERS
    // ---------------------------------------------------------------
    static int floor_div(int a, int b) {
        return a / b - (a % b != 0 && (a ^ b) < 0);
    }

    static int ifloor(float f) {
        int i = (int)f;
        return (f < 0.0f && f != (float)i) ? i - 1 : i;
    }

    static unsigned int chunk_hash(int gx, int gy, int depth, int seed) {
        unsigned int h = (unsigned int)(gx * 73856093 ^ gy * 19349663
                                      ^ depth * 83492791 ^ seed);
        h ^= h >> 16;
        h *= 0x45d9f3b;
        h ^= h >> 16;
        return h | 1u;
    }

    static int tile_to_chunk(int t) { return floor_div(t, CHUNK_W); }
    static int tile_local(int t)    { int r = t % CHUNK_W; return r < 0 ? r + CHUNK_W : r; }

    static int get_tile_wc(int wtx, int wty) {
        int cx = tile_to_chunk(wtx), cy = tile_to_chunk(wty);
        int lx = tile_local(wtx),    ly = tile_local(wty);
        for (int i = 0; i < GRID_W * GRID_H; i++) {
            if (grid_valid[i] && grid_cx[i] == cx && grid_cy[i] == cy)
                return tiles[i * CHUNK_W * CHUNK_H + ly * CHUNK_W + lx];
        }
        return 1;
    }

    // ---------------------------------------------------------------
    // NORMAL CHUNK GENERATION
    // ---------------------------------------------------------------
    static void gen_chunk(int slot, int cx, int cy, int depth, int seed) {
        if (seed == 0) seed = 1;
        grid_cx[slot]    = cx;
        grid_cy[slot]    = cy;
        grid_valid[slot] = 1;
        int base = slot * CHUNK_W * CHUNK_H;

        unsigned int next = chunk_hash(cx, cy, depth, seed);
        for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            next = next * 1103515245u + 12345u;
            int lx = i % CHUNK_W, ly = i / CHUNK_W;
            if (lx == 0 || lx == CHUNK_W-1 || ly == 0 || ly == CHUNK_H-1)
                tiles[base + i] = 1;
            else
                tiles[base + i] = (next % (unsigned)CFG_WALL_DENSITY == 0) ? 1 : 0;
        }

        // Guaranteed 2-wide doors on all 4 edges
        int mx = CHUNK_W / 2, my = CHUNK_H / 2;
        tiles[base + mx]                           = 0;
        tiles[base + mx - 1]                       = 0;
        tiles[base + (CHUNK_H-1)*CHUNK_W + mx]     = 0;
        tiles[base + (CHUNK_H-1)*CHUNK_W + mx - 1] = 0;
        tiles[base + my*CHUNK_W]                   = 0;
        tiles[base + (my-1)*CHUNK_W]               = 0;
        tiles[base + my*CHUNK_W + CHUNK_W-1]       = 0;
        tiles[base + (my-1)*CHUNK_W + CHUNK_W-1]   = 0;

        // Spawn clear at origin
        if (cx == 0 && cy == 0 && depth == 0) {
            for (int dy = 1; dy <= 3; dy++)
                for (int dx = 1; dx <= 3; dx++)
                    tiles[base + dy*CHUNK_W + dx] = 0;
        }

        // Blue door
        int bdx = (int)(chunk_hash(cx, cy, depth+1, seed) % (unsigned)(CHUNK_W-2)) + 1;
        int bdy = (int)(chunk_hash(cx+1, cy, depth,  seed) % (unsigned)(CHUNK_H-2)) + 1;
        tiles[base + bdy*CHUNK_W + bdx] = 0;

        // Enemies
        int ecount = calculate_enemy_count(seed, cx, cy, depth);
        for (int e = 0; e < ecount && ent_count < MAX_ENTS; e++) {
            unsigned int eh = chunk_hash(cx ^ e, cy ^ (e+1), depth, seed + e);
            int   etype = calculate_enemy_type(seed, cx, cy, depth, e);
            int   ehp   = calculate_enemy_hp(etype, depth);
            float ex    = (float)(cx * CHUNK_W) + (float)(eh % (unsigned)(CHUNK_W-2)) + 1.5f;
            float ey    = (float)(cy * CHUNK_H) + (float)((eh >> 4) % (unsigned)(CHUNK_H-2)) + 1.5f;
            ents[ent_count++] = {ex, ey, 0,0, etype, (float)ehp,(float)ehp, 1,
                                 slot % GRID_W, slot / GRID_W};
        }

        // Blue door entity
        if (ent_count < MAX_ENTS) {
            ents[ent_count++] = {
                (float)(cx * CHUNK_W + bdx) + 0.5f,
                (float)(cy * CHUNK_H + bdy) + 0.5f,
                0,0, 4, 1.0f,1.0f, 1,
                slot % GRID_W, slot / GRID_W
            };
        }
    }

    // ---------------------------------------------------------------
    // BACKROOMS CHUNK GENERATION
    // Very different feel:
    // - Much more open (walls sparse, only ~8% vs 20%)
    // - No border walls — infinite liminal corridors
    // - No blue door — instead a brown door (type 5)
    // - Only The Thing (type 6) spawns here
    // - Deterministic but using a scrambled seed so it never matches
    //   the normal world layout
    // ---------------------------------------------------------------
    static void gen_chunk_backrooms(int slot, int cx, int cy, int seed) {
        if (seed == 0) seed = 1;
        grid_cx[slot]    = cx;
        grid_cy[slot]    = cy;
        grid_valid[slot] = 1;
        int base = slot * CHUNK_W * CHUNK_H;

        // Scramble seed so backrooms never looks like normal world
        unsigned int bseed = chunk_hash(cx ^ 0xDEAD, cy ^ 0xBEEF, 99, seed);

        unsigned int next = bseed;
        for (int i = 0; i < CHUNK_W * CHUNK_H; i++) {
            next = next * 1103515245u + 12345u;
            int lx = i % CHUNK_W, ly = i / CHUNK_W;
            // No forced border walls — backrooms bleeds together
            // Only ~8% internal walls for that sparse liminal feel
            tiles[base + i] = (next % (unsigned)CFG_BR_WALL_DENSITY == 0) ? 1 : 0;
        }

        // Brown door — one per chunk, deterministic position
        int ddx = (int)(chunk_hash(cx ^ 0xFF, cy, 77, seed) % (unsigned)(CHUNK_W-2)) + 1;
        int ddy = (int)(chunk_hash(cx, cy ^ 0xFF, 77, seed) % (unsigned)(CHUNK_H-2)) + 1;
        tiles[base + ddy*CHUNK_W + ddx] = 0;

        // Brown door entity (type 5)
        if (ent_count < MAX_ENTS) {
            ents[ent_count++] = {
                (float)(cx * CHUNK_W + ddx) + 0.5f,
                (float)(cy * CHUNK_H + ddy) + 0.5f,
                0,0, 5, 1.0f,1.0f, 1,
                slot % GRID_W, slot / GRID_W
            };
        }

        // The Thing (type 6) — 1 to 3 per chunk, always present
        unsigned int tc = chunk_hash(cx ^ 0xCAFE, cy, 42, seed);
        int tcount = (int)(tc % 3) + 1;
        for (int t = 0; t < tcount && ent_count < MAX_ENTS; t++) {
            unsigned int th = chunk_hash(cx ^ t, cy ^ (t*7), 42, seed + t);
            float tx2 = (float)(cx * CHUNK_W) + (float)(th % (unsigned)(CHUNK_W-2)) + 1.5f;
            float ty2 = (float)(cy * CHUNK_H) + (float)((th >> 5) % (unsigned)(CHUNK_H-2)) + 1.5f;
            // The Thing has a lot of HP and does heavy damage — it belongs here
            float thp = CFG_THING_HP_BASE + (float)(current_depth * (int)CFG_THING_HP_DEPTH);
            ents[ent_count++] = {tx2, ty2, 0,0, 6, thp, thp, 1,
                                 slot % GRID_W, slot / GRID_W};
        }
    }

    // ---------------------------------------------------------------
    // SAFE SPAWN
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    void find_safe_spawn(float wx, float wy) {
        int tx = ifloor(wx), ty = ifloor(wy);
        for (int r = 0; r < 12; r++) {
            for (int dy = -r; dy <= r; dy++) {
                for (int dx = -r; dx <= r; dx++) {
                    if (__builtin_abs(dx) != r && __builtin_abs(dy) != r) continue;
                    int cx = tx + dx, cy = ty + dy;
                    if (get_tile_wc(cx, cy) == 0) {
                        spawn_result_x = (float)cx + 0.5f;
                        spawn_result_y = (float)cy + 0.5f;
                        return;
                    }
                }
            }
        }
        spawn_result_x = wx;
        spawn_result_y = wy;
    }

    __attribute__((visibility("default"))) float get_spawn_x() { return spawn_result_x; }
    __attribute__((visibility("default"))) float get_spawn_y() { return spawn_result_y; }

    // ---------------------------------------------------------------
    // WORLD INIT
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    void init_world(int seed, int depth) {
        // Bad seed (0 or negative from JS input) = wallhack easter egg enabled
        int bad_seed = (seed <= 0);
        global_seed    = (seed <= 0) ? 1 : seed;
        current_depth  = depth;
        ent_count      = 0;
        player_chunk_x = 0;
        player_chunk_y = 0;
        in_backrooms   = 0;
        wallhack_enabled = bad_seed ? 1 : 0;
        wallhack_rng   = (unsigned int)(global_seed ^ 0xDEADF00D) | 1u;
        for (int i = 0; i < GRID_W * GRID_H; i++) grid_valid[i] = 0;
        int half = GRID_W / 2;
        for (int gy = 0; gy < GRID_H; gy++)
            for (int gx = 0; gx < GRID_W; gx++)
                gen_chunk(gy*GRID_W+gx, gx-half, gy-half, depth, global_seed);
    }

    // ---------------------------------------------------------------
    // ENTER BACKROOMS
    // Saves normal world state, regenerates grid with backrooms chunks
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    void enter_backrooms() {
        if (in_backrooms) return;
        // Save normal world state
        saved_depth = current_depth;
        saved_seed  = global_seed;
        saved_pcx   = player_chunk_x;
        saved_pcy   = player_chunk_y;
        in_backrooms = 1;

        // Backrooms uses a totally different seed derived from current
        // so it never looks like the normal world
        global_seed = (int)((unsigned int)(global_seed * 2246822519u ^ 0xBACC0000u) | 1u);

        ent_count = 0;
        int half = GRID_W / 2;
        for (int gy = 0; gy < GRID_H; gy++)
            for (int gx = 0; gx < GRID_W; gx++)
                gen_chunk_backrooms(gy*GRID_W+gx,
                                    gx-half+player_chunk_x,
                                    gy-half+player_chunk_y,
                                    global_seed);
    }

    // ---------------------------------------------------------------
    // EXIT BACKROOMS via brown door
    // Sends player to a random depth (gamble) — ±3 from saved depth
    // Uses the brown door's chunk position as entropy for the gamble
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    int exit_backrooms_depth() {
        // Returns the target depth the brown door exits to
        // Caller uses this to set up the normal world at that depth
        unsigned int h = chunk_hash(player_chunk_x, player_chunk_y,
                                    saved_depth, saved_seed);
        int delta = (int)(h % 7) - 3; // -3 to +3
        int target = saved_depth + delta;
        if (target < 0) target = 0;
        return target;
    }

    __attribute__((visibility("default")))
    void exit_backrooms(int target_depth) {
        in_backrooms   = 0;
        current_depth  = target_depth;
        // Restore global seed mixed with depth for new feel
        global_seed = (int)((unsigned int)(saved_seed ^ (target_depth * 2246822519u)) | 1u);
        player_chunk_x = saved_pcx;
        player_chunk_y = saved_pcy;

        ent_count = 0;
        int half = GRID_W / 2;
        for (int gy = 0; gy < GRID_H; gy++)
            for (int gx = 0; gx < GRID_W; gx++)
                gen_chunk(gy*GRID_W+gx,
                          gx-half+player_chunk_x,
                          gy-half+player_chunk_y,
                          current_depth, global_seed);
    }

    // ---------------------------------------------------------------
    // GRID SHIFT — works for both normal and backrooms
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    void shift_grid(int dir) {
        #define COPY_SLOT(dst, src) \
            grid_cx[dst]=grid_cx[src]; grid_cy[dst]=grid_cy[src]; \
            grid_valid[dst]=grid_valid[src]; \
            for(int _t=0;_t<CHUNK_W*CHUNK_H;_t++) \
                tiles[(dst)*CHUNK_W*CHUNK_H+_t]=tiles[(src)*CHUNK_W*CHUNK_H+_t];

        #define GEN_NEW(slot, cx, cy) \
            grid_valid[slot]=0; \
            if (in_backrooms) gen_chunk_backrooms(slot, cx, cy, global_seed); \
            else gen_chunk(slot, cx, cy, current_depth, global_seed);

        if (dir == 1) { // south
            player_chunk_y++;
            for(int i=0;i<ent_count;i++) if(ents[i].chunk_gy==0) ents[i].active=0;
            for(int gy=0;gy<GRID_H-1;gy++)
                for(int gx=0;gx<GRID_W;gx++) {
                    int d=gy*GRID_W+gx, s=(gy+1)*GRID_W+gx;
                    COPY_SLOT(d,s);
                    for(int e=0;e<ent_count;e++) if(ents[e].chunk_gy==gy+1) ents[e].chunk_gy=gy;
                }
            for(int gx=0;gx<GRID_W;gx++) {
                int slot=(GRID_H-1)*GRID_W+gx;
                int new_cy=grid_cy[(GRID_H-2)*GRID_W+gx]+1;
                int col_cx=grid_cx[(GRID_H-2)*GRID_W+gx];
                GEN_NEW(slot, col_cx, new_cy);
            }
        } else if (dir == 0) { // north
            player_chunk_y--;
            for(int i=0;i<ent_count;i++) if(ents[i].chunk_gy==GRID_H-1) ents[i].active=0;
            for(int gy=GRID_H-1;gy>0;gy--)
                for(int gx=0;gx<GRID_W;gx++) {
                    int d=gy*GRID_W+gx, s=(gy-1)*GRID_W+gx;
                    COPY_SLOT(d,s);
                    for(int e=0;e<ent_count;e++) if(ents[e].chunk_gy==gy-1) ents[e].chunk_gy=gy;
                }
            for(int gx=0;gx<GRID_W;gx++) {
                int slot=gx;
                int new_cy=grid_cy[1*GRID_W+gx]-1;
                int col_cx=grid_cx[1*GRID_W+gx];
                GEN_NEW(slot, col_cx, new_cy);
            }
        } else if (dir == 3) { // east
            player_chunk_x++;
            for(int i=0;i<ent_count;i++) if(ents[i].chunk_gx==0) ents[i].active=0;
            for(int gy=0;gy<GRID_H;gy++)
                for(int gx=0;gx<GRID_W-1;gx++) {
                    int d=gy*GRID_W+gx, s=gy*GRID_W+gx+1;
                    COPY_SLOT(d,s);
                    for(int e=0;e<ent_count;e++) if(ents[e].chunk_gx==gx+1) ents[e].chunk_gx=gx;
                }
            for(int gy=0;gy<GRID_H;gy++) {
                int slot=gy*GRID_W+(GRID_W-1);
                int new_cx=grid_cx[gy*GRID_W+(GRID_W-2)]+1;
                int row_cy=grid_cy[gy*GRID_W+(GRID_W-2)];
                GEN_NEW(slot, new_cx, row_cy);
            }
        } else if (dir == 2) { // west
            player_chunk_x--;
            for(int i=0;i<ent_count;i++) if(ents[i].chunk_gx==GRID_W-1) ents[i].active=0;
            for(int gy=0;gy<GRID_H;gy++)
                for(int gx=GRID_W-1;gx>0;gx--) {
                    int d=gy*GRID_W+gx, s=gy*GRID_W+gx-1;
                    COPY_SLOT(d,s);
                    for(int e=0;e<ent_count;e++) if(ents[e].chunk_gx==gx-1) ents[e].chunk_gx=gx;
                }
            for(int gy=0;gy<GRID_H;gy++) {
                int slot=gy*GRID_W+0;
                int new_cx=grid_cx[gy*GRID_W+1]-1;
                int row_cy=grid_cy[gy*GRID_W+1];
                GEN_NEW(slot, new_cx, row_cy);
            }
        }

        #undef COPY_SLOT
        #undef GEN_NEW

        int w=0;
        for(int i=0;i<ent_count;i++) if(ents[i].active) ents[w++]=ents[i];
        ent_count=w;
    }

    // ---------------------------------------------------------------
    // GO DEEPER (normal world only)
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    void go_deeper() {
        current_depth++;
        global_seed = (int)((unsigned int)(global_seed ^ (current_depth * 2246822519u)) | 1u);
        ent_count = 0;
        int half = GRID_W / 2;
        for(int gy=0;gy<GRID_H;gy++)
            for(int gx=0;gx<GRID_W;gx++)
                gen_chunk(gy*GRID_W+gx,
                          gx-half+player_chunk_x,
                          gy-half+player_chunk_y,
                          current_depth, global_seed);
    }

    // ---------------------------------------------------------------
    // UPDATE LOGIC
    // Backrooms: no noclip — solid walls enforced
    // The Thing (type 6): slow, relentless, walks through walls
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    void update_logic(float px, float py, int mov, int sprint) {
        if (mov && p_st > 0.0f) {
            p_st -= sprint ? CFG_SPRINT_DRAIN : CFG_WALK_DRAIN;
        } else if (p_st < 100.0f) {
            p_st += CFG_ST_REGEN + st_upgrade * CFG_ST_REGEN_UPG;
        }
        if (p_st < 0.0f)   p_st = 0.0f;
        if (p_st > 100.0f) p_st = 100.0f;

        // Skip type 5 (brown door) in update
        for (int i = 0; i < ent_count; i++) {
            Entity* e = &ents[i];
            if (!e->active || e->type == 4 || e->type == 5) continue;

            float dx = px - e->x, dy = py - e->y;
            float d  = __builtin_sqrtf(dx*dx + dy*dy);

            switch (e->type) {
                case 0: if(d<7.0f){e->vx+=dx*0.0012f;e->vy+=dy*0.0012f;} e->vx*=0.90f;e->vy*=0.90f; break;
                case 1: if(d<9.0f){e->vx+=dx*0.003f; e->vy+=dy*0.003f; } e->vx*=0.85f;e->vy*=0.85f; break;
                case 2: if(d<5.0f){e->vx+=dx*0.0006f;e->vy+=dy*0.0006f;} e->vx*=0.95f;e->vy*=0.95f; break;
                case 3: if(d<8.0f){e->vx+=dx*0.0015f;e->vy+=dy*0.0015f;} e->vx*=0.88f;e->vy*=0.88f; break;
                case 6: // The Thing — slow, always tracks, always phases walls
                    if(d<12.0f){e->vx+=dx*0.0008f;e->vy+=dy*0.0008f;}
                    e->vx*=0.92f;e->vy*=0.92f;
                    break;
            }

            float nx = e->x + e->vx, ny = e->y + e->vy;

            if (e->type == 6) {
                // The Thing always moves — it phases through everything
                e->x = nx; e->y = ny;
            } else {
                int phases = (e->type == 3) && ((int)(d * 137.0f) % 5 == 0);
                if (phases || get_tile_wc(ifloor(nx), ifloor(ny)) == 0) {
                    e->x = nx; e->y = ny;
                } else {
                    if      (get_tile_wc(ifloor(nx), ifloor(e->y)) == 0) { e->x=nx; e->vx*=0.5f; }
                    else if (get_tile_wc(ifloor(e->x), ifloor(ny)) == 0) { e->y=ny; e->vy*=0.5f; }
                    else { e->vx*=-0.4f; e->vy*=-0.4f; }
                }
            }

            // Damage
            float hr = (e->type == 2) ? 0.55f : (e->type == 6) ? 0.45f : 0.38f;
            if (d < hr) {
                // The Thing does serious damage
                float dmg = (e->type == 6) ? CFG_THING_DAMAGE
                          : (e->type == 2) ? 0.20f
                          : (e->type == 1) ? 0.07f : 0.11f;
                p_hp -= dmg;
            }
        }
        if (p_hp < 0.0f) p_hp = 0.0f;
    }

    // ---------------------------------------------------------------
    // FIRE WEAPON
    // Can shoot The Thing — takes double shots to confirm a hit
    // (it keeps coming regardless, but it can be hurt)
    // Can't shoot type 5 brown door
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    int fire_weapon(float px, float py, float pa) {
        for (int i = 0; i < ent_count; i++) {
            Entity* e = &ents[i];
            if (!e->active || e->type == 4 || e->type == 5) continue;
            float dx = e->x - px, dy = e->y - py;
            float ang = __builtin_atan2f(dy, dx) - pa;
            while (ang < -3.14159f) ang += 6.28318f;
            while (ang >  3.14159f) ang -= 6.28318f;
            float dist = __builtin_sqrtf(dx*dx + dy*dy);
            if (__builtin_fabsf(ang) < CFG_WEAPON_ANGLE && dist < CFG_WEAPON_RANGE) {
                e->hp -= CFG_WEAPON_DMG;
                int spawned = 0;
                for (int p = 0; p < MAX_PARTS && spawned < 6; p++) {
                    if (parts[p].active) continue;
                    unsigned int ph = chunk_hash((int)(e->x*100),(int)(e->y*100),p,global_seed);
                    // The Thing particles are black/dark — wrong feeling
                    int cr=(e->type==6)?20 :(e->type==2)?255:(e->type==3)?120:255;
                    int cg=(e->type==6)?20 :(e->type==2)?0  :(e->type==3)?0  :60;
                    int cb=(e->type==6)?20 :(e->type==3)?255:0;
                    float pvx=((float)(ph%200)/100.0f-1.0f)*0.12f;
                    float pvy=((float)((ph>>8)%200)/100.0f-1.0f)*0.12f;
                    parts[p]={e->x,e->y,pvx,pvy,18,18,cr,cg,cb,1};
                    spawned++;
                }
                if (e->hp <= 0.0f) {
                    e->active = 0;
                    p_xp    += (e->type == 6) ? CFG_THING_XP   : calculate_enemy_xp(e->type, current_depth);
                    p_scrap += (e->type == 6) ? CFG_THING_SCRAP : (e->type == 2) ? 3 : 1;
                }
                return 1;
            }
        }
        return 0;
    }

    // ---------------------------------------------------------------
    // DDA RAYCAST — works for both normal and backrooms
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    float cast_ray(float px, float py, float angle) {
        float ray_dx = __builtin_cosf(angle);
        float ray_dy = __builtin_sinf(angle);
        int map_x = ifloor(px), map_y = ifloor(py);
        int step_x = (ray_dx >= 0.0f) ? 1 : -1;
        int step_y = (ray_dy >= 0.0f) ? 1 : -1;
        float abs_dx = (ray_dx < 0.0f) ? -ray_dx : ray_dx;
        float abs_dy = (ray_dy < 0.0f) ? -ray_dy : ray_dy;
        float delta_x = (abs_dx < 0.00001f) ? 1e30f : 1.0f / abs_dx;
        float delta_y = (abs_dy < 0.00001f) ? 1e30f : 1.0f / abs_dy;
        float side_x = (step_x > 0) ? ((float)(map_x+1)-px)*delta_x : (px-(float)map_x)*delta_x;
        float side_y = (step_y > 0) ? ((float)(map_y+1)-py)*delta_y : (py-(float)map_y)*delta_y;
        float dist = 0.0f;
        for (int step = 0; step < 400; step++) {
            if (side_x < side_y) { dist=side_x; side_x+=delta_x; map_x+=step_x; }
            else                  { dist=side_y; side_y+=delta_y; map_y+=step_y; }
            if (dist > (float)CFG_MAX_DRAW_DIST) return (float)CFG_MAX_DRAW_DIST;
            if (get_tile_wc(map_x, map_y) == 1) return dist;
        }
        return 20.0f;
    }

    __attribute__((visibility("default"))) int ray_brightness(float d) { return wall_bright((int)(d*100.0f)); }
    __attribute__((visibility("default"))) int entity_light(float d)   { return ent_light((int)(d*100.0f)); }

    // ---------------------------------------------------------------
    // GETTERS
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    float get_p_stat(int id) {
        switch(id) {
            case 0: return p_hp;
            case 1: return p_st;
            case 3: return (float)calculate_level(p_xp);
            case 4: return p_mh;
            case 5: return (float)p_scrap;
            case 6: return (float)p_xp;
        }
        return p_mh;
    }

    __attribute__((visibility("default")))
    float get_ent_f(int i, int prop) {
        if (i < 0 || i >= ent_count) return -1.0f;
        switch(prop) {
            case 0: return ents[i].x;
            case 1: return ents[i].y;
            case 2: return (float)ents[i].type;
            case 3: return (float)ents[i].active;
            case 4: return ents[i].hp;
            case 5: return ents[i].mhp;
        }
        return 0.0f;
    }

    __attribute__((visibility("default"))) int   get_ent_count()            { return ent_count; }

    // Wallhack-aware tile lookup for JS collision
    // In backrooms wallhack is always on (you're already outside reality)
    // In normal world: if wallhack_enabled, 1/1000 chance a wall returns 0
    __attribute__((visibility("default")))
    int get_tile_world(int wtx, int wty) {
        int t = get_tile_wc(wtx, wty);
        if (t == 1 && (wallhack_enabled || in_backrooms)) {
            // Roll the wallhack RNG — fast LCG
            wallhack_rng = wallhack_rng * 1664525u + 1013904223u;
            // 1/1000 chance — backrooms always phases (already in walls is fine there)
            if (in_backrooms) return 0; // full phase in backrooms
            if (wallhack_rng % (unsigned)CFG_WALLHACK_CHANCE == 0) return 0; // lucky phase
        }
        return t;
    }

    __attribute__((visibility("default"))) int   get_tile_world_raw(int x,int y){ return get_tile_wc(x,y); }
    __attribute__((visibility("default"))) int   get_current_depth()         { return current_depth; }
    __attribute__((visibility("default"))) int   get_player_chunk_x()        { return player_chunk_x; }
    __attribute__((visibility("default"))) int   get_player_chunk_y()        { return player_chunk_y; }
    __attribute__((visibility("default"))) int   get_in_backrooms()          { return in_backrooms; }
    __attribute__((visibility("default"))) int   get_wallhack()              { return wallhack_enabled; }

    // Toggle wallhack — callable from JS dev console or internal logic
    __attribute__((visibility("default")))
    void set_wallhack(int val) {
        wallhack_enabled = val;
        // Re-seed the wallhack RNG from current global seed so rolls are fresh
        wallhack_rng = (unsigned int)(global_seed ^ 0xDEADF00D) | 1u;
    }

    // Called by JS after player spawns following a death
    // Checks if player is near spawn point AND rolls 1/1000
    // Returns 1 if wallhack should be enabled this life
    __attribute__((visibility("default")))
    int roll_spawn_wallhack(float px, float py) {
        // Spawn point for current chunk is always near (chunkX*10+1.5, chunkY*10+1.5)
        float sx = (float)(player_chunk_x * CHUNK_W) + 1.5f;
        float sy = (float)(player_chunk_y * CHUNK_H) + 1.5f;
        float dx = px - sx, dy = py - sy;
        // Must be near spawn point (within 2 tiles)
        if (dx*dx + dy*dy > 4.0f) return 0;
        // Roll 1/1000
        wallhack_rng = wallhack_rng * 1664525u + 1013904223u;
        if (wallhack_rng % (unsigned)CFG_WALLHACK_CHANCE == 0) {
            wallhack_enabled = 1;
            return 1;
        }
        return 0;
    }

    // ---------------------------------------------------------------
    // PARTICLES
    // ---------------------------------------------------------------
    __attribute__((visibility("default")))
    float get_part_f(int i, int prop) {
        if (i < 0 || i >= MAX_PARTS) return 0.0f;
        switch(prop) {
            case 0: return parts[i].x;   case 1: return parts[i].y;
            case 2: return (float)parts[i].life; case 3: return (float)parts[i].maxlife;
            case 4: return (float)parts[i].color_r; case 5: return (float)parts[i].color_g;
            case 6: return (float)parts[i].color_b; case 7: return (float)parts[i].active;
        }
        return 0.0f;
    }
    __attribute__((visibility("default"))) int get_part_count() { return MAX_PARTS; }

    __attribute__((visibility("default")))
    void update_particles() {
        for (int i = 0; i < MAX_PARTS; i++) {
            if (!parts[i].active) continue;
            parts[i].x += parts[i].vx; parts[i].y += parts[i].vy;
            parts[i].vx *= 0.90f;      parts[i].vy *= 0.90f;
            if (--parts[i].life <= 0) parts[i].active = 0;
        }
    }

    // ---------------------------------------------------------------
    // UPGRADES / MUTATORS
    // ---------------------------------------------------------------
    __attribute__((visibility("default"))) void buy_upgrade(int type) {
        if (type == 0) { p_mh += 20.0f; p_hp = p_mh; }
        if (type == 1) { st_upgrade++; p_st = 100.0f; }
    }
    __attribute__((visibility("default"))) void set_st_upgrade(int val) { st_upgrade = val; }
    __attribute__((visibility("default"))) void add_scrap(int delta)    { p_scrap += delta; if(p_scrap<0) p_scrap=0; }
    __attribute__((visibility("default"))) void heal_player(float amt)  { p_hp += amt; if(p_hp>p_mh) p_hp=p_mh; }
    __attribute__((visibility("default"))) int  mix_seed_rps(int a,int b,int salt) { return rps_seed_mix(a,b,salt); }
}
