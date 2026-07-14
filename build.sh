#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
SCRIPT_START=$SECONDS

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; NC='\033[0m'
pass() { echo -e "  ${GREEN}✓${NC} $1"; }
fail() { echo -e "  ${RED}✗${NC} $1"; }
info() { echo -e "  ${YELLOW}→${NC} $1"; }
die()  { echo -e "  ${RED}✗${NC} $1"; exit 1; }
step() { echo -e "\n  ${BLUE}[${1}/${2}]${NC} ${3}"; }

CLEAN=false
FORMAT=false
TIDY=false
JOBS=$(nproc)

usage() {
    echo "Usage: bash build.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --clean       Clean previous build artifacts before building"
    echo "  --format      Run clang-format (auto-fix sources)"
    echo "  --tidy        Run clang-tidy with --fix (check and auto-fix issues)"
    echo "  -j N          Use N parallel jobs (default: all cores)"
    echo "  -h, --help    Show this help"
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --clean) CLEAN=true ;;
        --format) FORMAT=true ;;
        --tidy) TIDY=true ;;
        -j) JOBS="$2"; shift ;;
        -h|--help) usage ;;
        *) die "Unknown argument: $1" ;;
    esac
    shift
done

BUILD_DIR="/tmp/build-i2pchat"
DIST_DIR="$ROOT/dist"

# --- dependency check ---
DEPS=(g++ qmake make bear compiledb pkg-config)
if $FORMAT; then DEPS+=(clang-format); fi
if $TIDY; then DEPS+=(clang-tidy run-clang-tidy); fi
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

TOTAL_STEPS=2
if $FORMAT; then ((TOTAL_STEPS++)); fi
if $TIDY; then ((TOTAL_STEPS++)); fi
CUR_STEP=0

next_step() {
    ((++CUR_STEP))
    step "$CUR_STEP" "$TOTAL_STEPS" "$1"
}

next_step "Preparing build directory & generated headers"
if $CLEAN && test -f Makefile; then
    echo "  Cleaning previous build..."
    make distclean >/dev/null 2>&1
    rm -f Makefile
fi
mkdir -p "$BUILD_DIR" "$DIST_DIR"
qmake -after DESTDIR=dist/ OBJECTS_DIR="$BUILD_DIR/obj/" MOC_DIR="$BUILD_DIR/moc/" RCC_DIR="$BUILD_DIR/qrc/" >/dev/null 2>&1
for ui in src/gui/*.ui; do
  uic "$ui" -o "src/gui/ui_$(basename "${ui%.ui}").h"
done
pass "Generated $(ls src/gui/*.ui | wc -l) uic headers"
compiledb make -n >/dev/null 2>&1
mv compile_commands.json "$BUILD_DIR/"
pass "compile_commands.json ready"

if $FORMAT; then
next_step "Formatting ${#BUILT_SOURCES[@]} sources with clang-format"
clang-format -i "${BUILT_SOURCES[@]}"
pass "Formatted ${#BUILT_SOURCES[@]} files"
fi

if $TIDY; then
next_step "Running clang-tidy (${#BUILT_SOURCES[@]} files, parallel)..."
CLANG_OUTPUT=$(run-clang-tidy -p "$BUILD_DIR" -quiet -use-color -fix -j "$JOBS" "${BUILT_SOURCES[@]}" 2>&1 || true)
FINDINGS=$(echo "$CLANG_OUTPUT" \
  | grep -v '^[0-9]\+ warnings generated' \
  | grep -v '^Suppressed' \
  | sed '/^$/d')
if [[ -n "$FINDINGS" ]]; then
    echo "$FINDINGS"
    if echo "$FINDINGS" | grep -qE '^.*: error:.*\[.*\].*$'; then
        HAS_ERRORS=true
    else
        HAS_ERRORS=false
    fi
else
    HAS_ERRORS=false
fi
if $HAS_ERRORS; then
    fail "clang-tidy found issues — fix before building"
    exit 1
fi
pass "clang-tidy clean"
fi

next_step "Compiling ${#BUILT_SOURCES[@]} source files..."
bear --output "$BUILD_DIR/compile_commands.json" -- make -j"$JOBS" >/dev/null 2>&1
strip "$DIST_DIR/I2PChat"
FILESIZE=$(stat --format=%s "$DIST_DIR/I2PChat")

echo ""
echo -e "  ${GREEN}BUILD SUCCESSFUL${NC}"
printf "     %-10s %s\n" "Binary:"    "${DIST_DIR}/I2PChat"
printf "     %-10s %s\n" "Size:"      "${FILESIZE} bytes"
printf "     %-10s %s\n" "Completed:" "$(date '+%Y-%m-%d %H:%M:%S') ($((SECONDS - SCRIPT_START))s)"
