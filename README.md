# yambar-bluetooth

*A Bluetooth module for [Yambar](https://codeberg.org/dnkl/yambar) status panel.*


## Installation

First build the software:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make
```

Then install it:

```bash
sudo make install
```

## Usage

The `yambar-bluetooth` program will observe connections to a Bluetooth adapter. 

It produces the following tags that can be used by Yambar:

| Name        | Type   | Description                                                      |
| ----------- | ------ | ---------------------------------------------------------------- |
| powered     | bool   | Whether the adapter is powered on                                |
| discovering | bool   | Whether the adapter is in discovering mode                       |
| connected   | bool   | Whether the observed device is connected                         |
| address     | string | The MAC address of the observed device (empty if none was found) |
| name        | string | The name of the observed device (empty if none was found)        |
| icon        | string | The icon of the observed device (empty if none was found)        |


## Configuration

The `yambar-bluetooth` command accepts two optional arguments:

| Option                       | Type   | Description                                                                                                           |
| ---------------------------- | ------ | --------------------------------------------------------------------------------------------------------------------- |
| `--adapter-name <name>`      | string | The name of the Bluetooth adapter that will be observed. By default, `"hci0"` is used.                                |
| `--device-address <address>` | string | The MAC address of a specific device to observe. By default, the first device found to be connected will be observed. |


See also `yambar-bluetooth --help`.

## Example

Here is a possible `config.yaml` for Yambar:

```yaml
bar:
  height: 32
  location: bottom
  background: 111111cc

  left:
    - script:
        path: /usr/bin/yambar-bluetooth
        content:
          map:
            conditions:
              connected:
                string:
                  text: "[Bluetooth ON] {name} ({address})"
              ~connected:
                string:
                  text: "[Bluetooth OFF] No device"
```
