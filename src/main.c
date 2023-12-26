#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>

#define str_eq(a, b) (strcmp((a), (b)) == 0)

struct monitoring_config
{
    const char *adapter_object_path; // D-Bus path of the adapter. Can't be NULL.
    const char *device_mac_address;  // MAC address of the device. If NULL, all devices are monitored.
};

struct adapter_info
{
    bool powered;
    bool discovering;
};

struct device_info
{
    bool connected;
    const char *mac;
    const char *name;
    const char *icon;
};

static void init_adapter_info(struct adapter_info *adapter)
{
    adapter->powered = false;
    adapter->discovering = false;
}

static void init_device_info(struct device_info *device)
{
    device->connected = false;
    device->mac = "";
    device->name = "";
    device->icon = "";
}

static int read_boolean_variant(sd_bus_message *reply, bool *value)
{
    int ret = 0;

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_VARIANT, "b");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter boolean variant container");
        return ret;
    }

    int intValue; // Documentation requires 'int' and not 'bool'.
    ret = sd_bus_message_read(reply, "b", &intValue);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to read boolean variant\n");
        return ret;
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit boolean variant container");
        return ret;
    }

    *value = intValue;
    return 0;
}

static int read_object_path_variant(sd_bus_message *reply, const char **value)
{
    int ret = 0;

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_VARIANT, "o");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter object path variant container");
        return ret;
    }

    ret = sd_bus_message_read(reply, "o", value);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to read object path variant\n");
        return ret;
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit object path variant container");
        return ret;
    }

    return 0;
}

static int read_string_variant(sd_bus_message *reply, const char **value)
{
    int ret = 0;

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_VARIANT, "s");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter string variant container");
        return ret;
    }

    ret = sd_bus_message_read(reply, "s", value);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to read string variant\n");
        return ret;
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit string variant container");
        return ret;
    }

    return 0;
}

static int parse_adapter_properties(sd_bus_message *reply, struct adapter_info *output)
{
    int ret = 0;

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sv}");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter properties array of adapter\n");
        return ret;
    }

    for (;;)
    {
        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter dict entry of adapter properties\n");
            return ret;
        }
        if (ret == 0)
        {
            break;
        }

        const char *property;
        ret = sd_bus_message_read(reply, "s", &property);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to read adapter property name\n");
            return ret;
        }

        if (str_eq(property, "Powered"))
        {
            ret = read_boolean_variant(reply, &output->powered);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read value of 'Powered' property\n");
                return ret;
            }
        }
        else if (str_eq(property, "Discovering"))
        {
            ret = read_boolean_variant(reply, &output->discovering);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read value of 'Discovering' property\n");
                return ret;
            }
        }
        else
        {
            ret = sd_bus_message_skip(reply, "v");
            if (ret < 0)
            {
                fprintf(stderr, "Failed to skip variant\n");
                return ret;
            }
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit dict entry of adapter property\n");
            return ret;
        }
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit properties array of adapter\n");
        return ret;
    }

    return 0;
}

static int parse_device_properties(sd_bus_message *reply, struct device_info *output)
{
    int ret = 0;

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sv}");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter properties array of device\n");
        return ret;
    }

    for (;;)
    {
        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter dict entry of device properties\n");
            return ret;
        }
        if (ret == 0)
        {
            break;
        }

        const char *property;
        ret = sd_bus_message_read(reply, "s", &property);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to read device property name\n");
            return ret;
        }

        if (str_eq(property, "Connected"))
        {
            ret = read_boolean_variant(reply, &output->connected);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read value of 'Connected' property\n");
                return ret;
            }
        }
        else if (str_eq(property, "Name"))
        {
            ret = read_string_variant(reply, &output->name);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read value of 'Name' property\n");
                return ret;
            }
        }
        else if (str_eq(property, "Icon"))
        {
            ret = read_string_variant(reply, &output->icon);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read value of 'Icon' property\n");
                return ret;
            }
        }
        else if (str_eq(property, "Address"))
        {
            ret = read_string_variant(reply, &output->mac);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read value of 'Address' property\n");
                return ret;
            }
        }
        else
        {
            ret = sd_bus_message_skip(reply, "v");
            if (ret < 0)
            {
                fprintf(stderr, "Failed to skip variant\n");
                return ret;
            }
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit dict entry of adapter property\n");
            return ret;
        }
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit properties array of adapter\n");
        return ret;
    }

    return 0;
}

static int fetch_bluetooth_state(sd_bus *bus, const struct monitoring_config *config)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;

    int ret = 0;

    struct adapter_info adapter_info;
    bool adapter_found = false;
    init_adapter_info(&adapter_info);

    struct device_info device_info;
    bool device_found = false;
    init_device_info(&device_info);

    ret = sd_bus_call_method(bus, "org.bluez", "/",
                             "org.freedesktop.DBus.ObjectManager",
                             "GetManagedObjects", &error, &reply, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to call 'ObjectManager' method\n");
        goto finish;
    }

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{oa{sa{sv}}}");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter objects array\n");
        goto finish;
    }

    for (;;)
    {
        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "oa{sa{sv}}");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to object dict entry\n");
            goto finish;
        }
        if (ret == 0)
        {
            break;
        }

        const char *path;
        ret = sd_bus_message_read(reply, "o", &path);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to read object path\n");
            goto finish;
        }

        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sa{sv}}");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter interfaces array\n");
            goto finish;
        }

        for (;;)
        {
            ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sa{sv}");
            if (ret < 0)
            {
                fprintf(stderr, "Failed to enter interface dict entry\n");
                goto finish;
            }
            if (ret == 0)
            {
                break;
            }

            const char *interface;
            ret = sd_bus_message_read(reply, "s", &interface);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to read interface name\n");
                goto finish;
            }

            if (!adapter_found && str_eq(interface, "org.bluez.Adapter1") && str_eq(path, config->adapter_object_path))
            {
                struct adapter_info adapter;
                init_adapter_info(&adapter);
                ret = parse_adapter_properties(reply, &adapter);
                if (ret < 0)
                {
                    fprintf(stderr, "Failed to parse adapter properties\n");
                    goto finish;
                }
                adapter_info = adapter;
                adapter_found = true;
            }
            else if (!device_found && str_eq(interface, "org.bluez.Device1"))
            {
                struct device_info device;
                init_device_info(&device);
                ret = parse_device_properties(reply, &device);
                if (ret < 0)
                {
                    fprintf(stderr, "Failed to parse device properties\n");
                    goto finish;
                }
                if ((config->device_mac_address == NULL && device.connected) ||
                    (config->device_mac_address != NULL && str_eq(device.mac, config->device_mac_address)))
                {
                    device_info = device;
                    device_found = true;
                }
            }
            else
            {
                ret = sd_bus_message_skip(reply, "a{sv}");
                if (ret < 0)
                {
                    fprintf(stderr, "Failed to skip interface entry\n");
                    goto finish;
                }
            }

            ret = sd_bus_message_exit_container(reply);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to exit interface entry\n");
                goto finish;
            }
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit interfaces array\n");
            goto finish;
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit object dict entry\n");
            goto finish;
        }
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit objects array\n");
        goto finish;
    }

    fprintf(stdout, "powered|bool|%s\n", adapter_info.powered ? "true" : "false");
    fprintf(stdout, "discovering|bool|%s\n", adapter_info.discovering ? "true" : "false");
    fprintf(stdout, "connected|bool|%s\n", device_info.connected ? "true" : "false");
    fprintf(stdout, "mac|string|%s\n", device_info.mac);
    fprintf(stdout, "name|string|%s\n", device_info.name);
    fprintf(stdout, "icon|string|%s\n", device_info.icon);
    fprintf(stdout, "\n");
    fflush(stdout);

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(reply);

    return ret;
}

static int on_device_properties_changed(sd_bus_message *reply, void *config, sd_bus_error *ret_error)
{
    (void)ret_error;

    sd_bus *bus = sd_bus_message_get_bus(reply);

    int ret = 0;

    const char *interface;
    ret = sd_bus_message_read(reply, "s", &interface);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to read interface name\n");
        goto finish;
    }

    if (!str_eq(interface, "org.bluez.Device1"))
    {
        goto finish;
    }

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sv}");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter properties changed array\n");
        goto finish;
    }

    bool refresh = false;

    for (;;)
    {
        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter dict entry of device properties changed\n");
            goto finish;
        }
        if (ret == 0)
        {
            break;
        }

        const char *property;
        ret = sd_bus_message_read(reply, "s", &property);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to read device property name\n");
            goto finish;
        }

        fprintf(stderr, "Property changed: %s\n", property);

        if (str_eq(property, "Connected"))
        {
            refresh = true;
        }

        ret = sd_bus_message_skip(reply, "v");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to skip variant\n");
            goto finish;
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit dict entry of device property\n");
            goto finish;
        }
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit properties changed array\n");
        goto finish;
    }

    if (refresh)
    {
        ret = fetch_bluetooth_state(bus, (struct monitoring_config *)config);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to fetch bluetooth state\n");
            goto finish;
        }
    }

finish:
    if (ret < 0)
    {
        fprintf(stderr, "Error (%d): %s\n", ret, strerror(-ret));
    }

    return ret;
}

static int on_adapter_properties_changed(sd_bus_message *reply, void *config, sd_bus_error *ret_error)
{
    (void)ret_error;

    sd_bus *bus = sd_bus_message_get_bus(reply);

    int ret = 0;

    const char *interface;
    ret = sd_bus_message_read(reply, "s", &interface);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to read interface name\n");
        goto finish;
    }

    if (!str_eq(interface, "org.bluez.Adapter1"))
    {
        goto finish;
    }

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sv}");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter properties changed array\n");
        goto finish;
    }

    bool refresh = false;

    for (;;)
    {
        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter dict entry of device properties changed\n");
            goto finish;
        }
        if (ret == 0)
        {
            break;
        }

        const char *property;
        ret = sd_bus_message_read(reply, "s", &property);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to read device property name\n");
            goto finish;
        }

        fprintf(stderr, "Property changed: %s\n", property);

        if (str_eq(property, "Powered") || str_eq(property, "Discovering"))
        {
            refresh = true;
        }

        ret = sd_bus_message_skip(reply, "v");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to skip variant\n");
            goto finish;
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit dict entry of device property\n");
            goto finish;
        }
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit properties changed array\n");
        goto finish;
    }

    if (refresh)
    {
        ret = fetch_bluetooth_state(bus, (struct monitoring_config *)config);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to fetch bluetooth state\n");
            goto finish;
        }
    }

finish:
    if (ret < 0)
    {
        fprintf(stderr, "Error (%d): %s\n", ret, strerror(-ret));
    }

    return ret;
}

int main()
{
    struct monitoring_config config;
    config.adapter_object_path = "/org/bluez/hci0";
    config.device_mac_address = NULL;

    sd_bus *bus = NULL;
    int ret = 0;

    ret = sd_bus_open_system(&bus);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to connect to the system bus\n");
        goto finish;
    }

    ret = fetch_bluetooth_state(bus, &config);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to fetch bluetooth state\n");
        goto finish;
    }

    ret = sd_bus_add_match(bus,
                           NULL,
                           "type='signal',sender='org.bluez',interface='org.freedesktop.DBus.Properties',member='PropertiesChanged',arg0namespace='org.bluez.Device1'",
                           on_device_properties_changed,
                           &config);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to add match for device properties changed\n");
        goto finish;
    }

    ret = sd_bus_add_match(bus,
                           NULL,
                           "type='signal',sender='org.bluez',interface='org.freedesktop.DBus.Properties',member='PropertiesChanged',arg0namespace='org.bluez.Adapter1'",
                           on_adapter_properties_changed,
                           &config);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to add match for adapter properties changed\n");
        goto finish;
    }

    while (true)
    {
        ret = sd_bus_process(bus, NULL);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to process bus\n");
            goto finish;
        }
        if (ret > 0)
        {
            continue;
        }
        ret = sd_bus_wait(bus, UINT64_MAX);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to wait on bus\n");
            goto finish;
        }
    }

finish:
    if (ret < 0)
    {
        fprintf(stderr, "Error (%d): %s\n", ret, strerror(-ret));
    }

    sd_bus_unref(bus);

    return ret;
}