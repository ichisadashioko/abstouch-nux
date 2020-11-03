THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

.PHONY: build
build: src/rust/main.rs
	cargo build
	gcc -o input src/c/input.c

.PHONY: install
install:
	install -Dm 755 "${THIS_DIR}/target/debug/main" "${PREFIX}/bin/abstouch"
	install -Dm 755 "${THIS_DIR}/input" "${PREFIX}/share/abstouch-nux/abstouch-nux-input"
