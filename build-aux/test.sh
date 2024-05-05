#!/bin/bash

set -Eeuo pipefail

if [[ "${VARIANT:-}" = "memcheck" ]]; then
	export VALGRIND=valgrind
	export TESTS_ENVIRONMENT="
		${VALGRIND} \
			--tool=memcheck \
			--leak-check=full \
			--leak-resolution=high \
			--num-callers=20 \
			--error-exitcode=1 \
			--show-possibly-lost=no"
fi

if [[ "${1}" == *.sh ]]; then
	exec "${@}"
else
	exec ${TESTS_ENVIRONMENT:-} "${@}"
fi
