#!/bin/bash
# build.sh — reads shadow_seed.xml for all compile flags
# Usage: ./build.sh
# Requires: rustc, clang++, xmllint (or falls back to grep/sed)

set -e

CFG="shadow_seed.xml"

if [ ! -f "$CFG" ]; then
    echo "ERROR: $CFG not found"
    exit 1
fi

echo "==> Reading build config from $CFG"

# ------------------------------------------------------------
# Parse a value from an ini line inside a named XML section
# Usage: xml_ini_get <section> <key>
# Falls back to grep+sed if xmllint not available
# ------------------------------------------------------------
xml_ini_get() {
    local section="$1"
    local key="$2"
    # Extract the raw text content of <section>...</section>
    # then grep for key=value
    local block
    block=$(sed -n "/<$section>/,/<\/$section>/p" "$CFG" | grep -v "<")
    echo "$block" | grep "^[[:space:]]*${key}=" | head -1 | sed "s/.*${key}=//" | tr -d '[:space:]'
}

# ------------------------------------------------------------
# Decode base64 lines from a section (obfuscated config)
# ------------------------------------------------------------
xml_b64_section() {
    local section="$1"
    # Extract section content, strip XML tags, trim whitespace per line
    sed -n "/<$section>/,/<\/$section>/p" "$CFG" \
        | grep -v "<" \
        | while IFS= read -r line; do
            line=$(echo "$line" | tr -d '[:space:]')
            [ -z "$line" ] && continue
            decoded=$(echo "$line" | base64 -d 2>/dev/null)
            [ -n "$decoded" ] && echo "$decoded"
        done
}

# ------------------------------------------------------------
# Read build flags
# ------------------------------------------------------------
RUSTC_EDITION=$(xml_ini_get "build" "rustc_edition")
RUSTC_TARGET=$(xml_ini_get  "build" "rustc_target")
RUSTC_OPT=$(xml_ini_get     "build" "rustc_opt")
RUSTC_CRATE=$(xml_ini_get   "build" "rustc_crate_type")
CLANG_TARGET=$(xml_ini_get  "build" "clang_target")
CLANG_OPT=$(xml_ini_get     "build" "clang_opt")
OUTPUT=$(xml_ini_get        "build" "output")

echo "    rustc  edition=$RUSTC_EDITION target=$RUSTC_TARGET opt=$RUSTC_OPT"
echo "    clang  target=$CLANG_TARGET opt=$CLANG_OPT output=$OUTPUT"

# Decode and show secrets section (so build log is informative)
echo ""
echo "==> Decoding secrets section:"
xml_b64_section "secrets" | while IFS= read -r line; do
    [ -z "$line" ] && continue
    echo "    $line"
done

echo ""
echo "==> Decoding backrooms section:"
xml_b64_section "backrooms" | while IFS= read -r line; do
    [ -z "$line" ] && continue
    echo "    $line"
done

# ------------------------------------------------------------
# Step 1 — Compile Rust to static lib
# ------------------------------------------------------------
echo ""
echo "==> Compiling security.rs -> libsecurity.a"
# Note: security.rs uses #[unsafe(no_mangle)] which requires edition 2024+
rustc \
    --target "$RUSTC_TARGET" \
    --edition "$RUSTC_EDITION" \
    -C opt-level="$RUSTC_OPT" \
    --crate-type "$RUSTC_CRATE" \
    security.rs \
    -o libsecurity.a

echo "    OK: libsecurity.a"

# ------------------------------------------------------------
# Step 2 — Compile C++ + link Rust static lib -> WASM
# ------------------------------------------------------------
echo ""
echo "==> Compiling raycaster.cpp + libsecurity.a -> $OUTPUT"
clang++ \
    --target="$CLANG_TARGET" \
    -nostdlib \
    -Wl,--no-entry \
    -Wl,--export-dynamic \
    -Wl,--allow-undefined \
    -O"$CLANG_OPT" \
    raycaster.cpp libsecurity.a \
    -o "$OUTPUT"

echo "    OK: $OUTPUT"
echo ""
echo "==> Build complete!"
echo "    Files needed to run: $OUTPUT, rogue_3d.html, shadow_seed.xml"
echo ""

# Show wasm size
if command -v wasm-opt &> /dev/null; then
    echo "==> Optional: running wasm-opt for extra size savings"
    wasm-opt -Oz "$OUTPUT" -o "$OUTPUT"
    echo "    OK: wasm-opt done"
fi

SIZE=$(wc -c < "$OUTPUT")
echo "    $OUTPUT size: ${SIZE} bytes"
