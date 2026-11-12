#!/usr/bin/env bash
set -euo pipefail

if [[ ${1:-} == "--container" ]]; then
    cd /workspace
    if [[ -d /cache/.west && -d /cache/zephyr && -d /cache/modules &&
          -d /cache/zmk && -d /cache/zmk-rgbled-widget &&
          -d /cache/zmk-dongle-display ]]; then
        cp -a /cache/.west /cache/zephyr /cache/modules /cache/zmk \
            /cache/zmk-rgbled-widget /cache/zmk-dongle-display .
        cp -a /repo/config config
    else
        cp -a /repo/config config
        west init -l config
    fi
    if ! west update --fetch-opt=--filter=tree:0 > /out/dependencies.log 2>&1; then
        tail -n 80 /out/dependencies.log >&2
        exit 1
    fi
    west zephyr-export
    west manifest --freeze --active-only > /out/west-manifest.lock.yml
    if ! west build -s zmk/app -d build/standalone -b seeeduino_xiao_ble -- \
        -DZMK_CONFIG=/workspace/config -DSHIELD=totem_left \
        -DZMK_EXTRA_MODULES=/repo > /out/standalone.log 2>&1; then
        tail -n 80 /out/standalone.log >&2
        exit 1
    fi
    cp build/standalone/zephyr/zmk.uf2 /out/standalone.uf2
    cp build/standalone/zephyr/zephyr.dts /out/standalone.dts
    cp build/standalone/zephyr/.config /out/standalone.config
    cp config/totem.keymap /out/totem.keymap
    exit 0
fi

if [[ ${1:-} == "--help" || ${1:-} == "-h" ]]; then
    echo "Usage: $0 [label]"
    echo "Build the current working tree's standalone left firmware with Docker."
    echo "Output: firmware/local-<label>/ (default label: timestamp)."
    echo "Existing output and firmware/latest are never overwritten."
    exit 0
fi
if [[ $# -gt 1 ]]; then
    echo "Usage: $0 [label]" >&2
    exit 2
fi
label=${1:-$(date +%Y%m%d-%H%M%S)}
if [[ ! $label =~ ^[a-zA-Z0-9][a-zA-Z0-9._-]*$ ]]; then
    echo "Invalid label: use letters, numbers, '.', '_' or '-'." >&2
    exit 2
fi

repo=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
command -v docker > /dev/null || { echo "Docker is required." >&2; exit 1; }
docker info > /dev/null || { echo "Docker is not available." >&2; exit 1; }
mkdir -p "$repo/firmware"
out="$repo/firmware/local-$label"
if ! mkdir "$out"; then
    echo "Output already exists or cannot be created: $out" >&2
    exit 1
fi

tmp=$(mktemp -d /tmp/totemist-local-build.XXXXXXXX)
volume="totemist-local-$(basename "$tmp")"
volume_created=0
cleanup() {
    if [[ $volume_created == 1 ]]; then
        docker volume rm "$volume" > /dev/null || echo "Could not remove volume: $volume" >&2
    fi
    rm -rf "$tmp"
}
trap cleanup EXIT

mkdir "$tmp/repo"
cd "$repo"
git ls-files -z --cached --others --exclude-standard > "$tmp/files"
while IFS= read -r -d '' path; do
    if [[ -f $path || -L $path ]]; then
        mkdir -p "$tmp/repo/$(dirname "$path")"
        cp -pP "$path" "$tmp/repo/$path"
    fi
done < "$tmp/files"

if docker volume inspect "$volume" > /dev/null 2>&1; then
    echo "Temporary volume already exists: $volume" >&2
    exit 1
fi
docker volume create "$volume" > /dev/null
volume_created=1
cache=()
if docker volume inspect fortemis-zmk > /dev/null 2>&1; then
    cache=(-v fortemis-zmk:/cache:ro)
    echo "Using the existing dependency cache (read-only)."
else
    echo "No dependency cache found; fetching dependencies."
fi
echo "Building standalone left firmware; logs: $out"
if ! docker run --rm ${cache[@]+"${cache[@]}"} \
    -v "$volume:/workspace" -v "$tmp/repo:/repo:ro" -v "$out:/out" \
    zmkfirmware/zmk-build-arm:stable bash /repo/scripts/build-local.sh --container; then
    echo "Build failed; see logs in $out" >&2
    exit 1
fi
echo "Firmware: $out/standalone.uf2"
