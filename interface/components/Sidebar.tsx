import { ActionIcon, Center, CloseButton, Group, Loader, NavLink, RingProgress, ScrollArea, Stack, TextInput, Tooltip, TransitionProps, rem } from "@mantine/core";
import { useDebouncedValue } from "@mantine/hooks";
import { IconFileImport, IconFolderCode, IconSearch, IconSettings } from "@tabler/icons-react";
import { useEffect, useMemo, useRef, useState } from "react";
import { Virtuoso } from "react-virtuoso";
import { chooseFile, chooseFolder } from "../src/utils";
import classes from "../style/global.module.css";

export function Sidebar({ openSettings, setCode }: {openSettings: () => void, setCode: (code: string) => void})
{
    const transition: Partial<TransitionProps> = { transition: "pop" };

    const [data, setData] = useState<string[]>([]);
    const [dataLoading, setDataLoading] = useState(false);

    const importArchive = () =>
    {
        setDataLoading(true);

        chooseFile(result =>
        {
            window.saucer.call("load", [result]).then(data =>
            {
                setDataLoading(false);

                if (typeof data === "boolean")
                {
                    window.saucer.call("list", []).then(setData);
                    return;
                }

                // TODO: Error handling
            });
        }, () => setDataLoading(false));
    };

    useEffect(() =>
    {
        window.saucer.call("list", []).then(setData);
    }, []);

    const [selected, setSelected] = useState("");

    const [progress, setProgress] = useState(-1);
    const exportLoading = progress >= 0;

    const startExport = () =>
    {
        window.exportProgress = (current, max) => setProgress((current / max) * 100);
        chooseFolder(path => window.saucer.call("export", [path])); // TODO: Handle export return value
    };

    const [query, setQuery] = useState("");
    const [debouncedQuery] = useDebouncedValue(query, 250);

    const filtered = useMemo(() =>
    {
        const needle = debouncedQuery.toLowerCase();
        const filter = (item: string) => item.toLowerCase().includes(needle);

        return data.filter(filter);
    }, [data, debouncedQuery]);

    const scroller = useRef<HTMLDivElement>(null);

    return <Stack
        pt="md"
        px="md"
        pb="sm"
        gap="sm"
        h="100%"
    >
        <TextInput
            leftSection={
                <IconSearch style={{ width: rem(16), height: rem(16) }} />
            }
            variant="filled"
            placeholder="Search..."
            onChange={e => setQuery(e.currentTarget.value)}
        />

        <ScrollArea
            h="100%"
            viewportRef={scroller}
            className={classes.accentBackground}
        >
            <Virtuoso
                totalCount={filtered.length}
                customScrollParent={scroller.current ?? undefined}
                itemContent={index =>
                {
                    const item = filtered[index];

                    const update = () =>
                    {
                        setSelected(item);
                        window.saucer.call("decompile", [item]).then(setCode);
                    };

                    return <Tooltip
                        label={item}
                        openDelay={200}
                    >
                        <NavLink
                            key={item}
                            label={item}
                            onClick={update}
                            active={selected === item}
                        />
                    </Tooltip>;
                }}
            />
        </ScrollArea>

        <Group
            mt={5}
            mb="sm"
            wrap="nowrap"
            justify="space-evenly"
        >
            <Tooltip label="Load Resource" transitionProps={transition}>
                {dataLoading ?
                    <Loader size={43} type="dots" />
                    :
                    <ActionIcon onClick={importArchive} variant="light" radius="xl" size="xl">
                        <IconFileImport />
                    </ActionIcon>
                }
            </Tooltip>

            <Tooltip label={exportLoading ? `Exporting Archive (${progress.toPrecision(2)}%)` : "Export Archive"} transitionProps={transition}>
                {exportLoading ?
                    <RingProgress
                        size={45}
                        thickness={5}
                        roundCaps
                        sections={[{ value: progress, color: "blue" }]}
                        label={
                            <Center>
                                <CloseButton
                                    size="xs"
                                    variant="transparent"
                                    onClick={() => window.saucer.call("cancel", [])}
                                />
                            </Center>
                        }
                    />
                    :
                    <ActionIcon onClick={startExport} disabled={data.length <= 0} variant="light" radius="xl" size="xl">
                        <IconFolderCode />
                    </ActionIcon>
                }
            </Tooltip>

            <Tooltip label="Settings" transitionProps={transition}>
                <ActionIcon onClick={openSettings} variant="light" radius="xl" size="xl">
                    <IconSettings />
                </ActionIcon>
            </Tooltip>
        </Group>
    </Stack>;
}
