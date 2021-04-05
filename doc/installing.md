# abstouch-nux

## Installing

Only available for **GNU/Linux**.

### Installing Dependencies

The dependencies are:

* **gcc**
* **make**
* **autoconf**
* **automake**
* **libx11-dev** (*libx11* or *libX11-devel* On Some Systems)

#### Arch Linux (Manjaro, Artix, etc.)

```bash
$ sudo pacman -Sy make gcc autoconf automake libx11 --needed
```

#### Debian (Ubuntu, Kali, Mint, etc.)

```bash
$ sudo apt-get update && sudo apt-get make install gcc autoconf automake libx11-dev
```

#### Fedora (CentOS, Red Hat etc.)

```bash
$ sudo dnf install make gcc autoconf automake libX11-devel 
```

#### openSUSE

```bash
$ sudo zypper install make gcc autoconf automake libX11-devel
```

### Building and Installing

* See **[dependencies](#installing-dependencies)** first.
* You can build using ***autotools*** and ***make***.
* Don't forget to add user to `input` group.

```bash
$ git clone https://github.com/acedron/abstouch-nux.git
$ cd abstouch-nux
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
$ sudo usermod -aG input $(whoami)
```

If you want to uninstall, just replace the `install` with `uninstall`.

## Important

* Make sure the user is in `input` group, or else you have to run it using `sudo`.
* If the input system doesn't work, make sure you use **X11 / X.org *instead of* Wayland** to run desktop environment on.
