#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

STRAKSD=${STRAKSD:-$SRCDIR/straksd}
STRAKSCLI=${STRAKSCLI:-$SRCDIR/straks-cli}
STRAKSTX=${STRAKSTX:-$SRCDIR/straks-tx}
STRAKSQT=${STRAKSQT:-$SRCDIR/qt/straks-qt}

[ ! -x $STRAKSD ] && echo "$STRAKSD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
STAKVER=($($STRAKSCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for straksd if --version-string is not set,
# but has different outcomes for straks-qt and straks-cli.
echo "[COPYRIGHT]" > footer.h2m
$STRAKSD --version | sed -n '1!p' >> footer.h2m

for cmd in $STRAKSD $STRAKSCLI $STRAKSTX $STRAKSQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${STAKVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${STAKVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
