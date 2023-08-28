interface Window
{
    saucer:
    {
        call: <Ret = any>(func: string, args: any[]) => Promise<Ret>;
    }
}