# abstouch-nux
An absolute touchpad input client for GNU/Linux.
Inspired by [apsun/AbsoluteTouchEx](https://github.com/apsun/AbsoluteTouchEx). <br/>
Originally started as an [evtest](https://github.com/freedesktop-unofficial-mirror/evtest) mod. That means this repository uses code from it. <br/>
<br/>
**If you use Windows, check this awesome repository: [apsun/AbsoluteTouchEx](https://github.com/apsun/AbsoluteTouchEx)!**

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

## Usage

First, you have to set an input event.

```bash
abstouch setevent
```

And then, you can start the client right away.

```bash
abstouch start
```

You can run it on background if you use `--daemon` flag.

```bash
abstouch start -d
```

You can set cursor offset to match your touchpad.

```bash
abstouch setoffset
```

See help or man page for more information.

```bash
abstouch help
man abstouch
```
