#! /bin/bash
## Usage: resize.sh img size
## Options:
##   -h, --help    Display this message.
##

usage() {
  [ "$*" ] && echo "$0: $*"
  sed -n '/^##/,/^$/s/^## \{0,1\}//p' "$0"
  exit 2
} 2>/dev/null

main() {
  while [ $# -gt 0 ]; do
    case $1 in
    (-h|--help) usage 2>&1;;
    (--) shift; break;;
    (-*) usage "$1: unknown option";;
    (*) break;;
    esac
  done

  filename=$(basename -- "$1")
  filename="${filename%.*}"
  resized="${filename}.png"
  convert "${1}" -resize "$2!" -quality 100 "${resized}"
  echo conversion done!
  file "${resized}"
}

main "$@"
