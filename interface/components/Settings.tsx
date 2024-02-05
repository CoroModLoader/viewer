import { ActionIcon, Button, Group, Input, Modal, Stack, rem } from "@mantine/core";
import { IconFileSearch, IconTrash } from "@tabler/icons-react";
import { useEffect, useState } from "react";
import { ThemeToggle } from "./ThemeToggle";

export function Settings({ opened, close }: {opened: boolean, close: () => void})
{
    const [javaPath, setJavaPath] = useState("");
    const [unluacPath, setUnluacPath] = useState("");

    useEffect(() =>
    {
        window.saucer.call("java-path", []).then(setJavaPath);
        window.saucer.call("unluac-path", []).then(setUnluacPath);
    }, []);

    useEffect(() =>
    {
        window.saucer.call("set-java-path", [javaPath]);
    }, [javaPath]);

    useEffect(() =>
    {
        window.saucer.call("set-unluac-path", [unluacPath]);
    }, [unluacPath]);

    const chooseFile = () =>
    {
        return window.saucer.call("choose-path", []);
    };

    return <Modal
        opened={opened}
        onClose={close}
        title="Settings"
        centered
    >
        <Stack>
            <ThemeToggle />

            <Input.Wrapper
                label="Java Path"
                description="Location of the Java Binary"
            >
                <Input
                    value={javaPath}
                    onChange={e => setJavaPath(e.currentTarget.value)}
                    placeholder="Default (Java in Path)"
                    rightSectionPointerEvents="all"
                    rightSection={
                        <ActionIcon onClick={() => chooseFile().then(setJavaPath)} color="gray" variant="transparent">
                            <IconFileSearch style={{ width: rem(18) }} />
                        </ActionIcon>
                    }
                />
            </Input.Wrapper>


            <Input.Wrapper
                label="Unluac Path"
                description="Location of the Unluac Jar"
            >
                <Input
                    value={unluacPath}
                    onChange={e => setUnluacPath(e.currentTarget.value)}
                    error={!unluacPath}
                    placeholder="..."
                    rightSectionPointerEvents="all"
                    rightSection={
                        <ActionIcon onClick={() => chooseFile().then(setUnluacPath)} color="gray" variant="transparent">
                            <IconFileSearch style={{ width: rem(18) }} />
                        </ActionIcon>
                    }
                />
            </Input.Wrapper>

            <Group justify="end">
                <Button
                    leftSection={
                        <IconTrash style={{ width: rem(16), height: rem(16) }} />
                    }
                    color="red"
                    onClick={close}
                >
                        Restore Defaults
                </Button>
            </Group>
        </Stack>
    </Modal>;
}
