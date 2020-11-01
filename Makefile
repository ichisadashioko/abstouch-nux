THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

.PHONY: build
build: src/go/main.go
	mkdir build
	go build -o build/abstouch src/go/main.go
	gcc -o build/test src/c/test.c
	chmod +x build/abstouch build/test

.PHONY: install
install:
	install -Dm 755 "${THIS_DIR}/build/abstouch" "${PREFIX}/bin/abstouch"
	install -Dm 755 "${THIS_DIR}/build/test" "${PREFIX}/share/abstouch-nux/test"
