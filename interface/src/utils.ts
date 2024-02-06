export function chooseFile(setter: (value: string) => void, cancel?: () => void)
{
    window.saucer.call("choose-path", []).then(result =>
    {
        if (result)
        {
            setter(result);
            return;
        }

        if (!cancel)
        {
            return;
        }

        cancel();
    });
}

export function chooseFolder(setter: (value: string) => void)
{
    window.saucer.call("choose-folder", []).then(result =>
    {
        if (!result)
        {
            return;
        }

        setter(result);
    });
}
