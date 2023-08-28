import { Box, Button, Center, ColorScheme, Divider, Grid, Group, SegmentedControl, Stack, Text, useMantineColorScheme } from "@mantine/core";
import { IconMoon, IconSun, TablerIconsProps } from "@tabler/icons-react";
import { ReactNode, useEffect, useState } from "react";

function IconText({ Icon, text }: {Icon: (props: TablerIconsProps) => JSX.Element, text: string})
{
    return <Center>
        <Icon size={12} />
        <Box ml={5}>{text}</Box>
    </Center>;
}

type SettingProps<Props extends object> = Omit<Props, "label" | "description"> &
{
    label: string;
    description?: string;
    Component: (props: Props) => ReactNode;
};

function Setting<Props extends object>({ label, description, Component, ...props }: SettingProps<Props>)
{
    return <Grid m={0} p="xs" columns={10} align="center">
        <Grid.Col span={8}>
            <Stack spacing={0} justify="center">
                <Text>{label}</Text>
                {description && <Text c="dimmed">{description}</Text>}
            </Stack>
        </Grid.Col>
        <Grid.Col span={2}>
            <Group position="right" align="center" noWrap>
                <Divider orientation="vertical" h={50} />
                <Component w={250} {...props as Props} />
            </Group>
        </Grid.Col>
    </Grid>;
}

export function Settings()
{
    const { colorScheme, toggleColorScheme } = useMantineColorScheme();
    const [unluac, set_unluac] = useState("");
    const [java, set_java] = useState("");

    useEffect(() =>
    {
        if (!window.saucer)
        {
            return;
        }

        window.saucer.call<string>("java-path", []).then(set_java);
        window.saucer.call<string>("unluac-path", []).then(set_unluac);
    }, []);

    const update_java = () =>
    {
        window.saucer.call<string>("update-java-path", []).then(set_java);
    };

    const update_unluac = () =>
    {
        window.saucer.call<string>("update-unluac-path", []).then(set_unluac);
    };

    return <Stack spacing={0}>
        <Setting
            label="Theme"
            Component={SegmentedControl}
            onChange={(value: ColorScheme) => toggleColorScheme(value)}
            value={colorScheme}
            data={[
                { label: <IconText Icon={IconMoon} text="Dark" />, value: "dark" },
                { label: <IconText Icon={IconSun} text="Light" />, value: "light" }
            ]}
        />
        <Setting label="Java Path" description={`Location of the Java Binary ("${java}")`} Component={Button<"button">} onClick={update_java}>Locate</Setting>
        <Setting label="Unluac Path" description={`Location of the Unluac Binary ("${unluac}")`} Component={Button<"button">} onClick={update_unluac}>Locate</Setting>
    </Stack>;
}