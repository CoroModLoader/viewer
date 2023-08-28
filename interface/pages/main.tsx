import { Alert, Button, Divider, Flex, Modal, NavLink, ScrollArea, Stack, Tabs, TextInput } from "@mantine/core";
import { useDebouncedValue, useDisclosure, useViewportSize } from "@mantine/hooks";
import { Prism } from "@mantine/prism";
import { IconEye, IconFileImport, IconHourglassEmpty, IconSearch, IconSettings, IconX } from "@tabler/icons-react";
import { useEffect, useMemo, useRef, useState } from "react";
import { Virtuoso } from "react-virtuoso";
import { Settings } from "../components/settings";
import create_root from "../src/react";

function Viewer()
{
    const [files, set_files] = useState<string[]>([]);
    const [code, set_code] = useState<string | undefined>(undefined);

    const [selected, set_selected] = useState<string | undefined>(undefined);

    const [search, set_search] = useState<string>("");
    const [debounced_search] = useDebouncedValue(search, 500);

    const filtered = useMemo(() =>
    {
        const query = search.toLowerCase();
        const filter = (file: string) => file.toLowerCase().includes(query);

        return files.filter(filter);
    }, [files, debounced_search]);

    useEffect(() =>
    {
        if (!window.saucer)
        {
            return;
        }

        window.saucer.call<string[]>("list-files", []).then(set_files);
    }, []);

    useEffect(() =>
    {
        if (!window.saucer)
        {
            return;
        }

        window.saucer.call<string>("decompile", [selected]).then(set_code);
    }, [selected]);

    const width = useViewportSize().width - 365;
    const height = useViewportSize().height - 45;
    const viewport = useRef<HTMLDivElement>(null);

    return <Flex h={height}>
        <Stack p="xs">
            <TextInput
                value={search}
                placeholder="Search..."
                icon={<IconSearch size="1rem" />}
                onChange={e => set_search(e.currentTarget.value)}
            />

            <ScrollArea h={height} w={350} viewportRef={viewport}>
                <Virtuoso
                    totalCount={filtered.length}
                    customScrollParent={viewport.current ?? undefined}
                    itemContent={index =>
                    {
                        const file = filtered[index];

                        return <NavLink
                            label={file}
                            active={selected === file}
                            onClick={() => set_selected(file)}
                        />;
                    }}
                />
            </ScrollArea>
        </Stack>
        <Divider h="100%" orientation="vertical" />
        <ScrollArea w={width} h={height - 15} p="xs">
            {code === null &&
                <Alert title="Sorry!" color="red" icon={<IconX size="1rem" />}>
                    Failed to decompile! See the log for more details.
                </Alert>
            }
            {code === undefined &&
                <Alert title="Nothing here!" color="yellow" icon={<IconHourglassEmpty size="1rem" />}>
                    Select a file to decompile...
                </Alert>
            }
            {!!code &&
                <Prism withLineNumbers language="ocaml" w={width - 40}>
                    {code}
                </Prism>
            }
        </ScrollArea>
    </Flex>;
}

function App()
{
    const [opened, { open, close }] = useDisclosure(true);
    const [error, set_error] = useState(false);

    const locate = () =>
    {
        if (!window.saucer)
        {
            return;
        }

        window.saucer.call<boolean>("open-archive", []).then(status =>
        {
            set_error(status);

            if (!status)
            {
                return;
            }

            close();
        });
    };

    return <>
        <Modal title="Select Archive" opened={opened} onClose={open} withCloseButton={false} centered>
            <Stack>
                {error &&
                    <Alert title="Failed to open archive" color="red" icon={<IconX size="1rem" />}>
                        See log for more details
                    </Alert>
                }
                <Button leftIcon={<IconFileImport />} onClick={locate} fullWidth>
                    Open Archive
                </Button>
            </Stack>
        </Modal>

        {!opened &&
            <Tabs defaultValue="viewer" sx={{ height: "100vh", display: "flex", flexFlow: "column" }}>
                <Tabs.List h={45} sx={{ flex: "0 0 auto" }} grow>
                    <Tabs.Tab value="viewer" icon={<IconEye size="1rem" />}>Viewer</Tabs.Tab>
                    <Tabs.Tab value="settings" icon={<IconSettings size="1rem" />}>Settings</Tabs.Tab>
                </Tabs.List>

                <Tabs.Panel value="viewer" sx={{ flex: "1 1 auto" }}>
                    <Viewer />
                </Tabs.Panel>

                <Tabs.Panel value="settings">
                    <Settings />
                </Tabs.Panel>
            </Tabs>
        }
    </>;
}

create_root(<App />);