# abstouch-nux
<p align="center">
  <h1 align="center">abstouch-nux</h1>
  <h4 align="center">An absolute touchpad input client for GNU/Linux.</h4>
  <h5 align="center">Inspired by [apsun/AbsoluteTouchEx](https://github.com/apsun/AbsoluteTouchEx). Make sure to check his repository, especially if you use Windows!</h5>
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

### Arch Linux (Manjaro etc.)
Available as `abstouch-nux` AUR [package](https://aur.archlinux.org/packages/abstouch-nux).

```bash
$ yay -S abstouch-nux
```

### Debian (Ubuntu, Kali etc.)
Available as `abstouch-nux` .deb [package](https://github.com/acedron/abstouch-nux/releases/latest).

```bash
$ wget $(curl -s https://api.github.com/repos/acedron/abstouch-nux/releases/latest | grep -o "http.*deb") -q --show-progress
$ sudo dpkg -i abstouch-nux*.deb
```

### Building From Source

**Not recommended.** Make sure you have `rust` and `gcc` installed.

```bash
$ git clone https://github.com/acedron/abstouch-nux.git
$ cd abstouch-nux
$ make -i build
$ sudo make -i install
```

You can then uninstall it with:

```bash
$ git clone https://github.com/acedron/abstouch-nux.git
$ cd abstouch-nux
$ sudo make -i uninstall
```

## Usage

First, you have to set an input event.

```bash
$ abstouch setevent
```

And then, you can start the client right away.

```bash
$ abstouch start
```

You can run it on background if you use `--daemon` flag.

```bash
$ abstouch start -d
```

You can set cursor offset to match your touchpad.

```bash
$ abstouch setoffset
```

See help or man page for more information.

```bash
$ abstouch help
$ man abstouch
```
