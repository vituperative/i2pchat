#!/bin/bash
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
BUMP=false
APPIMAGE=false
WINDOWS=false
UPX=false
JOBS=$(nproc)

usage() {
    echo "Usage: bash build.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --clean       Clean previous build artifacts before building"
    echo "  --format      Run clang-format (auto-fix sources)"
    echo "  --tidy        Run clang-tidy with --fix (check and auto-fix issues)"
    echo "  --appimage    Build portable AppImage (Linux only)"
    echo "  --windows     Cross-compile Windows .exe via MinGW-w64"
    echo "  --upx         Compress binary with UPX (reduces size, all platforms)"
    echo "  --bump        Bump patch version, update Core.h and create git tag"
    echo "  -j N          Use N parallel jobs (default: all cores)"
    echo "  -h, --help    Show this help"
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
    --clean) CLEAN=true ;;
    --format) FORMAT=true ;;
    --tidy) TIDY=true ;;
    --appimage) APPIMAGE=true ;;
    --windows) WINDOWS=true ;;
    --upx) UPX=true ;;
    --bump) BUMP=true ;;
    -j) JOBS="$2"; shift ;;
    -h|--help) usage ;;
    *) die "Unknown argument: $1" ;;
    esac
    shift
done

# --- version bump ---
if $BUMP; then
  OLD=$(sed -n 's/^#define CLIENTVERSION "\(.*\)"$/\1/p' src/backend/Core.h)
  IFS=. read -r MAJ MIN PAT <<< "$OLD"
  NEW="$MAJ.$MIN.$((PAT + 1))"
  sed -i "s/^#define CLIENTVERSION \"$OLD\"$/#define CLIENTVERSION \"$NEW\"/" src/backend/Core.h
  echo "  Bumped version: $OLD → $NEW"
  git add src/backend/Core.h
  git commit -m "chore: bump version to $NEW"
  git tag -a "$NEW" -m "Release $NEW"
  echo "  Created tag: $NEW"
  exit 0
fi

BUILD_DIR="/tmp/build-i2pchat"
DIST_DIR="$ROOT/dist"

# --- dependency check ---
DEPS=(g++ qmake make pkg-config file)
if $FORMAT; then DEPS+=(clang-format); fi
if $TIDY; then DEPS+=(clang-tidy run-clang-tidy bear compiledb); fi
if $APPIMAGE; then DEPS+=(wget); fi
MISSING=()
if $WINDOWS; then
  DEPS+=(git gperf)
  if ! command -v libtool &>/dev/null; then
    if command -v libtoolize &>/dev/null; then
      MISSING+=("libtool (try: apt install libtool-bin)")
    else
      MISSING+=("libtool (try: apt install libtool libtool-bin)")
    fi
  fi
fi
for dep in "${DEPS[@]}"; do
  command -v "$dep" &>/dev/null || MISSING+=("$dep")
done

if ! $WINDOWS && ! pkg-config --exists Qt6Core Qt6Widgets Qt6Network Qt6Multimedia Qt6Svg Qt6Xml 2>/dev/null; then
  MISSING+=("Qt6 dev packages (qt6-base-dev, libqt6svg6-dev, qt6-multimedia-dev, libqt6xml6-dev)")
fi

[[ ${#MISSING[@]} -gt 0 ]] && die "Missing dependencies: ${MISSING[*]}"

if $WINDOWS; then
  MXE_DIR="$ROOT/tools/mxe"
  MXE_TARGET="x86_64-w64-mingw32.static"
  MXE_QMAKE="$MXE_DIR/usr/bin/$MXE_TARGET-qmake-qt6"

  # Bootstrap MXE if not found via system or local
  if [ ! -f "$MXE_QMAKE" ] && ! command -v "${MXE_TARGET}-qmake-qt6" &>/dev/null; then
    if [ ! -d "$MXE_DIR" ]; then
      info "MXE not found at $MXE_DIR — cloning..."
      git clone --depth 1 https://github.com/mxe/mxe.git "$MXE_DIR"
    fi
    info "Building MXE qt6 target (first build takes a while)..."
    # Ensure `python` resolves (pyenv shim fails for bare `python`)
    PYTHON_REAL="$(python3 -c 'import sys; print(sys.executable)')"
    PYTHON_WRAPPER="$BUILD_DIR/mxe-python"
    mkdir -p "$PYTHON_WRAPPER"
    ln -sf "$PYTHON_REAL" "$PYTHON_WRAPPER/python"
    pushd "$MXE_DIR" >/dev/null
    # sccache clashes with mxe's nonetwork LD_PRELOAD (sccache calls
    # connect() etc. which nonetwork blocks with EACCES).  Stub a
    # pass-through sccache so meson/cmake/ninja don't use the real one.
    MXE_CC_WRAPPER="$BUILD_DIR/mxe-cc"
    mkdir -p "$MXE_CC_WRAPPER"
    cat > "$MXE_CC_WRAPPER/sccache" << 'SCCACHE_STUB'
#!/bin/bash
exec "$@"
SCCACHE_STUB
    chmod +x "$MXE_CC_WRAPPER/sccache"
    # Strip sccache refs from cmake toolchain files left by previous runs
    find "$MXE_DIR/tmp-glib-"* -path '*/meson-private/*.cmake' \
      -exec sed -i '/sccache/d' {} + 2>/dev/null || true
    rm -rf "$MXE_DIR"/tmp-glib-* "$MXE_DIR"/log/glib*
    # fontconfig's meson build links -lintl but not -liconv (needed for
    # static).  fontconfig uses dependency('intl') without static: true,
    # so Libs.private is ignored.  Put -liconv in Libs: unconditionally.
    for T in "$MXE_TARGET"; do
      PCDIR="$MXE_DIR/usr/$T/lib/pkgconfig"
      mkdir -p "$PCDIR"
      # fontconfig queries `dependency('intl')` → looks for intl.pc,
      # not libintl.pc.  Provide both names.
      for pc in libintl intl; do
        cat > "$PCDIR/$pc.pc" << 'INTL_PC'
prefix=/usr
exec_prefix=${prefix}
libdir=${prefix}/lib
includedir=${prefix}/include

Name: libintl
Description: libintl from GNU gettext
Version: 1.0
Libs: -lintl -liconv
Cflags: -I${includedir}
INTL_PC
      done
    done
    PATH="$MXE_CC_WRAPPER:$PYTHON_WRAPPER:$PATH" \
      make qt6 MXE_TARGETS="$MXE_TARGET" JOBS="$JOBS" PYTHON=python3
    popd >/dev/null
  fi

  if [ ! -f "$MXE_QMAKE" ]; then
    die "MinGW Qt6 qmake not found (MXE build may have failed)"
  fi
fi

# --- source list (only files that participate in the build) ---
BUILT_SOURCES=($(sed -n '/^SOURCES/,/^HEADERS/p' I2PChat.pro \
  | grep '\.cpp' | sed 's/\\$//; s/^\s*//; s/\s*$//' | sed '/^$/d'))

TOTAL_STEPS=2
if $FORMAT; then ((TOTAL_STEPS++)); fi
if $TIDY; then ((TOTAL_STEPS++)); fi
if $APPIMAGE; then ((TOTAL_STEPS++)); fi
if $WINDOWS; then ((TOTAL_STEPS++)); fi
if $UPX; then ((TOTAL_STEPS++)); fi
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
if command -v compiledb &>/dev/null; then
  compiledb make -n >/dev/null 2>&1
  mv compile_commands.json "$BUILD_DIR/"
  pass "compile_commands.json ready"
fi

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

if $WINDOWS; then
  BINARY="${DIST_DIR}/I2PChat.exe"
else
  BINARY="${DIST_DIR}/I2PChat"
fi

if ! $CLEAN && test -f "$BINARY"; then
  NEWER=$(find src/ \( -name '*.cpp' -o -name '*.h' -o -name '*.ui' -o -name '*.pro' \) -newer "$BINARY" -print -quit 2>/dev/null)
  if [[ -n "$NEWER" ]]; then
    info "Source files changed — recompiling"
  else
    info "No source changes — skipping compilation"
    SKIP_BUILD=true
  fi
fi

if [[ -z "${SKIP_BUILD:-}" ]]; then
  if $WINDOWS; then
  next_step "Cross-compiling ${#BUILT_SOURCES[@]} source files for Windows..."
  if [ -n "$MXE_QMAKE" ] && [ -f "$MXE_QMAKE" ]; then
    CROSS_QMAKE="$MXE_QMAKE"
  else
    CROSS_QMAKE="${MXE_TARGET%%.*}-qmake-qt6"
  fi
  $CROSS_QMAKE -after "DESTDIR=$DIST_DIR/" "OBJECTS_DIR=$BUILD_DIR/obj/" "MOC_DIR=$BUILD_DIR/moc/" "RCC_DIR=$BUILD_DIR/qrc/" >/dev/null 2>&1
  PATH="$MXE_DIR/usr/bin:$PATH" make -j"$JOBS" >/dev/null 2>&1
  ${MXE_TARGET}-strip "$DIST_DIR/I2PChat.exe" 2>/dev/null || strip "$DIST_DIR/I2PChat.exe" 2>/dev/null || true
  else
  next_step "Compiling ${#BUILT_SOURCES[@]} source files..."
  if command -v bear &>/dev/null; then
    bear --output "$BUILD_DIR/compile_commands.json" -- make -j"$JOBS" >/dev/null 2>&1
  else
    make -j"$JOBS" >/dev/null 2>&1
  fi
  strip "$DIST_DIR/I2PChat"
  fi
fi
FILESIZE=$(stat --format=%s "$BINARY" 2>/dev/null || stat -f%z "$BINARY" 2>/dev/null)

echo ""
echo -e "  ${GREEN}BUILD SUCCESSFUL${NC}"
printf "     %-10s %s\n" "Binary:"    "$BINARY"
printf "     %-10s %s\n" "Size:"      "${FILESIZE} bytes"
printf "     %-10s %s\n" "Completed:" "$(date '+%Y-%m-%d %H:%M:%S') ($((SECONDS - SCRIPT_START))s)"

# --- UPX compression ---
if $UPX; then
next_step "Compressing with UPX..."
if command -v upx &>/dev/null; then
  upx --best "$BINARY" 2>&1
  FILESIZE=$(stat --format=%s "$BINARY" 2>/dev/null || stat -f%z "$BINARY" 2>/dev/null)
  printf "     %-10s %s\n" "Compressed:" "${FILESIZE} bytes"
else
  fail "upx not found — install with: apt install upx"
fi
fi

# --- AppImage ---
if $APPIMAGE; then
next_step "Creating portable AppImage..."
LINUXDEPLOY="$BUILD_DIR/linuxdeploy-x86_64.AppImage"
PLUGIN_QT="$BUILD_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"

if [ ! -f "$LINUXDEPLOY" ]; then
  wget -q "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" -O "$LINUXDEPLOY"
  chmod +x "$LINUXDEPLOY"
fi
if [ ! -f "$PLUGIN_QT" ]; then
  wget -q "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage" -O "$PLUGIN_QT"
  chmod +x "$PLUGIN_QT"
fi

APPDIR="$BUILD_DIR/AppDir"
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/scalable/apps"

cp "$DIST_DIR/I2PChat" "$APPDIR/usr/bin/"
cp "$ROOT/packaging/I2PChat.desktop" "$APPDIR/usr/share/applications/"
cp "$ROOT/src/gui/icons/avatar.svg" "$APPDIR/usr/share/icons/hicolor/scalable/apps/i2pchat.svg"

export APPIMAGE_EXTRACT_AND_RUN=1
export LINUXDEPLOY_OUTPUT_VERSION=

"$LINUXDEPLOY" --appdir "$APPDIR" --plugin qt --output appimage 2>&1 || true
APPIMAGE_PATH=$(ls I2PChat-*.AppImage 2>/dev/null | head -1 || true)
if [ -n "$APPIMAGE_PATH" ]; then
  mv "$APPIMAGE_PATH" "$DIST_DIR/"
  APPIMAGE_SIZE=$(stat --format=%s "$DIST_DIR/$(basename "$APPIMAGE_PATH")" 2>/dev/null || stat -f%z "$DIST_DIR/$(basename "$APPIMAGE_PATH")" 2>/dev/null)
  printf "     %-10s %s\n" "AppImage:" "$DIST_DIR/$(basename "$APPIMAGE_PATH") ($(numfmt --to=iec $APPIMAGE_SIZE 2>/dev/null || echo "$APPIMAGE_SIZE bytes"))"
fi
fi
