# yambar-bluetooth

*A Bluetooth module for [Yambar](https://codeberg.org/dnkl/yambar) status pannel.*


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

The `yambar_bluetooth` program will monitor connections to a Bluetooth adapter. 

It produces the following tags that can be used by Yambar:

| Name        | Type   | Description                                                       |
| ----------- | ------ | ----------------------------------------------------------------- |
| powered     | bool   | Whether the adapter is powered on                                 |
| discovering | bool   | Whether the adapter is in discovering mode                        |
| connected   | bool   | Whether the monitored device is connected                         |
| address     | string | The MAC address of the monitored device (empty if none was found) |
| name        | string | The name of the monitored device (empty if none was found)        |
| icon        | string | The icon of the monitored device (empty if none was found)        |


## Configuration

The `yambar_bluetooth` command accepts two optional arguments:

| Option                       | Type   | Description                                                                                                            |
| ---------------------------- | ------ | ---------------------------------------------------------------------------------------------------------------------- |
| `--adapter-name <name>`      | string | The name of the Bluetooth adapter that will be monitored. By default, `"hci0"` is used.                                |
| `--device-address <address>` | string | The MAC address of a specific device to monitor. By default, the first device found to be connected will be monitored. |


See also `yambar_bluetooth --help`.

## Example

Here is a possible `config.yaml` for Yambar:

```yaml
bar:
  height: 32
  location: bottom
  background: 111111cc

  left:
    - script:
        path: /usr/bin/yambar_bluetooth
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
