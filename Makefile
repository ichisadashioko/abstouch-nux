THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

PREFIX := "/usr"

.PHONY: build
build: src/rust/main.rs
	cargo build
	mkdir build
	echo "-1" > build/event.conf
	echo "" > build/ename.conf
	echo "0" > build/xoff.conf
	echo "0" > build/yoff.conf
	gcc -lX11 -o build/input src/c/input.c
	gcc -o build/set_event src/c/set_event.c
	gcc -o build/set_offset src/c/set_offset.c

.PHONY: install
install:
	@#Bin Files
	install -Dm755 "${THIS_DIR}/target/debug/main" "${PREFIX}/bin/abstouch"
	install -Dm755 "${THIS_DIR}/build/input" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-input"
	install -Dm755 "${THIS_DIR}/build/set_event" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-set_event"
	install -Dm755 "${THIS_DIR}/build/set_offset" "${PREFIX}/share/abstouch-nux/bin/abstouch-nux-set_offset"
	@#Other Files
	install -Dm666 "${THIS_DIR}/build/event.conf" "${PREFIX}/share/abstouch-nux/event.conf"
	install -Dm666 "${THIS_DIR}/build/ename.conf" "${PREFIX}/share/abstouch-nux/ename.conf"
	install -Dm666 "${THIS_DIR}/build/xoff.conf" "${PREFIX}/share/abstouch-nux/xoff.conf"
	install -Dm666 "${THIS_DIR}/build/yoff.conf" "${PREFIX}/share/abstouch-nux/yoff.conf"
	@#Man Files and Completions
	install -Dm644 "${THIS_DIR}/man/abstouch.1" "${PREFIX}/share/man/man1/abstouch.1"
	install -Dm755 "${THIS_DIR}/completion/bash" "${PREFIX}/share/bash-completion/completions/abstouch"
	install -Dm755 "${THIS_DIR}/completion/zsh" "${PREFIX}/share/zsh/site-functions/_abstouch"
	install -Dm755 "${THIS_DIR}/completion/fish" "${PREFIX}/share/fish/vendor_completions.d/abstouch.fish"
