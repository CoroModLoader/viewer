import { CodeHighlight } from "@mantine/code-highlight";
import { ActionIcon, Group, ScrollArea, Stack, rem } from "@mantine/core";
import { useElementSize } from "@mantine/hooks";
import { IconCopy } from "@tabler/icons-react";
import classes from "../style/global.module.css";

export function Code({ code }: {code?: string})
{
    const { ref, width } = useElementSize();

    return <Stack
        pt="md"
        px="md"
        pb="sm"
        gap="sm"
        h="100%"
        ref={ref}
    >
        <ScrollArea
            h="100%"
            type="scroll"
            offsetScrollbars
            className={classes.accentBackground}
        >
            <CodeHighlight
                withCopyButton={false}
                bg="transparent"
                language="lua"
                code={
                    code || "-- Nothing here yet, use the load button in the bottom right to get started!"
                }
            />
        </ScrollArea>

        {code &&
            <Group wrap="nowrap"
                p="xs"
                w={width}
                pos="fixed"
                justify="end"
            >
                <ActionIcon color="gray" variant="transparent">
                    <IconCopy style={{ width: rem(16) }} />
                </ActionIcon>
            </Group>
        }
    </Stack>;
}
