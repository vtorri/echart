#!/bin/sh

autoreconf -vif --warnings=all || exit 1

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
