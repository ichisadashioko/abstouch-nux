THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

BIN := abstouch
DESTDIR :=
CARGO ?= cargo
GCC ?= gcc
PKGNAME := abstouch-nux
PREFIX := /usr/local

C_INPUT_FLAGS ?= -lX11

.PHONY: default
default: build

.PHONY: build
build: src/rust/main.rs
	$(CARGO) build
	mkdir build
	echo "-1" > build/event.conf
	echo "" > build/ename.conf
	echo "0" > build/xoff.conf
	echo "0" > build/yoff.conf
	$(GCC) $(C_INPUT_FLAGS) -o build/input src/c/input.c
	$(GCC) -o build/set_event src/c/set_event.c
	$(GCC) -o build/set_offset src/c/set_offset.c

.PHONY: install
install:
	@#Bin Files
	install -Dm755 "${THIS_DIR}/target/debug/main" "${DESTDIR}${PREFIX}/bin/${BIN}"
	install -Dm755 "${THIS_DIR}/build/input" "${DESTDIR}${PREFIX}/share/${PKGNAME}/bin/${PKGNAME}-input"
	install -Dm755 "${THIS_DIR}/build/set_event" "${DESTDIR}${PREFIX}/share/${PKGNAME}/bin/${PKGNAME}-set_event"
	install -Dm755 "${THIS_DIR}/build/set_offset" "${DESTDIR}${PREFIX}/share/${PKGNAME}/bin/${PKGNAME}-set_offset"
	@#Other Files
	install -Dm666 "${THIS_DIR}/build/event.conf" "${DESTDIR}${PREFIX}/share/${PKGNAME}/event.conf"
	install -Dm666 "${THIS_DIR}/build/ename.conf" "${DESTDIR}${PREFIX}/share/${PKGNAME}/ename.conf"
	install -Dm666 "${THIS_DIR}/build/xoff.conf" "${DESTDIR}${PREFIX}/share/${PKGNAME}/xoff.conf"
	install -Dm666 "${THIS_DIR}/build/yoff.conf" "${DESTDIR}${PREFIX}/share/${PKGNAME}/yoff.conf"
	@#Man Files and Completions
	install -Dm644 "${THIS_DIR}/man/${BIN}.1" "${DESTDIR}${PREFIX}/share/man/man1/${BIN}.1"
	install -Dm644 "${THIS_DIR}/completion/bash" "${DESTDIR}${PREFIX}/share/bash-completion/completions/${BIN}"
	install -Dm644 "${THIS_DIR}/completion/zsh" "${DESTDIR}${PREFIX}/share/zsh/site-functions/_${BIN}"
	install -Dm644 "${THIS_DIR}/completion/fish" "${DESTDIR}${PREFIX}/share/fish/vendor_completions.d/${BIN}.fish"

.PHONY: uninstall
uninstall:
	@#Bin Files and Other Files
	rm -f "${DESTDIR}${PREFIX}/bin/${BIN}"
	rm -rf "${DESTDIR}${PREFIX}/share/${PKGNAME}"
	@#Man Files and Completions
	rm -f "${DESTDIR}${PREFIX}/share/man/man1/${BIN}.1"
	rm -f "${DESTDIR}${PREFIX}/share/bash-completion/completions/${BIN}"
	rm -f "${DESTDIR}${PREFIX}/share/zsh/site-functions/_${BIN}"
	rm -f "${DESTDIR}${PREFIX}/share/fish/vendor_completions.d/${BIN}.fish"
