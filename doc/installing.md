# abstouch-nux

## Installing

Only available for **GNU/Linux**.

### Installing Dependencies

The dependencies are:

* **gcc**
* **autoconf**
* **automake**
* **libx11-dev** (*libx11* or *libX11-devel* On Some Systems)

#### Arch Linux (Manjaro, Artix, etc.)

```bash
$ sudo pacman -Sy gcc autoconf automake libx11 --needed
```

#### Debian (Ubuntu, Kali, Mint, etc.)

```bash
$ sudo apt-get update && sudo apt-get install gcc autoconf automake libx11-dev
```

#### Fedora (CentOS, Red Hat, openSUSE, etc.)

```bash
$ sudo dnf install gcc autoconf automake libX11-devel
```

### Building and Installing

You can build using ***autotools*** and ***make***.

```bash
$ git clone https://github.com/acedron/abstouch-nux.git
$ cd abstouch-nux
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
```

If you want to uninstall, just replace the `install` with `uninstall`.
