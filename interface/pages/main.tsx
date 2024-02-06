import { Grid } from "@mantine/core";
import create_root from "../src/react";
import { Sidebar } from "../components/Sidebar";
import { Code } from "../components/Code";
import { useDisclosure } from "@mantine/hooks";
import { Settings } from "../components/Settings";
import { useState } from "react";

export function Main()
{
    const [opened, { open, close }] = useDisclosure(false);
    const [code, setCode] = useState("");

    return <>
        <Settings
            opened={opened}
            close={close}
        />
        <Grid
            gutter={0}
            columns={5}
        >
            <Grid.Col span={3} h="100vh">
                <Code code={code} />
            </Grid.Col>

            <Grid.Col span={2} h="100vh">
                <Sidebar openSettings={open} setCode={setCode} />
            </Grid.Col>
        </Grid>
    </>;
}

create_root(<Main />);
