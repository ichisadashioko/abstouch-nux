# abstouch-nux

## Usage

```bash
$ abstouch-nux <command> [options?]
```

### Additional Notes

* Make sure the user is in `input` group.
* If the input system doesn't work, make sure you use **X11 *instead of* Wayland** to run desktop environment on.

### Commands

#### help

* **Usage:** `abstouch-nux help`
* **Description:** Shows help text.

#### start

* **Usage:** `abstouch-nux start [options?]`
* **Description:** Starts the abstouch-nux input client.
* **Options:** See **[options](#start-1)** part.

#### stop

* **Usage:** `abstouch-nux stop`
* **Description:** Stops the abstouch-nux daemon if present. See **[daemon option for start](#-d----daemon)** command.

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

* **Descriptions:** Starts the abstouch-nux as daemon and makes it run at background. See **[stop](#stop)** command.
