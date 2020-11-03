THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

.PHONY: build
build: src/rust/main.rs
	cargo build
	mkdir build
	gcc -o build/input src/c/input.c
	gcc -o build/set_event src/c/set_event.c

.PHONY: install
install:
	install -Dm 755 "${THIS_DIR}/target/debug/main" "${PREFIX}/bin/abstouch"
	install -Dm 755 "${THIS_DIR}/build/input" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-input"
	install -Dm 755 "${THIS_DIR}/build/set_event" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-set_event"
