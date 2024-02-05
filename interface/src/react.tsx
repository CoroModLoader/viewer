import { ReactNode } from "react";
import { createRoot } from "react-dom/client";
import { Root } from "../components/Root";

export default function create_root(children: ReactNode)
{
    return createRoot(document.getElementById("root")!).render(
        <Root>
            {children}
        </Root>
    );
}
