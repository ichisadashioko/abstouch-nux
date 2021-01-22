# abstouch-nux

## Usage

```bash
$ abstouch-nux <command> [options?]
```

### Commands

#### help

* **Usage:** `abstouch-nux help`
* **Description:** Shows help text.

#### start

* **Usage:** `abstouch-nux start [options?]`
* **Description:** Starts the abstouch-nux input client.
* **Options:** See **[options](https://github.com/acedron/abstouch-nux/blob/master/doc/usage.md#options)** part.

#### stop

* **Usage:** `abstouch-nux stop`
* **Description:** Stops the abstouch-nux daemon if present. See **[daemon option for start]()** command.

#### calibrate

* **Usage:** `abstouch-nux calibrate`
* **Description:** Sets the touch offset and calibrates automatically.

#### setevent

* **Usage:** `abstouch-nux setevent`
* **Description:** Sets the input event to use for abstouch-nux input.

#### setoffset

* **Usage:** `abstouch-nux setoffset`
* **Description:** Sets the touch offset manually.

### Options

#### start

##### -q / --quiet

* **Description:** Disables verbose output and doesn't show any output except errors.

##### -d / --daemon

* **Descriptions:** Starts the abstouch-nux as daemon and makes it run at background. See **[stop]()** command.
