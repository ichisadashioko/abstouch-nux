THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

.PHONY: build
build: src/rust/main.rs
	mkdir build
	rustc -o build/abstouch src/rust/main.rs
	gcc -o build/input src/c/input.c
	chmod +x build/abstouch build/test

.PHONY: install
install:
	install -Dm 755 "${THIS_DIR}/build/abstouch" "${PREFIX}/bin/abstouch"
	install -Dm 755 "${THIS_DIR}/build/input" "${PREFIX}/share/abstouch-nux/input"
