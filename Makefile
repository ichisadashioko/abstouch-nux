THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

.PHONY: build
build: src/rust/main.rs
	cargo build
	mkdir build
	echo "-1" > build/event.conf
	gcc -o build/input src/c/input.c
	gcc -o build/set_event src/c/set_event.c

.PHONY: install
install:
	@#Bin Files
	install -Dm 755 "${THIS_DIR}/target/debug/main" "${PREFIX}/bin/abstouch"
	install -Dm 755 "${THIS_DIR}/build/input" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-input"
	install -Dm 755 "${THIS_DIR}/build/set_event" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-set_event"
	@#Other Files
	install -Dm 666 "${THIS_DIR}/build/event.conf" "${PREFIX}/share/abstouch-nux/event.conf"
	@#Man Files
	install -Dm 755 "${THIS_DIR}/man/abstouch.1" "${PREFIX}/share/man/man1/abstouch.1"
