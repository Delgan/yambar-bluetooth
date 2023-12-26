#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>

#define streq(a, b) (strcmp((a), (b)) == 0)

int main()
{
    sd_bus *bus = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;

    int ret = 0;

    ret = sd_bus_open_system(&bus);
    if (ret < 0)
    {
        fprintf(stderr, "Unable to connect to the system bus: %s\n",
                strerror(-ret));
        goto finish;
    }

    ret = sd_bus_call_method(bus, "org.bluez", "/",
                             "org.freedesktop.DBus.ObjectManager",
                             "GetManagedObjects", &error, &reply, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to call method: %s\n", strerror(-ret));
        goto finish;
    }

    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{oa{sa{sv}}}");
    if (ret < 0)
    {
        fprintf(stderr, "Failed to enter array container: %s\n", strerror(-ret));
        goto finish;
    }

    for (;;)
    {
        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "oa{sa{sv}}");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter dict container: %s\n", strerror(-ret));
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
            fprintf(stderr, "Failed to read object path: %s\n", strerror(-ret));
            goto finish;
        }

        ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sa{sv}}");
        if (ret < 0)
        {
            fprintf(stderr, "Failed to enter array container: %s\n", strerror(-ret));
            goto finish;
        }

        for (;;)
        {
            ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sa{sv}");
            if (ret < 0)
            {
                fprintf(stderr, "Failed to enter dict container: %s\n", strerror(-ret));
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
                fprintf(stderr, "Failed to read interface: %s\n", strerror(-ret));
                goto finish;
            }

            ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "{sv}");
            if (ret < 0)
            {
                fprintf(stderr, "Failed to enter array container: %s\n", strerror(-ret));
                goto finish;
            }

            for (;;)
            {
                ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
                if (ret < 0)
                {
                    fprintf(stderr, "Failed to enter dict container: %s\n", strerror(-ret));
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
                    fprintf(stderr, "Failed to read property: %s\n", strerror(-ret));
                    goto finish;
                }

                const char *contents;
                ret = sd_bus_message_peek_type(reply, NULL, &contents);
                if (ret < 0)
                {
                    fprintf(stderr, "Failed to peek type: %s\n", strerror(-ret));
                    goto finish;
                }

                if (streq(contents, "t") || streq(contents, "s") || streq(contents, "n") || streq(contents, "q") || streq(contents, "i") || streq(contents, "u") || streq(contents, "b") || streq(contents, "o") || streq(contents, "y"))
                {
                    ret = sd_bus_message_enter_container(reply, SD_BUS_TYPE_VARIANT, contents);
                    if (ret < 0)
                    {
                        fprintf(stderr, "Failed to enter variant container: %s\n", strerror(-ret));
                        goto finish;
                    }

                    if (streq(contents, "t"))
                    {
                        uint64_t value;
                        ret = sd_bus_message_read(reply, "t", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read uint64: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (t) %llu\n", path, interface, property, value);
                    }
                    else if (streq(contents, "n"))
                    {
                        int16_t value;
                        ret = sd_bus_message_read(reply, "n", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read int16: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (n) %d\n", path, interface, property, value);
                    }
                    else if (streq(contents, "q"))
                    {
                        uint16_t value;
                        ret = sd_bus_message_read(reply, "q", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read uint16: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (q) %d\n", path, interface, property, value);
                    }
                    else if (streq(contents, "y"))
                    {
                        uint8_t value;
                        ret = sd_bus_message_read(reply, "y", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read uint8: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (y) %d\n", path, interface, property, value);
                    }
                    else if (streq(contents, "s"))
                    {
                        const char *value;
                        ret = sd_bus_message_read(reply, "s", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read string: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (s) %s\n", path, interface, property, value);
                    }
                    else if (streq(contents, "o"))
                    {
                        const char *value;
                        ret = sd_bus_message_read(reply, "o", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read object: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (o) %s\n", path, interface, property, value);
                    }
                    else if (streq(contents, "b"))
                    {
                        int value;
                        ret = sd_bus_message_read(reply, "b", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read boolean: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (b) %s\n", path, interface, property, value ? "true" : "false");
                    }
                    else if (streq(contents, "i"))
                    {
                        int32_t value;
                        ret = sd_bus_message_read(reply, "i", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read int32: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (i) %d\n", path, interface, property, value);
                    }
                    else if (streq(contents, "u"))
                    {
                        uint32_t value;
                        ret = sd_bus_message_read(reply, "u", &value);
                        if (ret < 0)
                        {
                            fprintf(stderr, "Failed to read uint32: %s\n", strerror(-ret));
                            goto finish;
                        }
                        fprintf(stdout, "%s.%s.%s = (u) %u\n", path, interface, property, value);
                    }

                    ret = sd_bus_message_exit_container(reply);
                    if (ret < 0)
                    {
                        fprintf(stderr, "Failed to exit container: %s\n", strerror(-ret));
                        goto finish;
                    }
                }
                else
                {
                    ret = sd_bus_message_skip(reply, "v");
                    if (ret < 0)
                    {
                        fprintf(stderr, "Failed to skip variant: %s\n", strerror(-ret));
                        goto finish;
                    }

                    fprintf(stdout, "%s.%s.%s = (%s) <?>\n", path, interface, property, contents);
                }

                ret = sd_bus_message_exit_container(reply);
                if (ret < 0)
                {
                    fprintf(stderr, "Failed to exit container: %s\n", strerror(-ret));
                    goto finish;
                }
            }

            ret = sd_bus_message_exit_container(reply);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to exit container: %s\n", strerror(-ret));
                goto finish;
            }

            ret = sd_bus_message_exit_container(reply);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to exit container: %s\n", strerror(-ret));
                goto finish;
            }
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit container: %s\n", strerror(-ret));
            goto finish;
        }

        ret = sd_bus_message_exit_container(reply);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to exit container: %s\n", strerror(-ret));
            goto finish;
        }
    }

    ret = sd_bus_message_exit_container(reply);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to exit array container: %s\n", strerror(-ret));
        goto finish;
    }

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(reply);
    sd_bus_unref(bus);

    return ret;
}