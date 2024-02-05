// Import styles of packages that you've installed.
// All packages except `@mantine/hooks` require styles imports
import "@mantine/core/styles.css";
import "@mantine/code-highlight/styles.css";

import { MantineProvider } from "@mantine/core";
import { ReactNode, StrictMode } from "react";
import { theme } from "../src/theme";

export function Root({ children }: {children: ReactNode})
{
    return <StrictMode>
        <MantineProvider theme={theme}>
            {children}
        </MantineProvider>
    </StrictMode>;
}
