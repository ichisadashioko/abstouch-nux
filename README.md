<p align="center">
  <h1 align="center">abstouch-nux</h1>
  <h4 align="center">An absolute touchpad input client for GNU/Linux.</h4>
  <h5 align="center">Inspired by <a href="https://github.com/apsun/AbsoluteTouchEx">apsun/AbsoluteTouchEx</a>. Make sure to check his repository, especially if you use Windows!</h5>
  <h6 align="center">
    <a href="https://aur.archlinux.org/packages/abstouch-nux" alt="License">
      <img src="https://img.shields.io/aur/license/abstouch-nux?style=for-the-badge"></a>
    <a href="https://aur.archlinux.org/packages/abstouch-nux" alt="Version">
      <img src="https://img.shields.io/aur/version/abstouch-nux?style=for-the-badge"></a>
    <a href="https://aur.archlinux.org/packages/abstouch-nux" alt="AUR Rating">
      <img src="https://img.shields.io/aur/votes/abstouch-nux?style=for-the-badge"></a>
    <a href="https://aur.archlinux.org/packages/abstouch-nux" alt="AUR Maintainer">
      <img src="https://img.shields.io/aur/maintainer/abstouch-nux?style=for-the-badge"></a>
  </h6>
</p>


## Installation

### Building From Source

You can use autoconf to make it.

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
