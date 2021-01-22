<p align="center">
  <h1 align="center">abstouch-nux</h1>
  <h4 align="center">An absolute touchpad input client for GNU/Linux.</h4>
  <h5 align="center">Inspired by <a href="https://github.com/apsun/AbsoluteTouchEx">apsun/AbsoluteTouchEx</a>. Make sure to check his repository, especially if you use Windows!</h5>

  <h6 align="center">
    <a href="https://github.com/acedron/abstouch-nux" alt="License">
      <img src="https://img.shields.io/github/license/acedron/abstouch-nux?style=for-the-badge"></a>
    <a href="https://github.com/acedron/abstouch-nux/releases/latest" alt="Latest Release">
      <img src="https://img.shields.io/github/v/release/acedron/abstouch-nux?include_prereleases&style=for-the-badge"></a>
    <a href="https://github.com/acedron/abstouch-nux/commits/master" alt="Maintenance">
      <img src="https://img.shields.io/maintenance/yes/2021?style=for-the-badge"></a>
    <a href="https://github.com/acedron/abstouch-nux/commit/master" alt="AUR Maintainer">
      <img src="https://img.shields.io/github/last-commit/acedron/abstouch-nux?style=for-the-badge"></a>
  </h6>
</p>


## Installation

Only available for **GNU/Linux**.

See **[installing](https://github.com/acedron/abstouch-nux/blob/master/doc/installing.md)** documentation for more information.

### Building From Source

* You can use ***autotools*** to make it.
* Make sure you have `gcc`, `automake`, `autoconf` and `libx11-dev (libx11 or libX11-devel for some systems)` installed.

```bash
$ git clone https://github.com/acedron/abstouch-nux.git
$ cd abstouch-nux
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
```

You can replace `install` with `uninstall` if you want to uninstall.

## Usage

You can start the client right away.

See **[usage](https://github.com/acedron/abstouch-nux/blob/master/doc/usage.md)** documentation for more information.

See **[examples](https://github.com/acedron/abstouch-nux/tree/master/examples)** directory for some examples.

```bash
$ abstouch start
```

You can calibrate input to match your touchpad.

```bash
$ abstouch calibrate
```

You can run it on background if you use `--daemon` flag.

```bash
$ abstouch start -d
```

See help or man page for more information.

```bash
$ abstouch help
$ man abstouch
```
