import { ColorScheme, ColorSchemeProvider, MantineProvider } from "@mantine/core";
import { useLocalStorage } from "@mantine/hooks";
import { ReactNode, StrictMode } from "react";
import { createRoot } from "react-dom/client";
import theme from "./theme";

function Root({ children }: {children: ReactNode})
{
    const [user_theme, set_theme] = useLocalStorage<ColorScheme>({key: "theme", defaultValue: "dark", getInitialValueInEffect: true});
    const toggle_theme = (value?: ColorScheme) => set_theme(value || (user_theme === "dark" ? "light" : "dark"));

    return <StrictMode>
        <ColorSchemeProvider colorScheme={user_theme} toggleColorScheme={toggle_theme}>
            <MantineProvider theme={{ ...theme, colorScheme: user_theme }} withGlobalStyles withNormalizeCSS>
                    {children}
            </MantineProvider>
        </ColorSchemeProvider>
    </StrictMode>;
}

export default function create_root(children: ReactNode)
{
    return createRoot(document.getElementById("root")!).render(
        <Root>
            {children}
        </Root>
    );
}