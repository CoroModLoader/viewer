interface Window
{
    saucer:
    {
        call: <Ret = any>(func: string, args: any[]) => Promise<Ret>;
    }

    update_notification: (val: number, limit: number) => void;
}