#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
home="$root/.pio-home"
system_home="/home/gen/.platformio"
seed_libdeps=""

for candidate in \
  "$root/../vent_workshop/.pio/libdeps/d1_mini" \
  "$root/../street_info/.pio/libdeps/d1_mini"
do
  if [ -d "$candidate" ]; then
    seed_libdeps="$candidate"
    break
  fi
done

mkdir -p "$home/.cache" "$home/packages" "$home/platforms"

if [ -d "$system_home/packages" ] && [ ! -e "$home/.packages-linked" ]; then
  rm -rf "$home/packages"
  ln -s "$system_home/packages" "$home/packages"
  touch "$home/.packages-linked"
fi

if [ -d "$system_home/platforms" ] && [ ! -e "$home/.platforms-linked" ]; then
  rm -rf "$home/platforms"
  ln -s "$system_home/platforms" "$home/platforms"
  touch "$home/.platforms-linked"
fi

if [ -n "$seed_libdeps" ]; then
  mkdir -p "$root/.pio/libdeps"
  if [ ! -d "$root/.pio/libdeps/d1_mini" ] || [ -L "$root/.pio/libdeps/d1_mini" ]; then
    rm -rf "$root/.pio/libdeps/d1_mini"
    cp -a "$seed_libdeps" "$root/.pio/libdeps/d1_mini"
  fi
fi

export PLATFORMIO_HOME_DIR="$home"
exec pio "$@"
