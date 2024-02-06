interface Window
{
    saucer:
    {
        call:
        {
            (func: "load", args: [string: path]): Promise<boolean | string>;

            (func: "list", args: []): Promise<string[]>;
            (func: "decompile", args: [string: name]): Promise<string>;

            (func: "cancel", args: []): Promise<void>;
            (func: "export", args: [string: path]): Promise<boolean>;

            (func: "java-path", args: []): Promise<string?>;
            (func: "unluac-path", args: []): Promise<string?>;

            (func: "set-java-path", args: [string?: path]): Promise<void>;
            (func: "set-unluac-path", args: [string?: path]): Promise<void>;

            (func: "choose-path", args: [string?: path]): Promise<string | null>;
            (func: "choose-folder", args: [string?: path]): Promise<string | null>;
        };
    }

    exportProgress: (current: number, max: number) => void;
}
