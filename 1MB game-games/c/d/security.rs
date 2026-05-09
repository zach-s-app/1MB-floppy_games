#![no_std]
use core::panic::PanicInfo;

// Stable 3-input hash — mixes inputs without needing stdlib
#[inline]
fn hash3(a: i32, b: i32, c: i32) -> i32 {
    let mut h = a as u32;
    h ^= (b as u32).wrapping_mul(2654435761);
    h ^= (c as u32).wrapping_mul(2246822519);
    h  = h.wrapping_mul(3266489917);
    h ^= h >> 16;
    (h & 0x7FFFFFFF) as i32
}

// RPS-style entropy mixer
#[unsafe(no_mangle)]
pub extern "C" fn rps_seed_mix(a: i32, b: i32, salt: i32) -> i32 {
    let outcome = ((a * 3 + b) ^ salt) & 0x7FFFFFFF;
    hash3(outcome, salt, a ^ b)
}

// Loot drop count — seeded per chunk + depth
#[unsafe(no_mangle)]
pub extern "C" fn calculate_loot_chance(global_seed: i32, chunk_x: i32, chunk_y: i32, depth: i32) -> i32 {
    let h = hash3(global_seed ^ chunk_x, chunk_y, depth);
    let chance = h % 100;
    let threshold = (60 - depth * 3).max(15);
    if chance > threshold { (h % 4) + 2 } else { 1 }
}

// Level from XP
#[unsafe(no_mangle)]
pub extern "C" fn calculate_level(xp: i32) -> i32 {
    if      xp < 100  { 1 }
    else if xp < 250  { 2 }
    else if xp < 500  { 3 }
    else if xp < 900  { 4 }
    else if xp < 1400 { 5 }
    else if xp < 2000 { 6 }
    else              { 7 + (xp - 2000) / 800 }
}

// Enemy count per chunk
#[unsafe(no_mangle)]
pub extern "C" fn calculate_enemy_count(global_seed: i32, chunk_x: i32, chunk_y: i32, depth: i32) -> i32 {
    let h = hash3(global_seed ^ chunk_x, chunk_y ^ depth, depth + 1);
    let dist = (chunk_x.abs() + chunk_y.abs()).min(8);
    let base = 1 + dist / 2 + depth;
    let variation = h % 3;
    (base + variation).min(8)
}

// Enemy type — 0=Stalker 1=Crawler 2=Tank 3=Ghost
#[unsafe(no_mangle)]
pub extern "C" fn calculate_enemy_type(global_seed: i32, chunk_x: i32, chunk_y: i32, depth: i32, index: i32) -> i32 {
    if depth == 0 && chunk_x == 0 && chunk_y == 0 { return 0; }
    let h = hash3(global_seed ^ chunk_x ^ index, chunk_y ^ depth, index + 7);
    let roll = h % 100;
    let tank_thresh  = (50 - depth * 5).max(15);
    let ghost_thresh = (75 - depth * 4).max(40);
    if      roll < tank_thresh  { 2 }
    else if roll < ghost_thresh { 3 }
    else if h % 2 == 0          { 1 }
    else                        { 0 }
}

// Enemy HP
#[unsafe(no_mangle)]
pub extern "C" fn calculate_enemy_hp(enemy_type: i32, depth: i32) -> i32 {
    let base = match enemy_type {
        0 => 50,
        1 => 20,
        2 => 120,
        3 => 35,
        _ => 50,
    };
    base + depth * 8
}

// XP value for kill
#[unsafe(no_mangle)]
pub extern "C" fn calculate_enemy_xp(enemy_type: i32, depth: i32) -> i32 {
    let base = match enemy_type {
        0 => 50,
        1 => 25,
        2 => 100,
        3 => 75,
        _ => 50,
    };
    base + depth * 10
}

// Wall brightness via inverse square law — returns 0-255 int
#[unsafe(no_mangle)]
pub extern "C" fn wall_bright(d_scaled: i32) -> i32 {
    let d2 = (d_scaled as i64) * (d_scaled as i64);
    let denom = d2 / 667 + 1;
    let bright = (255 * 1000 / denom) as i32;
    if bright > 255 { 255 } else if bright < 0 { 0 } else { bright }
}

// Entity light level — softer falloff than walls
#[unsafe(no_mangle)]
pub extern "C" fn ent_light(d_scaled: i32) -> i32 {
    let d2 = (d_scaled as i64) * (d_scaled as i64);
    let denom = d2 / 1000 + 1;
    let light = (255 * 1000 / denom) as i32;
    if light > 255 { 255 } else if light < 0 { 0 } else { light }
}

#[panic_handler]
fn panic(_: &PanicInfo) -> ! { loop {} }
