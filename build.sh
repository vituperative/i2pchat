#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
pass() { echo -e "  ${GREEN}✓${NC} $1"; }
fail() { echo -e "  ${RED}✗${NC} $1"; }
info() { echo -e "  ${YELLOW}→${NC} $1"; }
die()  { echo -e "  ${RED}✗${NC} $1"; exit 1; }

BUILD_DIR="/tmp/build-i2pchat"
DIST_DIR="$ROOT/dist"

# --- dependency check ---
DEPS=(g++ qmake make bear clang-format clang-tidy pkg-config)
MISSING=()
for dep in "${DEPS[@]}"; do
  command -v "$dep" &>/dev/null || MISSING+=("$dep")
done

if ! pkg-config --exists Qt5Core Qt5Widgets Qt5Network Qt5Multimedia Qt5Svg Qt5Xml 2>/dev/null; then
  MISSING+=("Qt5 dev packages (qtbase5-dev, libqt5svg5-dev, qtmultimedia5-dev, libqt5xml5-dev)")
fi

[[ ${#MISSING[@]} -gt 0 ]] && die "Missing dependencies: ${MISSING[*]}"

# --- source list (only files that participate in the build) ---
BUILT_SOURCES=($(sed -n '/^SOURCES/,/^HEADERS/p' I2PChat.pro \
  | grep '\.cpp' | sed 's/\\$//; s/^\s*//; s/\s*$//' | sed '/^$/d'))

mkdir -p "$BUILD_DIR" "$DIST_DIR"

# --- Step 0: clean build, regenerate Makefile, build with bear ---
info "Compiling..."
rm -rf "$BUILD_DIR"/*
rm -f Makefile
qmake -after DESTDIR=dist/ OBJECTS_DIR="$BUILD_DIR/obj/" MOC_DIR="$BUILD_DIR/moc/" RCC_DIR="$BUILD_DIR/qrc/" 2>&1 | tail -1
bear --output "$BUILD_DIR/compile_commands.json" -- make -j"$(nproc)" 2>&1 | tail -1
strip "$DIST_DIR/I2PChat"
echo ""

# --- Step 1: clang-format (auto-fix in place) ---
info "Formatting ${#BUILT_SOURCES[@]} sources..."
clang-format -i "${BUILT_SOURCES[@]}"
pass "Formatted ${#BUILT_SOURCES[@]} files"
echo ""

# --- Step 2: clang-tidy ---
info "Running clang-tidy..."
HAS_ERRORS=false
for f in "${BUILT_SOURCES[@]}"; do
    OUTPUT=$(clang-tidy --quiet -p "$BUILD_DIR" "$f" 2>&1 || true)
    FINDINGS=$(echo "$OUTPUT" \
      | grep -v '^[0-9]\+ warnings generated' \
      | grep -v '^Suppressed' \
      | sed '/^$/d')
    if [[ -n "$FINDINGS" ]]; then
        echo "  ${YELLOW}$f${NC}"
        echo "$FINDINGS" | sed 's/^/    /'
        HAS_ERRORS=true
    fi
done

echo ""
if $HAS_ERRORS; then
    fail "clang-tidy found issues — binary at $DIST_DIR/I2PChat"
    exit 1
else
    pass "clang-tidy clean — binary at $DIST_DIR/I2PChat"
fi
