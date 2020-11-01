THIS_DIR=$(shell cd "$(dirname "${BASH_SOURCE[0]:-${(%):-%N}}")" && pwd)

.PHONY: build
build: src/go/main.go
	mkdir build
	go build -o build/abstouch src/go/main.go
	chmod +x build/abstouch

.PHONY: install
install:
	install -Dm 755 "${THIS_DIR}/build/abstouch" "${PREFIX}/bin/abstouch"
