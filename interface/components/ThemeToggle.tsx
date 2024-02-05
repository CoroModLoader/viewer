import { Group, Switch, rem, useComputedColorScheme, useMantineColorScheme } from "@mantine/core";
import { useTimeout } from "@mantine/hooks";
import { IconMoon, IconSun } from "@tabler/icons-react";
import { useRef, useState } from "react";

export function ThemeToggle()
{
    const { setColorScheme } = useMantineColorScheme();
    const computedColorScheme = useComputedColorScheme("light", { getInitialValueInEffect: true });

    const [ checked, setChecked ] = useState(computedColorScheme === "light");

    const checkedRef = useRef<boolean>();
    checkedRef.current = checked;

    const { start } = useTimeout(() =>
    {
        setColorScheme(checkedRef.current ? "light" : "dark");
    }, 100);

    return <Group
        wrap="nowrap"
        justify="space-between"
    >
        Color Theme
        <Switch
            size="md"
            onLabel={
                <IconSun style={{ width: rem(16), height: rem(16) }} />
            }
            offLabel={
                <IconMoon style={{ width: rem(16), height: rem(16) }} />
            }
            onChange={() =>
            {
                setChecked(!checked);
                start();
            }}
            checked={checked}
        />
    </Group>;
}
